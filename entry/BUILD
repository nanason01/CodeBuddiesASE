
# not amazing separation, but makes main 1000x cleaner
# using a static class with global dependencies
cc_library(
    name="entry",
    srcs=["endpoints.cc"],
    hdrs=["endpoints.h"],
    deps=[
        "//common",
        "//data",
        "//data:mock",
        "//engine",
        "//engine:mock",
        "@com_google_googletest//:gtest_main",
    ],
    visibility=[
        "//entry:__pkg__",
    ],
)


cc_binary(
    name="service",
    srcs=["main.cc"],
    linkopts=["-pthread", "-lcrypto", "-lcurl",
              "-lboost_date_time", "-ldl"],
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
        "@com_google_googletest//:gtest_main",
    ],
)
