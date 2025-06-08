load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bvh_scene",
    srcs = ["bvh_scene.cc"],
    hdrs = ["bvh_scene.h"],
    deps = [
        "//iris:intersector",
        "//iris:ray",
        "//iris:scene",
        "//iris:scene_objects",
        "//iris/scenes/internal:bvh_builder",
        "//iris/scenes/internal:bvh_node",
        "//iris/scenes/internal:bvh_traversal",
    ],
)

cc_test(
    name = "bvh_scene_test",
    srcs = ["bvh_scene_test.cc"],
    deps = [
        ":bvh_scene",
        "//iris:bounding_box",
        "//iris:hit",
        "//iris:integer",
        "//iris:intersector",
        "//iris:point",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:scene",
        "//iris:scene_objects",
        "//iris:vector",
        "//iris/geometry:mock_geometry",
        "//iris/testing:intersector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "list_scene",
    srcs = ["list_scene.cc"],
    hdrs = ["list_scene.h"],
    deps = [
        "//iris:intersector",
        "//iris:ray",
        "//iris:scene",
        "//iris:scene_objects",
    ],
)

cc_test(
    name = "list_scene_test",
    srcs = ["list_scene_test.cc"],
    deps = [
        ":list_scene",
        "//iris:bounding_box",
        "//iris:hit",
        "//iris:integer",
        "//iris:intersector",
        "//iris:point",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:scene",
        "//iris:scene_objects",
        "//iris:vector",
        "//iris/geometry:mock_geometry",
        "//iris/testing:intersector",
        "@googletest//:gtest_main",
    ],
)
