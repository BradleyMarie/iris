load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//iris/textures:__subpackages__"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "math",
    srcs = ["math.cc"],
    hdrs = ["math.h"],
    deps = [
        "//iris:float",
        "//iris:point",
        "//iris:vector",
    ],
)

cc_test(
    name = "math_test",
    srcs = ["math_test.cc"],
    deps = [
        ":math",
        "//iris:point",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "uv_texture",
    hdrs = ["uv_texture.h"],
    deps = [
        "//iris:float",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
    ],
)

cc_test(
    name = "uv_texture_test",
    srcs = ["uv_texture_test.cc"],
    deps = [
        ":uv_texture",
        "//iris:float",
        "//iris:point",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
