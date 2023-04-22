load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        "//frontends/pbrt:object_builder",
        "//frontends/pbrt:quoted_string",
        "//frontends/pbrt:texture_manager",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "@com_google_googletest//:gtest_main",
    ],
)
