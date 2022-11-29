cc_test(
    name="data-integration",
    srcs=["data_integration_test.cc"],
    linkopts=["-pthread", "-lcrypto", "-lcurl",
              "-lboost_date_time","-ldl"],
    deps=[
        "//common",
        "//data",
        "//exchanges",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name="engine-integration",
    srcs=["engine_integration_test.cc"],
    linkopts=["-lcurl"],
    deps=[
        "//common",
        "//engine",
        "//pricer",
        "@com_google_googletest//:gtest_main",
    ],
)
