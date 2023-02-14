load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//iris/scenes:__subpackages__"])

cc_library(
    name = "bvh_node",
    hdrs = ["bvh_node.h"],
    deps = [
        "//iris:bounding_box",
        "//iris:ray",
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
