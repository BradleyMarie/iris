load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "halton",
    srcs = ["halton.cc"],
    hdrs = ["halton.h"],
    deps = [
        "//iris:image_sampler",
        "//iris/image_samplers:halton_image_sampler",
        "//third_party/gruenschloss/halton",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "halton_test",
    srcs = ["halton_test.cc"],
    deps = [
        ":halton",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":halton",
        ":random",
        ":sobol",
        ":stratified",
        "//iris:image_sampler",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "random",
    srcs = ["random.cc"],
    hdrs = ["random.h"],
    deps = [
        "//iris:image_sampler",
        "//iris/image_samplers:random_image_sampler",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "random_test",
    srcs = ["random_test.cc"],
    deps = [
        ":random",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "sobol",
    srcs = ["sobol.cc"],
    hdrs = ["sobol.h"],
    deps = [
        "//iris:image_sampler",
        "//iris/image_samplers:sobol_image_sampler",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "sobol_test",
    srcs = ["sobol_test.cc"],
    deps = [
        ":sobol",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "stratified",
    srcs = ["stratified.cc"],
    hdrs = ["stratified.h"],
    deps = [
        "//iris:image_sampler",
        "//iris/image_samplers:stratified_image_sampler",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "stratified_test",
    srcs = ["stratified_test.cc"],
    deps = [
        ":stratified",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
