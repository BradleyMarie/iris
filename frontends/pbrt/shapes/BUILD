load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":trianglemesh",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
    ],
)

cc_library(
    name = "plymesh",
    srcs = ["plymesh.cc"],
    hdrs = ["plymesh.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":trianglemesh",
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
        "@tinyply",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "trianglemesh",
    srcs = ["trianglemesh.cc"],
    hdrs = ["trianglemesh.h"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//iris:geometry",
        "//iris:matrix",
        "//iris:reference_counted",
        "//iris/geometry:triangle_mesh",
    ],
)

cc_test(
    name = "trianglemesh_test",
    srcs = ["trianglemesh_test.cc"],
    deps = [
        ":trianglemesh",
        "//frontends/pbrt:build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@com_google_googletest//:gtest_main",
    ],
)
