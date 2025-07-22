load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "constant_texture",
    srcs = ["constant_texture.cc"],
    hdrs = ["constant_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
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
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "float_texture",
    srcs = ["float_texture.cc"],
    hdrs = ["float_texture.h"],
    deps = [
        ":mask_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
    ],
)

cc_library(
    name = "image_texture",
    srcs = ["image_texture.cc"],
    hdrs = ["image_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures/internal:uv_texture",
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
        "//iris/testing:spectral_allocator",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mask_texture",
    hdrs = ["mask_texture.h"],
    deps = [
        "//iris:float",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
    ],
)

cc_library(
    name = "mixed_texture",
    srcs = ["mixed_texture.cc"],
    hdrs = ["mixed_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "mixed_texture_test",
    srcs = ["mixed_texture_test.cc"],
    deps = [
        ":constant_texture",
        ":mixed_texture",
        "//iris:float",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "reflector_texture",
    srcs = ["reflector_texture.cc"],
    hdrs = ["reflector_texture.h"],
    deps = [
        ":spectrum_texture",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "reflector_texture_test",
    srcs = ["reflector_texture_test.cc"],
    deps = [
        ":reflector_texture",
        ":spectrum_texture",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "scaled_texture",
    srcs = ["scaled_texture.cc"],
    hdrs = ["scaled_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
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
    name = "spectrum_texture",
    hdrs = ["spectrum_texture.h"],
    deps = [
        "//iris:reference_counted",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:texture_coordinates",
    ],
)

cc_library(
    name = "test_util",
    testonly = True,
    srcs = ["test_util.cc"],
    hdrs = ["test_util.h"],
    deps = [
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
    ],
)
