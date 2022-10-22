cc_library(
    name = "engine",
    srcs = ["matching.cc"],
    hdrs = ["matching.h"],
    visibility = [
        "//entry:__pkg__",
    ],
    deps = [
        "//common",
        "//pricer",
    ],
)

cc_library(
    name = "mock-matching",
    hdrs = ["mock-matching.h"],
    visibility = ["//entry:__pkg__"],
    deps = ["//engine"],
)

cc_test(
    name = "matching-test",
    srcs = ["matching_test.cc"],
    deps = [
        "//common",
        "//engine",
        "//pricer:mock",
        "@com_google_googletest//:gtest_main",
    ],
)
