load("@rules_cc//cc:defs.bzl", "cc_library")

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
