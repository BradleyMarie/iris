load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_albedo_matcher",
    testonly = True,
    hdrs = ["mock_albedo_matcher.h"],
    deps = [
        "//iris:albedo_matcher",
        "//iris:float",
        "//iris:reflector",
        "@com_google_googletest//:gtest",
    ],
)
