cc_library(
    name="auth",
    srcs=["auth.cc"],
    hdrs=["auth.h"],
    visibility=[
        "//entry:__pkg__",
    ],
)


cc_binary(
    name="entry",
    srcs=[
        "main.cc"
    ],
    linkopts=["-pthread", "-lcrypto"],
    deps=[
        "//common",
        "//data",
        "//engine",
        ":auth",
    ],
)

# no mock, nothing depends on this

cc_test(
    name="test",
    srcs=["endpoints_test.cc"],
    deps=[
        "//common",
        "//data:mock",
        "//engine:mock",
        "@com_google_googletest//:gtest_main",
    ],
)
