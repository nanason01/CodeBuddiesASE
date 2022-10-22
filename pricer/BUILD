cc_library(
    name = "pricer",
    srcs = [],
    hdrs = ["pricer.h"],
    visibility = [
        "//engine:__pkg__",
        "//pricer:__pkg__",
    ],
    deps = ["//common"],
)

cc_library(
    name = "mock-pricer",
    hdrs = ["mock-pricer.h"],
    visibility = ["//engine:__pkg__"],
    deps = [
        "//pricer",
        "@com_google_googletest//:gtest_main",
    ],
)
