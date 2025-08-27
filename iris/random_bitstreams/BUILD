load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//visibility:public"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "mersenne_twister_random_bitstream",
    srcs = ["mersenne_twister_random_bitstream.cc"],
    hdrs = ["mersenne_twister_random_bitstream.h"],
    deps = [
        "//iris:random_bitstream",
    ],
)

cc_test(
    name = "mersenne_twister_random_bitstream_test",
    srcs = ["mersenne_twister_random_bitstream_test.cc"],
    deps = [
        ":mersenne_twister_random_bitstream",
        "//iris:random_bitstream",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_random_bitstream",
    testonly = True,
    hdrs = ["mock_random_bitstream.h"],
    deps = [
        "//iris:random_bitstream",
        "@googletest//:gtest",
    ],
)
