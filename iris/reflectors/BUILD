load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_reflector",
    testonly = True,
    hdrs = ["mock_reflector.h"],
    deps = [
        "//iris:float",
        "//iris:reflector",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sampled_reflector",
    srcs = ["sampled_reflector.cc"],
    hdrs = ["sampled_reflector.h"],
    deps = [
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
    ],
)

cc_test(
    name = "sampled_reflector_test",
    srcs = ["sampled_reflector_test.cc"],
    deps = [
        ":sampled_reflector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "uniform_reflector",
    srcs = ["uniform_reflector.cc"],
    hdrs = ["uniform_reflector.h"],
    deps = [
        "//iris:float",
        "//iris:reference_counted",
        "//iris:reflector",
    ],
)

cc_test(
    name = "uniform_reflector_test",
    srcs = ["uniform_reflector_test.cc"],
    deps = [
        ":uniform_reflector",
        "@googletest//:gtest_main",
    ],
)
