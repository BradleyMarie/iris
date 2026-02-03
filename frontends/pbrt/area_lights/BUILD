load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//visibility:private"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "diffuse",
    srcs = ["diffuse.cc"],
    hdrs = ["diffuse.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:spectrum_manager",
        "//iris:emissive_material",
        "//iris:reference_counted",
        "//iris/emissive_materials:constant_emissive_material",
        "@pbrt_proto//pbrt_proto:pbrt_cc_proto",
    ],
)

cc_test(
    name = "diffuse_test",
    srcs = ["diffuse_test.cc"],
    deps = [
        ":diffuse",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto:pbrt_cc_proto",
    ],
)

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":diffuse",
        "//frontends/pbrt:spectrum_manager",
        "//iris:emissive_material",
        "//iris:reference_counted",
        "@pbrt_proto//pbrt_proto/v3:v3_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:v3_cc_proto",
    ],
)
