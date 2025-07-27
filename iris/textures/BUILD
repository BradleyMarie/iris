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
        "//iris:point",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "fbm_texture",
    srcs = ["fbm_texture.cc"],
    hdrs = ["fbm_texture.h"],
    deps = [
        "//iris:float",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "//iris/textures/internal:math",
    ],
)

cc_test(
    name = "fbm_texture_test",
    srcs = ["fbm_texture_test.cc"],
    deps = [
        ":constant_texture",
        ":fbm_texture",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/reflectors:mock_reflector",
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
        "//iris:point",
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
    name = "marble_texture",
    srcs = ["marble_texture.cc"],
    hdrs = ["marble_texture.h"],
    deps = [
        "//iris:float",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures:reflector_texture",
        "//iris/textures/internal:math",
    ],
)

cc_test(
    name = "marble_texture_test",
    srcs = ["marble_texture_test.cc"],
    deps = [
        ":constant_texture",
        ":marble_texture",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/reflectors:mock_reflector",
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
        "//iris:point",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "ptex_texture",
    srcs = ["ptex_texture.cc"],
    hdrs = ["ptex_texture.h"],
    deps = [
        ":float_texture",
        ":reflector_texture",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//third_party/disney/ptex",
    ],
)

cc_test(
    name = "ptex_texture_test",
    srcs = ["ptex_texture_test.cc"],
    data = [
        "test_data/teapot.ptx",
    ],
    deps = [
        ":constant_texture",
        ":ptex_texture",
        ":reflector_texture",
        "//iris:reference_counted",
        "//iris/reflectors:mock_reflector",
        "//third_party/disney/ptex",
        "@bazel_tools//tools/cpp/runfiles",
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
        "//iris:point",
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
        "//iris:point",
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

cc_library(
    name = "windy_texture",
    srcs = ["windy_texture.cc"],
    hdrs = ["windy_texture.h"],
    deps = [
        "//iris:float",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "//iris/textures/internal:math",
    ],
)

cc_test(
    name = "windy_texture_test",
    srcs = ["windy_texture_test.cc"],
    deps = [
        ":constant_texture",
        ":windy_texture",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/reflectors:mock_reflector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "wrinkled_texture",
    srcs = ["wrinkled_texture.cc"],
    hdrs = ["wrinkled_texture.h"],
    deps = [
        "//iris:float",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "//iris/textures/internal:math",
    ],
)

cc_test(
    name = "wrinkled_texture_test",
    srcs = ["wrinkled_texture_test.cc"],
    deps = [
        ":constant_texture",
        ":wrinkled_texture",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/reflectors:mock_reflector",
        "@googletest//:gtest_main",
    ],
)
