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
    name = "glass",
    srcs = ["glass.cc"],
    hdrs = ["glass.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//iris/materials:glass_material",
    ],
)

cc_test(
    name = "glass_test",
    srcs = ["glass_test.cc"],
    deps = [
        ":glass",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "material_builder",
    hdrs = ["material_builder.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
    ],
)

cc_library(
    name = "matte",
    srcs = ["matte.cc"],
    hdrs = ["matte.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//iris/materials:matte_material",
    ],
)

cc_test(
    name = "matte_test",
    srcs = ["matte_test.cc"],
    deps = [
        ":matte",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mix",
    srcs = ["mix.cc"],
    hdrs = ["mix.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//frontends/pbrt:material_manager",
        "//iris/materials:mix_material",
    ],
)

cc_test(
    name = "mix_test",
    srcs = ["mix_test.cc"],
    deps = [
        ":mix",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris/materials:mock_material",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mirror",
    srcs = ["mirror.cc"],
    hdrs = ["mirror.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//iris/materials:mirror_material",
    ],
)

cc_test(
    name = "mirror_test",
    srcs = ["mirror_test.cc"],
    deps = [
        ":mirror",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
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
        ":glass",
        ":material_builder",
        ":matte",
        ":mirror",
        ":mix",
        ":plastic",
        ":substrate",
        ":translucent",
        ":uber",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:quoted_string",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt:material_manager",
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
        ":material_builder",
        "//iris/materials:plastic_material",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_test(
    name = "plastic_test",
    srcs = ["plastic_test.cc"],
    deps = [
        ":plastic",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "substrate",
    srcs = ["substrate.cc"],
    hdrs = ["substrate.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//iris/materials:substrate_material",
    ],
)

cc_test(
    name = "substrate_test",
    srcs = ["substrate_test.cc"],
    deps = [
        ":substrate",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "translucent",
    srcs = ["translucent.cc"],
    hdrs = ["translucent.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//iris/materials:translucent_material",
    ],
)

cc_test(
    name = "translucent_test",
    srcs = ["translucent_test.cc"],
    deps = [
        ":translucent",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "uber",
    srcs = ["uber.cc"],
    hdrs = ["uber.h"],
    deps = [
        ":bumpmap",
        ":material_builder",
        "//iris/materials:uber_material",
    ],
)

cc_test(
    name = "uber_test",
    srcs = ["uber_test.cc"],
    deps = [
        ":material_builder",
        ":uber",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)
