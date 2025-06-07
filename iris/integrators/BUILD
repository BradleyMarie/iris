load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_integrator",
    testonly = True,
    hdrs = ["mock_integrator.h"],
    deps = [
        "//iris:albedo_matcher",
        "//iris:integrator",
        "//iris:light_sampler",
        "//iris:random",
        "//iris:ray_differential",
        "//iris:ray_tracer",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "path_integrator",
    srcs = ["path_integrator.cc"],
    hdrs = ["path_integrator.h"],
    deps = [
        "//iris:albedo_matcher",
        "//iris:float",
        "//iris:integrator",
        "//iris:light_sampler",
        "//iris:random",
        "//iris:ray_differential",
        "//iris:ray_tracer",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
        "//iris/integrators/internal:path_builder",
        "//iris/integrators/internal:russian_roulette",
        "//iris/integrators/internal:sample_direct_lighting",
        "//iris/integrators/internal:sample_indirect_lighting",
    ],
)

cc_test(
    name = "path_integrator_test",
    srcs = ["path_integrator_test.cc"],
    deps = [
        ":path_integrator",
        "//iris:bxdf",
        "//iris:float",
        "//iris:integrator",
        "//iris:point",
        "//iris:ray",
        "//iris:ray_differential",
        "//iris:spectrum",
        "//iris:vector",
        "//iris/albedo_matchers:mock_albedo_matcher",
        "//iris/bxdfs:mock_bxdf",
        "//iris/environmental_lights:mock_environmental_light",
        "//iris/lights:mock_light",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:light_sampler",
        "//iris/testing:ray_tracer",
        "//iris/testing:spectral_allocator",
        "//iris/testing:visibility_tester",
        "@googletest//:gtest_main",
    ],
)
