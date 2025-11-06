load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//iris/geometry:__subpackages__"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "cubic_bezier_curve",
    srcs = ["cubic_bezier_curve.cc"],
    hdrs = ["cubic_bezier_curve.h"],
    deps = [
        "//iris:bounding_box",
        "//iris:float",
        "//iris:matrix",
        "//iris:point",
        "//iris:vector",
    ],
)

cc_test(
    name = "cubic_bezier_curve_test",
    srcs = ["cubic_bezier_curve_test.cc"],
    deps = [
        ":cubic_bezier_curve",
        "//iris:point",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
