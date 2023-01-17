load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//frontends/pbrt:__subpackages__"])

cc_library(
    name = "linear_rgb_spectrum_manager",
    srcs = ["linear_rgb_spectrum_manager.cc"],
    hdrs = ["linear_rgb_spectrum_manager.h"],
    deps = [
        "//frontends/pbrt:spectrum_manager",
        "//frontends/pbrt/spectrum_managers/internal:color_reflector",
        "//frontends/pbrt/spectrum_managers/internal:color_spectrum",
        "//iris:color",
        "//iris:color_matcher",
        "//iris/color_matchers:cie_color_matcher",
        "//iris/reflectors:sampled_reflector",
        "//iris/spectra:sampled_spectrum",
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
