load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "diffuse_bxdf",
    hdrs = ["diffuse_bxdf.h"],
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
    name = "diffuse_bxdf_test",
    srcs = ["diffuse_bxdf_test.cc"],
    deps = [
        ":diffuse_bxdf",
        "//iris/random:mock_random",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "specular_bxdf",
    hdrs = ["specular_bxdf.h"],
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
    name = "specular_bxdf_test",
    srcs = ["specular_bxdf_test.cc"],
    deps = [
        ":specular_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)
