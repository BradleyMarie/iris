load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "image_environmental_light",
    srcs = ["image_environmental_light.cc"],
    hdrs = ["image_environmental_light.h"],
    deps = [
        "//iris:environmental_light",
        "//iris:float",
        "//iris:matrix",
        "//iris:power_matcher",
        "//iris:reference_counted",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:vector",
        "//iris/environmental_lights/internal:distribution_2d",
    ],
)

cc_test(
    name = "image_environmental_light_test",
    srcs = ["image_environmental_light_test.cc"],
    deps = [
        ":image_environmental_light",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_environmental_light",
    testonly = True,
    hdrs = ["mock_environmental_light.h"],
    deps = [
        "//iris:environmental_light",
        "//iris:float",
        "//iris:power_matcher",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:vector",
        "@googletest//:gtest",
    ],
)
