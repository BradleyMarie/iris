load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_random",
    testonly = True,
    hdrs = ["mock_random.h"],
    deps = [
        "//iris:float",
        "//iris:random",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "mersenne_twister_random",
    srcs = ["mersenne_twister_random.cc"],
    hdrs = ["mersenne_twister_random.h"],
    deps = [
        "//iris:float",
        "//iris:random",
    ],
)

cc_test(
    name = "mersenne_twister_random_test",
    srcs = ["mersenne_twister_random_test.cc"],
    deps = [
        ":mersenne_twister_random",
        "//iris:float",
        "//iris:random",
        "@googletest//:gtest_main",
    ],
)
