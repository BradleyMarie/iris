load("@rules_cc//cc:defs.bzl", "cc_library")

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
        "//iris:reference_counted",
        "//iris:scene_objects",
        "//iris/geometry:mock_geometry",
        "//iris/testing:intersector",
        "@com_google_googletest//:gtest_main",
    ],
)
