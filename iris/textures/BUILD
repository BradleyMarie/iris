load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "constant_texture",
    hdrs = ["constant_texture.h"],
    deps = [
        ":texture2d",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "constant_texture_test",
    srcs = ["constant_texture_test.cc"],
    deps = [
        ":constant_texture",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "image_texture",
    hdrs = ["image_texture.h"],
    deps = [
        ":uv_texture",
        "//iris:reference_counted",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "image_texture_test",
    srcs = ["image_texture_test.cc"],
    deps = [
        ":image_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "scaled_texture",
    hdrs = ["scaled_texture.h"],
    deps = [
        ":texture2d",
        "//iris:reference_counted",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "scaled_texture_test",
    srcs = ["scaled_texture_test.cc"],
    deps = [
        ":constant_texture",
        ":scaled_texture",
        "//iris:float",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "texture2d",
    hdrs = ["texture2d.h"],
    deps = [
        "//iris:reference_countable",
        "//iris:texture_coordinates",
    ],
)

cc_library(
    name = "uv_texture",
    hdrs = ["uv_texture.h"],
    deps = [
        ":texture2d",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "uv_texture_test",
    srcs = ["uv_texture_test.cc"],
    deps = [
        ":uv_texture",
        "//iris:float",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "@googletest//:gtest_main",
    ],
)
