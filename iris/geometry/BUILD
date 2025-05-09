load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_test(
    name = "bvh_aggregate_test",
    srcs = ["bvh_aggregate_test.cc"],
    deps = [
        ":bvh_aggregate",
        ":mock_geometry",
        ":sphere",
        "//iris/materials:mock_material",
        "//iris/testing:hit_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bvh_aggregate",
    srcs = ["bvh_aggregate.cc"],
    hdrs = ["bvh_aggregate.h"],
    deps = [
        "//iris:geometry",
        "//iris:reference_counted",
        "//iris/scenes/internal:bvh_builder",
        "//iris/scenes/internal:bvh_traversal",
    ],
)

cc_library(
    name = "mock_geometry",
    testonly = True,
    hdrs = ["mock_geometry.h"],
    deps = [
        "//iris:geometry",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sphere",
    srcs = ["sphere.cc"],
    hdrs = ["sphere.h"],
    deps = [
        "//iris:geometry",
        "//iris:reference_counted",
    ],
)

cc_test(
    name = "sphere_test",
    srcs = ["sphere_test.cc"],
    deps = [
        ":sphere",
        "//iris/emissive_materials:mock_emissive_material",
        "//iris/materials:mock_material",
        "//iris/normal_maps:mock_normal_map",
        "//iris/random:mock_random",
        "//iris/testing:hit_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "triangle_mesh",
    srcs = ["triangle_mesh.cc"],
    hdrs = ["triangle_mesh.h"],
    deps = [
        "//iris:geometry",
        "//iris:reference_counted",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "triangle_mesh_test",
    srcs = ["triangle_mesh_test.cc"],
    deps = [
        ":triangle_mesh",
        "//iris/emissive_materials:mock_emissive_material",
        "//iris/materials:mock_material",
        "//iris/normal_maps:mock_normal_map",
        "//iris/random:mock_random",
        "//iris/testing:hit_allocator",
        "@googletest//:gtest_main",
    ],
)
