load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//frontends/pbrt/spectrum_managers:__subpackages__"])

cc_library(
    name = "color_reflector",
    srcs = ["color_reflector.cc"],
    hdrs = ["color_reflector.h"],
    deps = [
        "//iris:color",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
    ],
)

cc_test(
    name = "color_reflector_test",
    srcs = ["color_reflector_test.cc"],
    deps = [
        ":color_reflector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "color_spectrum",
    srcs = ["color_spectrum.cc"],
    hdrs = ["color_spectrum.h"],
    deps = [
        "//iris:color",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "color_spectrum_test",
    srcs = ["color_spectrum_test.cc"],
    deps = [
        ":color_spectrum",
        "@googletest//:gtest_main",
    ],
)
