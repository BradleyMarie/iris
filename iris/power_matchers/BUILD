load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "mock_power_matcher",
    testonly = True,
    hdrs = ["mock_power_matcher.h"],
    deps = [
        "//iris:float",
        "//iris:power_matcher",
        "//iris:spectrum",
        "@googletest//:gtest",
    ],
)
