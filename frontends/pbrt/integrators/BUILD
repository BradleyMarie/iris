load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":path",
        ":result",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "path",
    srcs = ["path.cc"],
    hdrs = ["path.h"],
    deps = [
        ":result",
        "//iris:light_scene",
        "//iris/integrators:path_integrator",
        "//iris/light_scenes:one_light_scene",
        "//iris/light_scenes:power_light_scene",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "path_test",
    srcs = ["path_test.cc"],
    deps = [
        ":path",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "result",
    hdrs = ["result.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        "//iris:integrator",
        "//iris:light_scene",
    ],
)
