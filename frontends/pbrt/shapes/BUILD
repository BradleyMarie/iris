load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "alpha_adapter",
    hdrs = ["alpha_adapter.h"],
    deps = [
        "//iris:reference_counted",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "alpha_adapter_test",
    srcs = ["alpha_adapter_test.cc"],
    deps = [
        ":alpha_adapter",
        "//iris:reference_counted",
        "@com_google_googletest//:gtest_main",
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
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//iris:geometry",
        "//iris:matrix",
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
    name = "plymesh",
    srcs = ["plymesh.cc"],
    hdrs = ["plymesh.h"],
    deps = [
        ":alpha_adapter",
        "//frontends/pbrt:object_builder",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/geometry:triangle_mesh",
        "@plyodine//plyodine/readers:triangle_mesh_reader",
    ],
)

cc_test(
    name = "plymesh_test",
    srcs = ["plymesh_test.cc"],
    data = [
        "test_data/empty.ply",
        "test_data/quads.ply",
        "test_data/triangles.ply",
    ],
    deps = [
        ":plymesh",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "sphere",
    srcs = ["sphere.cc"],
    hdrs = ["sphere.h"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/geometry:sphere",
    ],
)

cc_test(
    name = "sphere_test",
    srcs = ["sphere_test.cc"],
    deps = [
        ":sphere",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "trianglemesh",
    srcs = ["trianglemesh.cc"],
    hdrs = ["trianglemesh.h"],
    deps = [
        ":alpha_adapter",
        "//frontends/pbrt:object_builder",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/geometry:triangle_mesh",
    ],
)

cc_test(
    name = "trianglemesh_test",
    srcs = ["trianglemesh_test.cc"],
    deps = [
        ":trianglemesh",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)
