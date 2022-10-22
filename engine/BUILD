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
        "//pricer:mock-pricer",
    ],
)

cc_test(
    name = "matching-test",
    srcs = ["matching_test.cc"],
    deps = [
        "//common",
        "//engine",
        "//pricer:mock-pricer",
        "@com_google_googletest//:gtest_main",
    ],
)
