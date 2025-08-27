load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//visibility:public"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "cie_color_matcher",
    srcs = ["cie_color_matcher.cc"],
    hdrs = ["cie_color_matcher.h"],
    deps = [
        "//iris:color",
        "//iris:color_matcher",
        "//iris:float",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "cie_color_matcher_test",
    srcs = ["cie_color_matcher_test.cc"],
    deps = [
        ":cie_color_matcher",
        "//iris:color_matcher",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_color_matcher",
    testonly = True,
    hdrs = ["mock_color_matcher.h"],
    deps = [
        "//iris:color",
        "//iris:color_matcher",
        "//iris:float",
        "//iris:spectrum",
        "@googletest//:gtest",
    ],
)
