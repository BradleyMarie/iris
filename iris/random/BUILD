load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

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
