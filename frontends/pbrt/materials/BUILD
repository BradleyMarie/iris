load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bumpmap",
    srcs = ["bumpmap.cc"],
    hdrs = ["bumpmap.h"],
    deps = [
        "//frontends/pbrt:texture_manager",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris/normal_maps:bump_normal_map",
        "//iris/textures:texture2d",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "bumpmap_test",
    srcs = ["bumpmap_test.cc"],
    deps = [
        ":bumpmap",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris/textures:texture2d",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "fourier",
    srcs = ["fourier.cc"],
    hdrs = ["fourier.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:fourier_material",
        "@libfbsdf//libfbsdf/readers:validating_bsdf_reader",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "fourier_test",
    srcs = ["fourier_test.cc"],
    deps = [
        ":fourier",
        ":result",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "glass",
    srcs = ["glass.cc"],
    hdrs = ["glass.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:glass_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "glass_test",
    srcs = ["glass_test.cc"],
    deps = [
        ":glass",
        ":result",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "matte",
    srcs = ["matte.cc"],
    hdrs = ["matte.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:matte_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "matte_test",
    srcs = ["matte_test.cc"],
    deps = [
        ":matte",
        ":result",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "metal",
    srcs = ["metal.cc"],
    hdrs = ["metal.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:metal_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "metal_test",
    srcs = ["metal_test.cc"],
    deps = [
        ":metal",
        ":result",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "mirror",
    srcs = ["mirror.cc"],
    hdrs = ["mirror.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:mirror_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "mirror_test",
    srcs = ["mirror_test.cc"],
    deps = [
        ":mirror",
        ":result",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "mix",
    srcs = ["mix.cc"],
    hdrs = ["mix.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:mix_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "mix_test",
    srcs = ["mix_test.cc"],
    deps = [
        ":mix",
        ":result",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris/materials:mock_material",
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
        ":fourier",
        ":glass",
        ":matte",
        ":metal",
        ":mirror",
        ":mix",
        ":plastic",
        ":result",
        ":substrate",
        ":translucent",
        ":uber",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        ":result",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris/materials:mock_material",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "plastic",
    srcs = ["plastic.cc"],
    hdrs = ["plastic.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:plastic_material",
        "@abseil-cpp//absl/flags:flag",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "plastic_test",
    srcs = ["plastic_test.cc"],
    deps = [
        ":plastic",
        ":result",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "result",
    hdrs = ["result.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
    ],
)

cc_library(
    name = "substrate",
    srcs = ["substrate.cc"],
    hdrs = ["substrate.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:substrate_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "substrate_test",
    srcs = ["substrate_test.cc"],
    deps = [
        ":result",
        ":substrate",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "translucent",
    srcs = ["translucent.cc"],
    hdrs = ["translucent.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:translucent_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "translucent_test",
    srcs = ["translucent_test.cc"],
    deps = [
        ":result",
        ":translucent",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "uber",
    srcs = ["uber.cc"],
    hdrs = ["uber.h"],
    deps = [
        ":bumpmap",
        ":result",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:material",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/materials:uber_material",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "uber_test",
    srcs = ["uber_test.cc"],
    deps = [
        ":result",
        ":uber",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
