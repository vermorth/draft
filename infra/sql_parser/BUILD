# --------------------------------------
# library sql_parser
# --------------------------------------

filegroup(
    name = "sql_parser_parser_srcs",
    srcs = glob(
        [
            "src/parser/*.cpp",
        ],
        exclude = [
            "src/parser/conflict_test.cpp",
        ],
    ),
)

filegroup(
    name = "sql_parser_parser_hdrs",
    srcs = glob([
        "src/parser/*.h",
    ]),
)

filegroup(
    name = "sql_parser_main_srcs",
    srcs = glob([
        "src/*.cpp",
        "src/sql/*.cpp",
        "src/util/*.cpp",
    ]),
)

filegroup(
    name = "sql_parser_main_hdrs",
    srcs = glob([
        "src/*.h",
        "src/sql/*.h",
        "src/util/*.h",
    ]),
)

sql_parser_copts = [
    "-std=c++1z",
    "-Wall",
    #"-Werror",
]

sql_parser_tests_copts = [
    "-std=c++1z",
    "-Wall",
]

cc_library(
    name = "sql_parser",
    srcs = [
        ":sql_parser_main_srcs",
        ":sql_parser_parser_srcs",
    ],
    hdrs = [
        ":sql_parser_main_hdrs",
        ":sql_parser_parser_hdrs",
    ],
    copts = sql_parser_copts,
    includes = [
        "src",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
    deps = [
    ],
)

# --------------------------------------
# binary example
# --------------------------------------

cc_binary(
    name = "example",
    srcs = [
        "example/example.cpp",
    ],
    copts = sql_parser_copts,
    defines = [],
    includes = [],
    linkopts = [],
    deps = [
        ":sql_parser",
    ],
)

# --------------------------------------
# binary test
# --------------------------------------

cc_binary(
    name = "tests",
    srcs = [
        "test/auto_query_file_test.cpp",
        "test/prepare_tests.cpp",
        "test/select_tests.cpp",
        "test/sql_asserts.h",
        "test/sql_parser.cpp",
        "test/sql_tests.cpp",
        "test/thirdparty/microtest/microtest.h",
        "test/tpc_h_tests.cpp",
    ],
    copts = sql_parser_tests_copts,
    defines = [],
    includes = [],
    linkopts = [],
    deps = [
        ":sql_parser",
    ],
)
