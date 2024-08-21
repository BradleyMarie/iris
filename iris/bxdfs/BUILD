load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "composite_bxdf",
    hdrs = ["composite_bxdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
    ],
)

cc_test(
    name = "composite_bxdf_test",
    srcs = ["composite_bxdf_test.cc"],
    deps = [
        ":composite_bxdf",
        ":mock_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "fresnel",
    srcs = ["fresnel.cc"],
    hdrs = ["fresnel.h"],
    deps = [
        "//iris:float",
        "//iris:spectral_allocator",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "fresnel_test",
    srcs = ["fresnel_test.cc"],
    deps = [
        ":fresnel",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "lambertian_bxdf",
    srcs = ["lambertian_bxdf.cc"],
    hdrs = ["lambertian_bxdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "lambertian_bxdf_test",
    srcs = ["lambertian_bxdf_test.cc"],
    deps = [
        ":lambertian_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "math",
    hdrs = ["math.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//iris:float",
        "//iris:vector",
    ],
)

cc_test(
    name = "math_test",
    srcs = ["math_test.cc"],
    deps = [
        ":math",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "microfacet_bxdf",
    srcs = ["microfacet_bxdf.cc"],
    hdrs = ["microfacet_bxdf.h"],
    deps = [
        ":fresnel",
        ":math",
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
    ],
)

cc_test(
    name = "microfacet_bxdf_test",
    srcs = ["microfacet_bxdf_test.cc"],
    deps = [
        ":microfacet_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_bxdf",
    hdrs = ["mock_bxdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "oren_nayar_bxdf",
    srcs = ["oren_nayar_bxdf.cc"],
    hdrs = ["oren_nayar_bxdf.h"],
    deps = [
        ":math",
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "oren_nayar_bxdf_test",
    srcs = ["oren_nayar_bxdf_test.cc"],
    deps = [
        ":oren_nayar_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "specular_bxdf",
    srcs = ["specular_bxdf.cc"],
    hdrs = ["specular_bxdf.h"],
    deps = [
        ":fresnel",
        ":math",
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "specular_bxdf_test",
    srcs = ["specular_bxdf_test.cc"],
    deps = [
        ":specular_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)
