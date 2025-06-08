load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//iris/scenes:__subpackages__"])

cc_library(
    name = "bvh_builder",
    srcs = ["bvh_builder.cc"],
    hdrs = ["bvh_builder.h"],
    visibility = [
        "//iris/geometry:__subpackages__",
        "//iris/scenes:__subpackages__",
    ],
    deps = [
        ":bvh_node",
        "//iris:bounding_box",
        "//iris:float",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris:vector",
    ],
)

cc_test(
    name = "bvh_builder_test",
    srcs = ["bvh_builder_test.cc"],
    deps = [
        ":bvh_builder",
        ":bvh_node",
        "//iris:bounding_box",
        "//iris:float",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:point",
        "//iris:reference_counted",
        "//iris/geometry:mock_geometry",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bvh_node",
    srcs = ["bvh_node.cc"],
    hdrs = ["bvh_node.h"],
    deps = [
        "//iris:bounding_box",
        "//iris:float",
        "//iris:ray",
        "//iris:vector",
    ],
)

cc_test(
    name = "bvh_node_test",
    srcs = ["bvh_node_test.cc"],
    deps = [
        ":bvh_node",
        "//iris:bounding_box",
        "//iris:point",
        "//iris:ray",
        "//iris:vector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bvh_traversal",
    srcs = ["bvh_traversal.cc"],
    hdrs = ["bvh_traversal.h"],
    visibility = [
        "//iris/geometry:__subpackages__",
        "//iris/scenes:__subpackages__",
    ],
    deps = [
        "//iris:float",
        "//iris:geometry",
        "//iris:hit",
        "//iris:hit_allocator",
        "//iris:intersector",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:scene_objects",
        "//iris/scenes/internal:bvh_builder",
        "//iris/scenes/internal:bvh_node",
    ],
)

cc_test(
    name = "bvh_traversal_test",
    srcs = ["bvh_traversal_test.cc"],
    deps = [
        ":bvh_traversal",
        "//iris:bounding_box",
        "//iris:geometry",
        "//iris:hit",
        "//iris:hit_allocator",
        "//iris:integer",
        "//iris:intersector",
        "//iris:point",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:scene_objects",
        "//iris:vector",
        "//iris/geometry:mock_geometry",
        "//iris/scenes/internal:bvh_builder",
        "//iris/testing:hit_allocator",
        "//iris/testing:intersector",
        "@googletest//:gtest_main",
    ],
)
