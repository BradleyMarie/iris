load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ashikhmin_shirley_brdf",
    srcs = ["ashikhmin_shirley_brdf.cc"],
    hdrs = ["ashikhmin_shirley_brdf.h"],
    deps = [
        ":microfacet_distribution",
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:diffuse_bxdf",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "ashikhmin_shirley_brdf_test",
    srcs = ["ashikhmin_shirley_brdf_test.cc"],
    deps = [
        ":ashikhmin_shirley_brdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "attenuated_bxdf",
    srcs = ["attenuated_bxdf.cc"],
    hdrs = ["attenuated_bxdf.h"],
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
    name = "attenuated_bxdf_test",
    srcs = ["attenuated_bxdf_test.cc"],
    deps = [
        ":attenuated_bxdf",
        ":mock_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

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
        "@googletest//:gtest_main",
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
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "lambertian_bxdf",
    srcs = ["lambertian_bxdf.cc"],
    hdrs = ["lambertian_bxdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:diffuse_bxdf",
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
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "microfacet_bxdf",
    srcs = ["microfacet_bxdf.cc"],
    hdrs = ["microfacet_bxdf.h"],
    deps = [
        ":fresnel",
        ":microfacet_distribution",
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:diffuse_bxdf",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "microfacet_bxdf_test",
    srcs = ["microfacet_bxdf_test.cc"],
    deps = [
        ":microfacet_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "microfacet_distribution",
    srcs = ["microfacet_distribution.cc"],
    hdrs = ["microfacet_distribution.h"],
    visibility = ["//iris/bxdfs:__subpackages__"],
    deps = [
        "//iris:float",
        "//iris:vector",
        "//iris/bxdfs/internal:math",
    ],
)

cc_test(
    name = "microfacet_distribution_test",
    srcs = ["microfacet_distribution_test.cc"],
    deps = [
        ":microfacet_distribution",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mirror_brdf",
    srcs = ["mirror_brdf.cc"],
    hdrs = ["mirror_brdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:specular_bxdf",
    ],
)

cc_test(
    name = "mirror_brdf_test",
    srcs = ["mirror_brdf_test.cc"],
    deps = [
        ":mirror_brdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
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
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "oren_nayar_bxdf",
    srcs = ["oren_nayar_bxdf.cc"],
    hdrs = ["oren_nayar_bxdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:diffuse_bxdf",
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
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "specular_bxdf",
    srcs = ["specular_bxdf.cc"],
    hdrs = ["specular_bxdf.h"],
    deps = [
        ":fresnel",
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:specular_bxdf",
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
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "transparent_btdf",
    srcs = ["transparent_btdf.cc"],
    hdrs = ["transparent_btdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
        "//iris/bxdfs/helpers:specular_bxdf",
    ],
)

cc_test(
    name = "transparent_btdf_test",
    srcs = ["transparent_btdf_test.cc"],
    deps = [
        ":transparent_btdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)
