load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "trowbridge_reitz_distribution",
    srcs = ["trowbridge_reitz_distribution.cc"],
    hdrs = ["trowbridge_reitz_distribution.h"],
    deps = [
        "//iris:float",
        "//iris:vector",
        "//iris/bxdfs:microfacet_distribution",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "trowbridge_reitz_distribution_test",
    srcs = ["trowbridge_reitz_distribution_test.cc"],
    deps = [
        ":trowbridge_reitz_distribution",
        "@googletest//:gtest_main",
    ],
)
