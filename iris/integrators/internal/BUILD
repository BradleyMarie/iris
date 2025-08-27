load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//iris/integrators:__subpackages__"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "path_builder",
    srcs = ["path_builder.cc"],
    hdrs = ["path_builder.h"],
    deps = [
        "//iris:float",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "path_builder_test",
    srcs = ["path_builder_test.cc"],
    deps = [
        ":path_builder",
        "//iris:float",
        "//iris:reflector",
        "//iris:spectrum",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "russian_roulette",
    srcs = ["russian_roulette.cc"],
    hdrs = ["russian_roulette.h"],
    deps = [
        "//iris:float",
        "//iris:random",
    ],
)

cc_test(
    name = "russian_roulette_test",
    srcs = ["russian_roulette_test.cc"],
    deps = [
        ":russian_roulette",
        "//iris:float",
        "//iris/random:mock_random",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "sample_direct_lighting",
    srcs = ["sample_direct_lighting.cc"],
    hdrs = ["sample_direct_lighting.h"],
    deps = [
        "//iris:bsdf",
        "//iris:float",
        "//iris:light",
        "//iris:light_sampler",
        "//iris:random",
        "//iris:ray",
        "//iris:ray_tracer",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:vector",
        "//iris:visibility_tester",
    ],
)

cc_test(
    name = "sample_direct_lighting_test",
    srcs = ["sample_direct_lighting_test.cc"],
    deps = [
        ":sample_direct_lighting",
        "//iris:bxdf",
        "//iris:float",
        "//iris:hit_point",
        "//iris:light",
        "//iris:position_error",
        "//iris:ray",
        "//iris:ray_tracer",
        "//iris:spectrum",
        "//iris:vector",
        "//iris/bxdfs:mock_bxdf",
        "//iris/lights:mock_light",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:light_sampler",
        "//iris/testing:spectral_allocator",
        "//iris/testing:visibility_tester",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "sample_indirect_lighting",
    srcs = ["sample_indirect_lighting.cc"],
    hdrs = ["sample_indirect_lighting.h"],
    deps = [
        "//iris:bsdf",
        "//iris:ray",
        "//iris:ray_differential",
        "//iris:ray_tracer",
        "//iris:sampler",
        "//iris:spectral_allocator",
    ],
)

cc_test(
    name = "sample_indirect_lighting_test",
    srcs = ["sample_indirect_lighting_test.cc"],
    deps = [
        ":sample_indirect_lighting",
        "//iris:bsdf",
        "//iris:bxdf",
        "//iris:hit_point",
        "//iris:point",
        "//iris:position_error",
        "//iris:ray_differential",
        "//iris:ray_tracer",
        "//iris:sampler",
        "//iris:vector",
        "//iris/bxdfs:mock_bxdf",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
