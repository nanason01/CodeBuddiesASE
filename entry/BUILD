cc_library(
    name="entry",
    srcs=["auth.cc"],  # will eventually have endpoints.cc
    hdrs=["auth.h", "endpoints.h"],
    linkopts=["-pthread", "-lcrypto"],
    deps=[
        "//common",
        "//data",
        "//engine",
    ],
    visibility=[
        "//entry:__pkg__",
    ],
)


cc_binary(
    name="service",
    srcs=[
        "main.cc"
    ],
    linkopts=["-pthread", "-lcrypto"],
    deps=[
        "//entry"
    ],
)

# no mock, nothing depends on this

cc_test(
    name="test",
    srcs=["endpoints_test.cc"],
    linkopts=["-pthread", "-lcrypto", "-lcurl",
              "-lboost_date_time", "-ldl"],
    deps=[
        "//entry",
        "//common",
        "//data:mock",
        "//engine:mock",
        "@com_google_googletest//:gtest_main",
    ],
)
