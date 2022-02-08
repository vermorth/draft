#pragma once

#include <algorithm>
#include <fstream>
#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace draft {

namespace ini {

enum {
    IniParseError = -1001,
    IniIStreamError = -1002,
    IniOptionsError_InvalidInlineCommentsPrefixes = -1003,
    IniOptionsError_InvalidLineCommentsPrefixes = -1004,
    IniOptionsError_InvalidValuePrefixes = -1005,
};

class IniParser {
 public:
    class Options {
     public:
        Options()
            : allow_multiline_(false), allow_inline_comments_(true),
              inline_comments_prefixes_(";#"), allow_line_comments_(true),
              line_comments_prefixes_(";#"), value_prefixes_("="),
              stop_on_first_error_(false), trim_section_space_(true),
              call_on_section_line_(true) {}

        // 是否允许多行value
        // 跨行value 开头必须有space或tab组成
        // 且注释优先级大于multi line，以注释符开头的行，即使前方有空格
        // 也会被认为是注释
        // [Long Section]
        // short: this is a normal line
        // long: this value continues
        //       in the next line  ; this is inline comment
        //       ; this is line comment
        bool allow_multiline_ = false;
        // 是否允许行末的注释
        // 行末注释前 必须有一个space或tab
        // Key=Value ; comments
        bool allow_inline_comments_ = true;
        // 指定行末注释的前缀
        // 行末注释的前缀字符，必须是整行注释的子集
        // 只包含行末注释的行，也会被当作整行注释处理
        std::string inline_comments_prefixes_ = ";#";
        // 是否允许整行的注释
        // ; comments
        bool allow_line_comments_ = true;
        // 指定整行注释的前缀
        std::string line_comments_prefixes_ = ";#";
        // 指定key和value的分隔符
        // name=value
        // name:value
        std::string value_prefixes_ = "=";
        // 是否遇到一个错误就退出
        bool stop_on_first_error_ = false;
        // 是否trim掉section前后的空白符
        bool trim_section_space_ = true;
        // 是否在遇到section line时触发回调函数
        bool call_on_section_line_ = true;
    };

    // Flag
    enum {
        // 如果该次回调为section line触发，flag会设置该标记
        Flag_Is_Section_Line = 1,
        // 如果该次回调为multi line的非首行，flag会设置该标记
        Flag_Is_Multi_Line = 2,
    };

    // 解析到一个key value后，调用的回调函数
    using CustomHanlder =
        std::function<int(const std::string &, const std::string &,
                          const std::string &, uint64_t)>;

    using ErrorMap = std::map<uint32_t, std::pair<std::string, std::string>>;

 public:
    IniParser(CustomHanlder handler) : handler_(handler), options_() {}
    IniParser(CustomHanlder handler, const Options &options)
        : handler_(handler), options_(options) {}

    void SetOptions(const Options &options) { options_ = options; }
    const Options &Options() const { return options_; }

    const ErrorMap &Error() const { return error_map_; }

    // return 0 means success
    // return
    int Parse(std::istream &input);

 private:
    int CheckOptions();
    bool IsLineComment(const std::string &line);
    int TrimInlineComment(const std::string &line, std::string &trim_line);

 private:
    CustomHanlder handler_;
    class Options options_;
    // lineno -> (line_content, error_message)
    ErrorMap error_map_;
};

class IniMap {
 public:
    static int Callback(IniMap *self, const std::string &section,
                        const std::string &name, const std::string &value,
                        uint64_t flag);

    std::vector<std::string> GetSections() const;

    std::map<std::string, std::string>
    GetKeyValues(const std::string &section) const;

    const std::map<std::string, std::map<std::string, std::string>> &
    GetMap() const {
        return map_;
    }

    std::string Get(const std::string &section, const std::string &name,
                    const std::string &default_value) const;

    // ret 0 if section and name exists
    // return -1 means section not found
    // return -2 means name not found
    int Get(const std::string &section, const std::string &name,
            std::string &ret_value) const;

 private:
    std::map<std::string, std::map<std::string, std::string>> map_;
};

}  // namespace ini
}  // namespace draft