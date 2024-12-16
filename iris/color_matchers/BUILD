load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cie_color_matcher",
    srcs = ["cie_color_matcher.cc"],
    hdrs = ["cie_color_matcher.h"],
    deps = [
        "//iris:color",
        "//iris:color_matcher",
        "//iris:float",
        "//iris:reflector",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "cie_color_matcher_test",
    srcs = ["cie_color_matcher_test.cc"],
    deps = [
        ":cie_color_matcher",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_color_matcher",
    testonly = True,
    hdrs = ["mock_color_matcher.h"],
    deps = [
        "//iris:color_matcher",
        "//iris:float",
        "//iris:spectrum",
        "@googletest//:gtest",
    ],
)
