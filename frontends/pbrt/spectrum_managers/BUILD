load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//frontends/pbrt:__subpackages__"])

cc_library(
    name = "color_spectrum_manager",
    srcs = ["color_spectrum_manager.cc"],
    hdrs = ["color_spectrum_manager.h"],
    deps = [
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt/spectrum_managers/internal:color_reflector",
        "//frontends/pbrt/spectrum_managers/internal:color_spectrum",
        "//iris:albedo_matcher",
        "//iris:color",
        "//iris:color_matcher",
        "//iris:power_matcher",
        "//iris/color_matchers:cie_color_matcher",
        "//iris/reflectors:sampled_reflector",
        "//iris/spectra:sampled_spectrum",
    ],
)

cc_test(
    name = "color_spectrum_manager_test",
    srcs = ["color_spectrum_manager_test.cc"],
    deps = [
        ":color_spectrum_manager",
        "//frontends/pbrt/spectrum_managers/internal:color_reflector",
        "//frontends/pbrt/spectrum_managers/internal:color_spectrum",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "test_spectrum_manager",
    testonly = True,
    srcs = ["test_spectrum_manager.cc"],
    hdrs = ["test_spectrum_manager.h"],
    deps = [
        "//frontends/pbrt:spectrum_manager",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
    ],
)
