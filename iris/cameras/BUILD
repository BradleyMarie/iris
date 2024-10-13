load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_camera",
    testonly = True,
    hdrs = ["mock_camera.h"],
    deps = [
        "//iris:camera",
        "//iris:float",
        "//iris:ray_differential",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "orthographic_camera",
    srcs = ["orthographic_camera.cc"],
    hdrs = ["orthographic_camera.h"],
    deps = [
        "//iris:camera",
        "//iris:float",
        "//iris:matrix",
        "//iris:ray_differential",
    ],
)

cc_test(
    name = "orthographic_camera_test",
    srcs = ["orthographic_camera_test.cc"],
    deps = [
        ":orthographic_camera",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "pinhole_camera",
    srcs = ["pinhole_camera.cc"],
    hdrs = ["pinhole_camera.h"],
    deps = [
        "//iris:camera",
        "//iris:float",
        "//iris:matrix",
        "//iris:ray_differential",
        "//iris:vector",
    ],
)

cc_test(
    name = "pinhole_camera_test",
    srcs = ["pinhole_camera_test.cc"],
    deps = [
        ":pinhole_camera",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "thin_lens_camera",
    srcs = ["thin_lens_camera.cc"],
    hdrs = ["thin_lens_camera.h"],
    deps = [
        "//iris:camera",
        "//iris:float",
        "//iris:matrix",
        "//iris:ray_differential",
        "//iris:vector",
    ],
)

cc_test(
    name = "thin_lens_camera_test",
    srcs = ["thin_lens_camera_test.cc"],
    deps = [
        ":thin_lens_camera",
        "@com_google_googletest//:gtest_main",
    ],
)
