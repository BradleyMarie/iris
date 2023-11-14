load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//frontends/pbrt/spectrum_managers:__subpackages__"])

cc_library(
    name = "color_reflector",
    srcs = ["color_reflector.cc"],
    hdrs = ["color_reflector.h"],
    deps = [
        "//frontends/pbrt:color",
        "//iris:color",
        "//iris:float",
        "//iris:reflector",
    ],
)

cc_test(
    name = "color_reflector_test",
    srcs = ["color_reflector_test.cc"],
    deps = [
        ":color_reflector",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "color_spectrum",
    srcs = ["color_spectrum.cc"],
    hdrs = ["color_spectrum.h"],
    deps = [
        "//frontends/pbrt:color",
        "//iris:color",
        "//iris:float",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "color_spectrum_test",
    srcs = ["color_spectrum_test.cc"],
    deps = [
        ":color_spectrum",
        "@com_google_googletest//:gtest_main",
    ],
)
