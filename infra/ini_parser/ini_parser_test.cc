#include "ini_parser.hh"

#include "gtest/gtest.h"
#include <iostream>

using namespace draft::ini;

void PrintIniMap(const IniMap &ini_map) {
    for (auto s : ini_map.GetSections()) {
        std::cout << '[' << s << "]\n";
        auto kvs = ini_map.GetKeyValues(s);
        for (auto p : kvs) {
            std::cout << p.first << "=" << p.second << '\n';
        }
    }
    return;
}

TEST(IniParser, Normal) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.inline_comments_prefixes_ = ";";
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);

    std::string test_ini = R"ini(; This is an INI file
no_sec_one = abc
no_sec_two :   
[section1]  ; section comment
one=This is a test  ; name=value comment
two = 1234
; x=y

[ section 2 ]
happy  =  4
sad =

[empty]
; do nothing

[comment_test]
test1 = 1;2;3 ; only this will be a comment
test2 = 2;3;4;this won't be a comment, needs whitespace before ';'
test;3 = 345 ; key should be "test;3"
test4 = 4#5#6 ; '#' only starts a comment at start of line
#test5 = 567 ; entire line commented
 # test6 = 678 ; entire line commented
test7 = ; blank value, except if inline comments disabled
test8 =; not a comment, needs whitespace before ';'

[colon_tests]
Content-Type: text/html
foo:bar
adams : 42
funny1 : with = equals
funny2 = with : colons
funny3 = two = equals
funny4 : two : colons)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    ASSERT_EQ(ret, 0);
    std::map<std::string, std::map<std::string, std::string>> res = {
        {"", {{"no_sec_one", "abc"}, {"no_sec_two", ""}}},
        {
            "section1",
            {
                {"one", "This is a test"},
                {"two", "1234"},
            },
        },
        {
            "section 2",
            {
                {"happy", "4"},
                {"sad", ""},
            },
        },
        {
            "empty",
            {},
        },
        {
            "comment_test",
            {
                {"test1", "1;2;3"},
                {"test2",
                 "2;3;4;this won't be a comment, needs whitespace before ';'"},
                {"test;3", "345"},
                {"test4", "4#5#6"},
                {"test7", ""},
                {"test8", "; not a comment, needs whitespace before ';'"},
            },
        },
        {
            "colon_tests",
            {
                {"Content-Type", "text/html"},
                {"foo", "bar"},
                {"adams", "42"},
                {"funny1", "with = equals"},
                {"funny2", "with : colons"},
                {"funny3", "two = equals"},
                {"funny4", "two : colons"},
            },
        },
    };
    ASSERT_EQ(res, ini_map.GetMap());
}

TEST(IniParser, MultiLine) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini([section1]
single1 = abc
multi = this is a
        multi-line value
single2 = xyz
[section2]
multi = a
        b
        c
[section3]
single: ghi
multi: the quick
       brown fox
name = bob smith  ; comment line 1
                  ; comment line 2
[Long Section]
short: this is a normal line
long: this value continues
      in the next line  ; this is comment
      ; this is comment
)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    ASSERT_EQ(ret, 0);

    std::map<std::string, std::map<std::string, std::string>> res = {
        {
            "section1",
            {
                {"single1", "abc"},
                {"multi", "this is a\nmulti-line value"},
                {"single2", "xyz"},
            },
        },
        {
            "section2",
            {
                {"multi", "a\nb\nc"},
            },
        },
        {
            "section3",
            {
                {"single", "ghi"},
                {"multi", "the quick\nbrown fox"},
                {"name", "bob smith"},
            },
        },
        {
            "Long Section",
            {
                {"short", "this is a normal line"},
                {"long", "this value continues\nin the next line"},
            },
        },
    };
    ASSERT_EQ(res, ini_map.GetMap());
}

TEST(IniParser, DumpSection) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini([section1]
single1 = abc
single2 = xyz
[section1]
single3 = def
single4 = qrs)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    ASSERT_EQ(ret, 0);

    std::map<std::string, std::map<std::string, std::string>> res = {
        {
            "section1",
            {
                {"single1", "abc"},
                {"single2", "xyz"},
                {"single3", "def"},
                {"single4", "qrs"},
            },
        },
    };
    ASSERT_EQ(res, ini_map.GetMap());
}

TEST(IniParser, BadSection) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini([section1]
name1=value1
[section2
[section3   ; comment ]
name2=value2)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    ASSERT_EQ(ret, -1001);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    IniParser::ErrorMap error_res = {
        {3, {"[section2", "section line end without ]"}},
        {4, {"[section3   ; comment ]", "section line end without ]"}},
    };
    ASSERT_EQ(error_res, ini_parser.Error());
}

TEST(IniParser, BadComment) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini(This is an error)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    ASSERT_EQ(ret, -1001);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    IniParser::ErrorMap error_res = {
        {1, {"This is an error", "key value line without prefixes =:"}},
    };
    ASSERT_EQ(error_res, ini_parser.Error());
}

TEST(IniParser, BadMultiLine) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini(  indented
a=another
  indented again
)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    ASSERT_EQ(ret, -1001);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    IniParser::ErrorMap error_res = {
        {1, {"  indented", "key value line without prefixes =:"}},
    };
    ASSERT_EQ(error_res, ini_parser.Error());
}

TEST(IniParser, DumpSectionAndDumpKey) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini([section1]
single1 = abc
single2 = xyz
[section1]
single1 = def
single2 = qrs
[section2]
key1=10
key1=100
)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }
    ASSERT_EQ(ret, 0);

    std::map<std::string, std::map<std::string, std::string>> res = {
        {
            "section1",
            {
                {"single1", "def"},
                {"single2", "qrs"},
            },
        },
        {
            "section2",
            {
                {"key1", "100"},
            },
        },
    };
    ASSERT_EQ(res, ini_map.GetMap());
}

TEST(IniParser, NoValue) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini([section_list]
section0
section1

[section0]
key0=val0
key1=

[section1]
key1=val1)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    ASSERT_EQ(ret, -1001);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }

    IniParser::ErrorMap error_res = {
        {2, {"section0", "key value line without prefixes =:"}},
        {3, {"section1", "key value line without prefixes =:"}},
    };
    ASSERT_EQ(error_res, ini_parser.Error());
}

TEST(IniParser, NoKey) {
    class IniParser::Options opts;
    opts.allow_multiline_ = true;
    opts.value_prefixes_ = "=:";
    IniMap ini_map;
    IniParser ini_parser(
        [&ini_map](const std::string &section, const std::string &name,
                   const std::string &value, uint64_t flag) -> int {
            return ini_map.Callback(&ini_map, section, name, value, flag);
        },
        opts);
    std::string test_ini = R"ini([section0]
=val0)ini";
    std::stringstream input(test_ini);
    int ret = ini_parser.Parse(input);
    ASSERT_EQ(ret, -1001);
    if (ret == -1001) {
        std::cout << "[Error Map]\n";
        for (auto p : ini_parser.Error()) {
            std::cout << "line " << p.first << ": ";
            std::cout << p.second.first << '\n';
            std::cout << "error: " << p.second.second << '\n';
        }
    }

    IniParser::ErrorMap error_res = {
        {2, {"=val0", "key value line with empty key"}},
    };
    ASSERT_EQ(error_res, ini_parser.Error());
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}