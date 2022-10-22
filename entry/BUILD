cc_binary(
    name = "entry",
    srcs = [
        "main.cc",
        # Add your .cc files here probs endpoints.cc
    ],
    hdrs = ["endpoints.h"],
    deps = [
        "//common",
        "//data",
        "//engine",
    ],
)

# no mock, nothing depends on this

cc_test(
    name = "test",
    srcs = ["endpoints_test.cc"],
    hdrs = ["endpoints.h"],
    deps = [
        "//common",
        "//data:mock",
        "//engine:mock",
        "@com_google_googletest//:gtest_main",
    ],
)
