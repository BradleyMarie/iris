load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_light",
    hdrs = ["mock_light.h"],
    deps = [
        "//iris:float",
        "//iris:hit_point",
        "//iris:light",
        "//iris:power_matcher",
        "//iris:ray",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "point_light",
    srcs = ["point_light.cc"],
    hdrs = ["point_light.h"],
    deps = [
        "//iris:float",
        "//iris:hit_point",
        "//iris:light",
        "//iris:point",
        "//iris:power_matcher",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:sampler",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
    ],
)

cc_test(
    name = "point_light_test",
    srcs = ["point_light_test.cc"],
    deps = [
        ":point_light",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "//iris/testing:visibility_tester",
        "@com_google_googletest//:gtest_main",
    ],
)
