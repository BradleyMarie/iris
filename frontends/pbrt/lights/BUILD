load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "infinite",
    srcs = ["infinite.cc"],
    hdrs = ["infinite.h"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:spectrum_manager",
        "//iris:environmental_light",
        "//iris:light",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/environmental_lights:image_environmental_light",
        "//third_party/stb:stb_image",
        "//third_party/tinyexr",
    ],
)

cc_test(
    name = "infinite_test",
    srcs = ["infinite_test.cc"],
    data = [
        "test_data/image",
        "test_data/image.exr",
        "test_data/image.png",
        "test_data/image.txt",
    ],
    deps = [
        ":infinite",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@bazel_tools//tools/cpp/runfiles",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":infinite",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt:tokenizer",
        "//iris:environmental_light",
        "//iris:light",
        "//iris:matrix",
        "//iris:reference_counted",
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
