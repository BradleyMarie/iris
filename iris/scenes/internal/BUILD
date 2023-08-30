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
        "//iris:geometry",
        "//iris:reference_counted",
    ],
)

cc_test(
    name = "bvh_builder_test",
    srcs = ["bvh_builder_test.cc"],
    deps = [
        ":bvh_builder",
        "//iris:reference_counted",
        "//iris/geometry:mock_geometry",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "bvh_node",
    hdrs = ["bvh_node.h"],
    deps = [
        "//iris:bounding_box",
        "//iris:ray",
        "@com_google_absl//absl/log:check",
    ],
)

cc_test(
    name = "bvh_node_test",
    srcs = ["bvh_node_test.cc"],
    deps = [
        ":bvh_node",
        "@com_google_googletest//:gtest_main",
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
        "//iris:hit_allocator",
        "//iris:intersector",
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
        "//iris:reference_counted",
        "//iris:scene_objects",
        "//iris/geometry:mock_geometry",
        "//iris/scenes/internal:bvh_builder",
        "//iris/testing:hit_allocator",
        "//iris/testing:intersector",
        "@com_google_googletest//:gtest_main",
    ],
)
