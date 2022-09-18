# sqlparser
This is a SQL Parser for C++. It parses the given SQL query into C++ objects.

## Usage
**Note:** You can also find a detailed usage description [here](docs/basic-usage.md).

**Requirements:**
* gcc (v4.8+, Prefer v7.5+)
* [bison](https://www.gnu.org/software/bison/) (v3.0.2+, Prefer v3.8.2)
* [flex](https://github.com/westes/flex) (v2.6+, Prefer v2.6.4)

1. add `//platform/sundry/sqlparser:sqlparser` to your `cc_binary`/`cc_library` deps
2. code like [example](example/example.cpp)

## Build
```
bazel build :sqlparser
```

## Example
```bash
# build example
bazel build :example

# run example
./example "SELECT * FROM students WHERE name = 'xxx';"
```

## Test

安装valgrind bison flex
```bash
# if flex version is less than 2.6, you may need install flex manually
# wget https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz
# tar -xf flex-2.6.4.tar.gz
# cd flex-2.6.4 && ./configure && make && make install
yum install -y valgrind bison flex
```

build tests
```bash
bazel build :tests
```

run tests
```bash
./test/test.sh ./tests
```

## flex/bison regen

```bash
cd src/parser

# regen bison_parser.cpp
make bison_parser.cpp

# regen flex_lexer.cpp
make flex_lexer.cpp

# regen all
make
```
