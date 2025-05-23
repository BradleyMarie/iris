load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "alpha_mask",
    srcs = ["alpha_mask.cc"],
    hdrs = ["alpha_mask.h"],
    deps = [
        "//frontends/pbrt:texture_manager",
        "//iris:float",
        "//iris:reference_counted",
        "//iris/textures:texture2d",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "alpha_mask_test",
    srcs = ["alpha_mask_test.cc"],
    deps = [
        ":alpha_mask",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:reference_counted",
        "//iris/textures:texture2d",
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
        ":plymesh",
        ":sphere",
        ":trianglemesh",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/materials:parse",
        "//frontends/pbrt/materials:result",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt:material_manager",
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/materials:result",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "plymesh",
    srcs = ["plymesh.cc"],
    hdrs = ["plymesh.h"],
    deps = [
        ":alpha_mask",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/geometry:triangle_mesh",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
        "@plyodine//plyodine/readers:triangle_mesh_reader",
    ],
)

cc_test(
    name = "plymesh_test",
    srcs = ["plymesh_test.cc"],
    data = [
        "test_data/empty.ply",
        "test_data/triangles.ply",
    ],
    deps = [
        ":plymesh",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "sphere",
    srcs = ["sphere.cc"],
    hdrs = ["sphere.h"],
    deps = [
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/geometry:sphere",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "sphere_test",
    srcs = ["sphere_test.cc"],
    deps = [
        ":sphere",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "trianglemesh",
    srcs = ["trianglemesh.cc"],
    hdrs = ["trianglemesh.h"],
    deps = [
        ":alpha_mask",
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris/geometry:triangle_mesh",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "trianglemesh_test",
    srcs = ["trianglemesh_test.cc"],
    deps = [
        ":trianglemesh",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:emissive_material",
        "//iris:geometry",
        "//iris:material",
        "//iris:matrix",
        "//iris:normal_map",
        "//iris:reference_counted",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
