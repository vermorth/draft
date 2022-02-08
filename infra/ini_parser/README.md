# ini_parser
 
A simple ini parser.


## Usage

You can use the default callback implementation IniMap, it will parse ini config to `std::map`. Or you can implement your own callback.
```c++
    std::string test_ini = R"ini([section1]
single1 = abc
single2 = xyz
[section1]
single3 = def
single4 = qrs)ini";
    std::stringstream input(test_ini);

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

    int ret = ini_parser.Parse(input);
    if (ret != 0) {
        // error handle
    }

    // use config by ini_map here
```

## Features

* whitespace before and after section / name / value will be trimmed
* ini config from various sources (file / string / stream)
* keys without sections (section is "")
* whole line comments and inline comments
    * inline comments must start with space or tab
* customize which character starts an comment
* customize which character splits key and value
* basic multi-line value
    * multi-line value must start with space or tab
    * comments have higher precedence than multi-line value
