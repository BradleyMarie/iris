load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bumpmap",
    srcs = ["bumpmap.cc"],
    hdrs = ["bumpmap.h"],
    deps = [
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/normal_maps:bump_normal_map",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "bumpmap_test",
    srcs = ["bumpmap_test.cc"],
    deps = [
        ":bumpmap",
        "//iris/textures:constant_texture",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "matte",
    srcs = ["matte.cc"],
    hdrs = ["matte.h"],
    deps = [
        ":bumpmap",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:matte_material",
    ],
)

cc_test(
    name = "matte_test",
    srcs = ["matte_test.cc"],
    deps = [
        ":matte",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mirror",
    srcs = ["mirror.cc"],
    hdrs = ["mirror.h"],
    deps = [
        ":bumpmap",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:mirror_material",
    ],
)

cc_test(
    name = "mirror_test",
    srcs = ["mirror_test.cc"],
    deps = [
        ":mirror",
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
        ":matte",
        ":mirror",
        ":plastic",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "plastic",
    srcs = ["plastic.cc"],
    hdrs = ["plastic.h"],
    deps = [
        ":bumpmap",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:plastic_material",
    ],
)

cc_test(
    name = "plastic_test",
    srcs = ["plastic_test.cc"],
    deps = [
        ":plastic",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)
