load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//iris/bxdfs:__subpackages__"])

cc_library(
    name = "math",
    srcs = ["math.cc"],
    hdrs = ["math.h"],
    deps = [
        "//iris:float",
        "//iris:sampler",
        "//iris:vector",
    ],
)

cc_test(
    name = "math_test",
    srcs = ["math_test.cc"],
    deps = [
        ":math",
        "//iris/random:mock_random",
        "@com_google_googletest//:gtest_main",
    ],
)
