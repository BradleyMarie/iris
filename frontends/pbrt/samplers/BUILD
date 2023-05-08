load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "halton",
    srcs = ["halton.cc"],
    hdrs = ["halton.h"],
    deps = [
        ":result",
        "//frontends/pbrt:object_builder",
        "//iris/image_samplers:halton_image_sampler",
    ],
)

cc_test(
    name = "halton_test",
    srcs = ["halton_test.cc"],
    deps = [
        ":halton",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
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
        ":result",
        ":sobol",
        ":stratified",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:tokenizer",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "result",
    hdrs = ["result.h"],
    deps = [
        "//iris:image_sampler",
    ],
)

cc_library(
    name = "random",
    srcs = ["random.cc"],
    hdrs = ["random.h"],
    deps = [
        ":result",
        "//frontends/pbrt:object_builder",
        "//iris/image_samplers:random_image_sampler",
    ],
)

cc_test(
    name = "random_test",
    srcs = ["random_test.cc"],
    deps = [
        ":random",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "sobol",
    srcs = ["sobol.cc"],
    hdrs = ["sobol.h"],
    deps = [
        ":result",
        "//frontends/pbrt:object_builder",
        "//iris/image_samplers:sobol_image_sampler",
    ],
)

cc_test(
    name = "sobol_test",
    srcs = ["sobol_test.cc"],
    deps = [
        ":sobol",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "stratified",
    srcs = ["stratified.cc"],
    hdrs = ["stratified.h"],
    deps = [
        ":result",
        "//frontends/pbrt:object_builder",
        "//iris/image_samplers:stratified_image_sampler",
    ],
)

cc_test(
    name = "stratified_test",
    srcs = ["stratified_test.cc"],
    deps = [
        ":stratified",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)
