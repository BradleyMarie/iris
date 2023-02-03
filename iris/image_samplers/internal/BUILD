load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//iris/image_samplers:__subpackages__"])

cc_library(
    name = "halton_sequence",
    srcs = ["halton_sequence.cc"],
    hdrs = ["halton_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//third_party/gruenschloss/halton",
    ],
)

cc_library(
    name = "low_discrepancy_image_sampler",
    srcs = ["low_discrepancy_image_sampler.cc"],
    hdrs = ["low_discrepancy_image_sampler.h"],
    deps = [
        ":low_discrepancy_sequence",
        "//iris:image_sampler",
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
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_low_discrepancy_sequence",
    testonly = True,
    hdrs = ["mock_low_discrepancy_sequence.h"],
    deps = [
        ":low_discrepancy_sequence",
        "@com_google_googletest//:gtest",
    ],
)
