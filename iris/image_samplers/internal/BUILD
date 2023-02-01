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
    name = "low_discrepancy_sequence",
    srcs = ["low_discrepancy_sequence.cc"],
    hdrs = ["low_discrepancy_sequence.h"],
    deps = [
        "//iris:float",
        "//iris:random",
    ],
)
