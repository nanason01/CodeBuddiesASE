cc_library(
    name = "common",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    visibility = [
        "//data:__pkg__",
        "//engine:__pkg__",
        "//entry:__pkg__",
        "//exchanges:__pkg__",
        "//pricer:__pkg__",
    ],
)

# gtest build rule, copied from https://bazel.build/tutorials/cpp-use-cases
cc_library(
    name = "gtest",
    srcs = glob(
        ["googletest-release-1.10.0/src/*.cc"],
        exclude = ["googletest-release-1.10.0/src/gtest-all.cc"],
    ),
    hdrs = glob([
        "googletest-release-1.10.0/include/**/*.h",
        "googletest-release-1.10.0/src/*.h",
    ]),
    copts = [
        "-Iexternal/gtest/googletest-release-1.10.0/include",
        "-Iexternal/gtest/googletest-release-1.10.0",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
