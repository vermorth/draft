#include "ini_parser.hh"

namespace draft {

namespace ini {
const std::string kWHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(kWHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(kWHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

int IniParser::CheckOptions() {
    // 注释前缀必须非空
    if (options_.allow_inline_comments_ &&
        options_.inline_comments_prefixes_.empty()) {
        return IniOptionsError_InvalidInlineCommentsPrefixes;
    }
    if (options_.allow_line_comments_ &&
        options_.line_comments_prefixes_.empty()) {
        return IniOptionsError_InvalidLineCommentsPrefixes;
    }

    if (options_.allow_inline_comments_ && options_.allow_line_comments_) {
        // 行末注释的前缀字符，必须是整行注释的子集
        for (auto c : options_.inline_comments_prefixes_) {
            if (options_.line_comments_prefixes_.find_first_of(c) ==
                std::string::npos) {
                return IniOptionsError_InvalidInlineCommentsPrefixes;
            }
        }
    }

    // value前缀必须非空
    if (options_.value_prefixes_.empty()) {
        return IniOptionsError_InvalidValuePrefixes;
    }
    return 0;
}

bool IniParser::IsLineComment(const std::string &line) {
    for (char c : options_.line_comments_prefixes_) {
        if (line[0] == c) {
            return true;
        }
    }
    return false;
}

int IniParser::TrimInlineComment(const std::string &line,
                                 std::string &trim_line) {
    uint64_t min_pos(UINT64_MAX);
    for (char c : options_.inline_comments_prefixes_) {
        size_t pos = 1;
        for (; pos < line.size(); pos++) {
            if (line[pos] == c && line[pos - 1] == ' ' ||
                line[pos - 1] == '\t' && pos < min_pos) {
                min_pos = pos;
            }
        }
    }
    if (min_pos < line.size() && min_pos > 0) {
        trim_line = line.substr(0, min_pos - 1);
        trim_line = rtrim(trim_line);
    } else {
        trim_line = line;
    }
    return 0;
}

int IniParser::Parse(std::istream &input) {
    int ret(0);
    ret = CheckOptions();
    if (ret != 0) {
        return ret;
    }

    uint32_t lineno(0);
    std::string section("");
    std::string prev_name("");
    std::string name("");
    std::string value("");
    std::string line("");

    while (getline(input, line)) {
        if (options_.stop_on_first_error_ && not error_map_.empty()) {
            return IniParseError;
        }
        lineno++;
        auto trim_line = trim(line);
        // 几种情况:
        //   空行
        //   整行注释
        //   多行value的value部分 带/不带 行末注释
        //   section行 带/不带 行末注释
        //   key value行 带/不带 行末注释
        //   格式错误的行
        if (trim_line.empty()) {
            // 空行
            continue;
        } else if (IsLineComment(trim_line)) {
            // 整行注释
            continue;
        } else {
            // 带行末注释的情况，消除行末注释
            std::string no_comment_line;
            if (options_.allow_inline_comments_) {
                ret = TrimInlineComment(trim_line, no_comment_line);
                if (ret != 0) {
                    error_map_[lineno] = std::make_pair(
                        line, "trim inline comment ret " + std::to_string(ret));
                    prev_name.clear();
                    continue;
                }
            } else {
                no_comment_line = trim_line;
            }
            if (no_comment_line.empty()) {
                continue;
            }
            if (options_.allow_multiline_ && not prev_name.empty() &&
                    line[0] == ' ' ||
                line[0] == '\t') {
                // 多行value的value部分
                ret = handler_(section, prev_name, no_comment_line,
                               Flag_Is_Multi_Line);
                if (ret != 0) {
                    error_map_[lineno] =
                        std::make_pair(line, "multiline custom handler ret" +
                                                 std::to_string(ret));
                    prev_name.clear();
                    continue;
                }
            } else if (no_comment_line[0] == '[') {
                // section行
                if (no_comment_line.size() < 3) {
                    error_map_[lineno] =
                        std::make_pair(line, "section line length less than 3");
                    prev_name.clear();
                    continue;
                }
                if (no_comment_line[no_comment_line.size() - 1] == ']') {
                    section =
                        no_comment_line.substr(1, no_comment_line.size() - 2);
                    if (options_.trim_section_space_) {
                        section = trim(section);
                    }
                    prev_name.clear();
                    if (options_.call_on_section_line_) {
                        ret = handler_(section, "", "", Flag_Is_Section_Line);
                        if (ret != 0) {
                            error_map_[lineno] = std::make_pair(
                                line, "section line custom handler ret" +
                                          std::to_string(ret));
                            prev_name.clear();
                            continue;
                        }
                    }
                } else {
                    error_map_[lineno] =
                        std::make_pair(line, "section line end without ]");
                    prev_name.clear();
                    continue;
                }
            } else {
                // key value行
                auto pos =
                    no_comment_line.find_first_of(options_.value_prefixes_);
                if (pos == std::string::npos) {
                    error_map_[lineno] = std::make_pair(
                        line, "key value line without prefixes " +
                                  options_.value_prefixes_);
                    prev_name.clear();
                    continue;
                }
                if (pos == 0) {
                    error_map_[lineno] =
                        std::make_pair(line, "key value line with empty key");
                    prev_name.clear();
                    continue;
                }
                name = trim(no_comment_line.substr(0, pos));
                value = trim(no_comment_line.substr(pos + 1));
                ret = handler_(section, name, value, 0);
                if (ret != 0) {
                    error_map_[lineno] =
                        std::make_pair(line, "normal line custom handler ret" +
                                                 std::to_string(ret));
                    prev_name.clear();
                    continue;
                }
                // 在handler成功时再写入prev_name
                // 保证multiline value第一行失败时，后续行都失败跳过
                prev_name = name;
            }
        }
    }
    if (not error_map_.empty()) {
        return IniParseError;
    }
    if (input.bad()) {
        return IniIStreamError;
    }
    return false;
}

int IniMap::Callback(IniMap *self, const std::string &section,
                     const std::string &name, const std::string &value,
                     uint64_t flag) {
    if (flag & IniParser::Flag_Is_Section_Line) {
        self->map_[section];
        return 0;
    }

    if (self->map_[section].count(name) > 0 &&
        flag & IniParser::Flag_Is_Multi_Line) {
        self->map_[section][name] += "\n" + value;
    } else {
        // 如果同一个key出现多次，则以最后一个key的value为准
        self->map_[section][name] = value;
    }
    return 0;
}

std::vector<std::string> IniMap::GetSections() const {
    std::vector<std::string> sections;
    for (const auto &p : map_) {
        sections.push_back(p.first);
    }
    return sections;
}

std::map<std::string, std::string>
IniMap::GetKeyValues(const std::string &section) const {
    std::map<std::string, std::string> kvs;
    auto it = map_.find(section);
    if (it == map_.end()) {
        return kvs;
    }
    kvs = it->second;
    return kvs;
}

std::string IniMap::Get(const std::string &section, const std::string &name,
                        const std::string &default_value) const {
    std::string res;
    int ret = Get(section, name, res);
    if (ret != 0) {
        return default_value;
    }
    return res;
}

int IniMap::Get(const std::string &section, const std::string &name,
                std::string &ret_value) const {
    auto it1 = map_.find(section);
    if (it1 == map_.end()) {
        return -1;
    }
    const auto &name_map = it1->second;
    auto it2 = name_map.find(name);
    if (it2 == name_map.end()) {
        return -2;
    }
    ret_value = it2->second;
    return 0;
}
}  // namespace ini
}  // namespace draft