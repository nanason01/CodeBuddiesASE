cc_library(
    name = "exchanges",
    srcs = [
        # whatever source files u use go here @Aleks
    ],
    hdrs = [
        "coinbase.h",
        "crypto_com.h",
        "driver.h",
    ],
    visibility = [
        "//data:__pkg__",
        "//entry:__pkg__",
    ],
    deps = ["//common"],
)

cc_library(
    name = "mock",
    srcs = [],
    hdrs = [
        "driver.h",
        "mock_driver.h",
    ],
    visibility = [
        "//data:__pkg__",
        "//entry:__pkg__",
    ],
    deps = ["//common"],
)

cc_test(
    name = "test",
    srcs = ["exchanges_test.cc"],
    deps = [
        "//common",
        "//exchanges",
        "@com_google_googletest//:gtest_main",
    ],
)
