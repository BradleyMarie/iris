load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//visibility:private"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "orthographic",
    srcs = ["orthographic.cc"],
    hdrs = ["orthographic.h"],
    deps = [
        "//frontends/pbrt:matrix_manager",
        "//iris:camera",
        "//iris/cameras:orthographic_camera",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "orthographic_test",
    srcs = ["orthographic_test.cc"],
    deps = [
        ":orthographic",
        "//frontends/pbrt:matrix_manager",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":orthographic",
        ":perspective",
        "//frontends/pbrt:matrix_manager",
        "//iris:camera",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt:matrix_manager",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "perspective",
    srcs = ["perspective.cc"],
    hdrs = ["perspective.h"],
    deps = [
        "//frontends/pbrt:matrix_manager",
        "//iris:camera",
        "//iris/cameras:pinhole_camera",
        "//iris/cameras:thin_lens_camera",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "perspective_test",
    srcs = ["perspective_test.cc"],
    deps = [
        ":parse",
        ":perspective",
        "//frontends/pbrt:matrix_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
