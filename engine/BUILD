cc_library(
    name = "engine",
    srcs = ["matcher.cc"],
    hdrs = [
        "base_matcher.h",
        "matcher.h",
    ],
    visibility = [
        "//entry:__pkg__",
    ],
    deps = [
        "//common",
        "//pricer",
    ],
)

cc_library(
    name = "mock-matcher",
    hdrs = ["mock_matcher.h"],
    visibility = ["//entry:__pkg__"],
    deps = ["//engine"],
)

cc_test(
    name = "matcher-test",
    srcs = ["matcher_test.cc"],
    deps = [
        "//common",
        "//engine",
        "//pricer:mock",
        "@com_google_googletest//:gtest_main",
    ],
)
