load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//iris/scenes:__subpackages__"])

cc_library(
    name = "bvh_builder",
    srcs = ["bvh_builder.cc"],
    hdrs = ["bvh_builder.h"],
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
