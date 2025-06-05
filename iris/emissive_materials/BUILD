load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "constant_emissive_material",
    srcs = ["constant_emissive_material.cc"],
    hdrs = ["constant_emissive_material.h"],
    deps = [
        "//iris:emissive_material",
        "//iris:power_matcher",
        "//iris:reference_counted",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:texture_coordinates",
    ],
)

cc_test(
    name = "constant_emissive_material_test",
    srcs = ["constant_emissive_material_test.cc"],
    deps = [
        ":constant_emissive_material",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_emissive_material",
    testonly = True,
    hdrs = ["mock_emissive_material.h"],
    deps = [
        "//iris:emissive_material",
        "//iris:power_matcher",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "@googletest//:gtest",
    ],
)
