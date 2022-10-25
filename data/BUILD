cc_binary(
    name = "test-nit",
    srcs = ["sqlite_tester.cc"],
    deps = [
        "//data/sqlite_stuff:sqlite3",
    ],
)

cc_library(
    name = "data",
    srcs = [
        # add src file used here
    ],
    hdrs = [
        "base_data.h",
        "data.h",
    ],
    visibility = [
        "//entry:__pkg__",
    ],
    deps = [
        "//common",
        "//exchanges",
    ],
)

cc_library(
    name = "mock",
    hdrs = [
        "base_data.h",
        "mock_data.h",
    ],
    visibility = [
        "//entry:__pkg__",
    ],
    deps = [
        "//common",
        "//exchanges",
    ],
)

cc_test(
    name = "test",
    srcs = ["data_test.cc"],
    deps = [
        "//common",
        "//exchanges:mock",
        "@com_google_googletest//:gtest_main",
    ],
)
