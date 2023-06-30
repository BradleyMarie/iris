load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "image_environmental_light",
    srcs = ["image_environmental_light.cc"],
    hdrs = ["image_environmental_light.h"],
    deps = [
        "//iris:environmental_light",
        "//iris:float",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:vector",
    ],
)

cc_test(
    name = "image_environmental_light_test",
    srcs = ["image_environmental_light_test.cc"],
    deps = [
        ":image_environmental_light",
        "//iris/random:mock_random",
        "//iris/spectra:uniform_spectrum",
        "//iris/testing:spectral_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_environmental_light",
    testonly = True,
    hdrs = ["mock_environmental_light.h"],
    deps = [
        "//iris:environmental_light",
        "//iris:float",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:vector",
        "@com_google_googletest//:gtest",
    ],
)
