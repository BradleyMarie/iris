load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//iris/image_samplers:__subpackages__"])

cc_library(
    name = "halton_sequence",
    srcs = ["halton_sequence.cc"],
    hdrs = ["halton_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//iris:float",
        "//third_party/gruenschloss/halton",
    ],
)

cc_test(
    name = "halton_sequence_test",
    srcs = ["halton_sequence_test.cc"],
    deps = [
        ":halton_sequence",
        "//iris:float",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "low_discrepancy_image_sampler",
    srcs = ["low_discrepancy_image_sampler.cc"],
    hdrs = ["low_discrepancy_image_sampler.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//iris:float",
        "//iris:image_sampler",
        "//iris:random",
    ],
)

cc_test(
    name = "low_discrepancy_image_sampler_test",
    srcs = ["low_discrepancy_image_sampler_test.cc"],
    deps = [
        ":low_discrepancy_image_sampler",
        ":mock_low_discrepancy_sequence",
        "//iris/random:mock_random",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "low_discrepancy_sequence",
    srcs = ["low_discrepancy_sequence.cc"],
    hdrs = ["low_discrepancy_sequence.h"],
    deps = [
        "//iris:float",
        "//iris:random",
    ],
)

cc_test(
    name = "low_discrepancy_sequence_test",
    srcs = ["low_discrepancy_sequence_test.cc"],
    deps = [
        ":low_discrepancy_sequence",
        ":mock_low_discrepancy_sequence",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_low_discrepancy_sequence",
    testonly = True,
    hdrs = ["mock_low_discrepancy_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//iris:float",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sobol_sequence",
    srcs = ["sobol_sequence.cc"],
    hdrs = ["sobol_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//iris:float",
        "//third_party/gruenschloss/double:sobol",
        "//third_party/gruenschloss/single:sobol",
        "//third_party/pbrt:sobolmatrices",
    ],
)

cc_test(
    name = "sobol_sequence_test",
    srcs = ["sobol_sequence_test.cc"],
    deps = [
        ":sobol_sequence",
        "@googletest//:gtest_main",
    ],
)
