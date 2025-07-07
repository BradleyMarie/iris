load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "halton_image_sampler",
    srcs = ["halton_image_sampler.cc"],
    hdrs = ["halton_image_sampler.h"],
    deps = [
        "//iris:image_sampler",
        "//iris/image_samplers/internal:halton_sequence",
        "//iris/image_samplers/internal:low_discrepancy_image_sampler",
    ],
)

cc_test(
    name = "halton_image_sampler_test",
    srcs = ["halton_image_sampler_test.cc"],
    deps = [
        ":halton_image_sampler",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_image_sampler",
    testonly = True,
    hdrs = ["mock_image_sampler.h"],
    deps = [
        "//iris:image_sampler",
        "//iris:random_bitstream",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "random_image_sampler",
    srcs = ["random_image_sampler.cc"],
    hdrs = ["random_image_sampler.h"],
    deps = [
        "//iris:float",
        "//iris:image_sampler",
        "//iris:random_bitstream",
    ],
)

cc_test(
    name = "random_image_sampler_test",
    srcs = ["random_image_sampler_test.cc"],
    deps = [
        ":random_image_sampler",
        "//iris:image_sampler",
        "//iris/random_bitstreams:mock_random_bitstream",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "sobol_image_sampler",
    srcs = ["sobol_image_sampler.cc"],
    hdrs = ["sobol_image_sampler.h"],
    deps = [
        "//iris:image_sampler",
        "//iris/image_samplers/internal:low_discrepancy_image_sampler",
        "//iris/image_samplers/internal:sobol_sequence",
    ],
)

cc_test(
    name = "sobol_image_sampler_test",
    srcs = ["sobol_image_sampler_test.cc"],
    deps = [
        ":sobol_image_sampler",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "stratified_image_sampler",
    srcs = ["stratified_image_sampler.cc"],
    hdrs = ["stratified_image_sampler.h"],
    deps = [
        "//iris:float",
        "//iris:image_sampler",
        "//iris:random_bitstream",
    ],
)

cc_test(
    name = "stratified_image_sampler_test",
    srcs = ["stratified_image_sampler_test.cc"],
    deps = [
        ":stratified_image_sampler",
        "//iris:image_sampler",
        "//iris/random_bitstreams:mock_random_bitstream",
        "@googletest//:gtest_main",
    ],
)
