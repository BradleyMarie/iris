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
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectrum",
        "//iris/color_matchers:cie_color_matcher",
        "//iris/reflectors:sampled_reflector",
        "//iris/spectra:blackbody_spectrum",
        "//iris/spectra:sampled_spectrum",
        "@libspd//libspd/readers:emissive_spd_reader",
        "@libspd//libspd/readers:reflective_spd_reader",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "color_spectrum_manager_test",
    srcs = ["color_spectrum_manager_test.cc"],
    data = [
        "test_data/emissive.spd",
        "test_data/reflective.spd",
    ],
    deps = [
        ":color_spectrum_manager",
        "//frontends/pbrt/spectrum_managers/internal:color_reflector",
        "//frontends/pbrt/spectrum_managers/internal:color_spectrum",
        "//iris:color",
        "@bazel_tools//tools/cpp/runfiles",
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
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectrum",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
