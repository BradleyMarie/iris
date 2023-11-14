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
        "//iris/reflectors:uniform_reflector",
    ],
)

cc_test(
    name = "fresnel_test",
    srcs = ["fresnel_test.cc"],
    deps = [
        ":fresnel",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "lambertian_brdf",
    srcs = ["lambertian_brdf.cc"],
    hdrs = ["lambertian_brdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
    ],
)

cc_test(
    name = "lambertian_brdf_test",
    srcs = ["lambertian_brdf_test.cc"],
    deps = [
        ":lambertian_brdf",
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
        "//iris/reflectors:uniform_reflector",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mirror_brdf",
    srcs = ["mirror_brdf.cc"],
    hdrs = ["mirror_brdf.h"],
    deps = [
        "//iris:bxdf",
        "//iris:float",
        "//iris:reflector",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:vector",
    ],
)

cc_test(
    name = "mirror_brdf_test",
    srcs = ["mirror_brdf_test.cc"],
    deps = [
        ":mirror_brdf",
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
