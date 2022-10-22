cc_library(
    name = "engine",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    visibility = [
        "//entry:__pkg__",
    ],
    deps = [
        "//common",
        "//pricer",
    ],
)
cc_test(
    name = "hello_test",
    srcs = ["matching_test.cc"],
    deps = [
        "//engine:engine",
        "//common",
        "//pricer",
        "@com_google_googletest//:gtest_main",
    ],
)
