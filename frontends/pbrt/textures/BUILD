load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "constant",
    srcs = ["constant.cc"],
    hdrs = ["constant.h"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
    ],
)

cc_test(
    name = "constant_test",
    srcs = ["constant_test.cc"],
    deps = [
        ":constant",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "image",
    srcs = ["image.cc"],
    hdrs = ["image.h"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "//iris/textures:image_texture",
        "//third_party/stb:stb_image",
    ],
)

cc_test(
    name = "image_test",
    srcs = ["image_test.cc"],
    data = [
        "test_data/image",
        "test_data/image.png",
        "test_data/image.txt",
    ],
    deps = [
        ":image",
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
        ":constant",
        ":image",
        ":scale",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":constant",
        ":image",
        ":parse",
        ":scale",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "scale",
    srcs = ["scale.cc"],
    hdrs = ["scale.h"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "//iris/textures:scaled_texture",
    ],
)

cc_test(
    name = "scale_test",
    srcs = ["scale_test.cc"],
    deps = [
        ":scale",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)
