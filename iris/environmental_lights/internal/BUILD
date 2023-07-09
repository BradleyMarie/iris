load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//iris/environmental_lights:__subpackages__"])

cc_library(
    name = "distribution_1d",
    srcs = ["distribution_1d.cc"],
    hdrs = ["distribution_1d.h"],
    deps = [
        "//iris:float",
        "//iris:sampler",
    ],
)

cc_test(
    name = "distribution_1d_test",
    srcs = ["distribution_1d_test.cc"],
    deps = [
        ":distribution_1d",
        "//iris/random:mock_random",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "distribution_2d",
    srcs = ["distribution_2d.cc"],
    hdrs = ["distribution_2d.h"],
    deps = [
        ":distribution_1d",
        "//iris:float",
        "//iris:sampler",
    ],
)

cc_test(
    name = "distribution_2d_test",
    srcs = ["distribution_2d_test.cc"],
    deps = [
        ":distribution_2d",
        "//iris/random:mock_random",
        "@com_google_googletest//:gtest_main",
    ],
)
