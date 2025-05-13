load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "distant",
    srcs = ["distant.cc"],
    hdrs = ["distant.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:spectrum_manager",
        "//iris:light",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/lights:directional_light",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "distant_test",
    srcs = ["distant_test.cc"],
    deps = [
        ":distant",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:matrix",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "infinite",
    srcs = ["infinite.cc"],
    hdrs = ["infinite.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:spectrum_manager",
        "//iris:environmental_light",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/environmental_lights:image_environmental_light",
        "//third_party/stb:stb_image",
        "//third_party/tinyexr",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "infinite_test",
    srcs = ["infinite_test.cc"],
    data = [
        "test_data/image",
        "test_data/image.exr",
        "test_data/image.png",
        "test_data/image.txt",
    ],
    deps = [
        ":infinite",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:matrix",
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
        ":distant",
        ":infinite",
        "//frontends/pbrt:spectrum_manager",
        "//iris:environmental_light",
        "//iris:light",
        "//iris:matrix",
        "//iris:reference_counted",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris:environmental_light",
        "//iris:light",
        "//iris:matrix",
        "//iris:reference_counted",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
