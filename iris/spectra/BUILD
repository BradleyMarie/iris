load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_spectrum",
    testonly = True,
    hdrs = ["mock_spectrum.h"],
    deps = [
        "//iris:float",
        "//iris:spectrum",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "sampled_spectrum",
    srcs = ["sampled_spectrum.cc"],
    hdrs = ["sampled_spectrum.h"],
    deps = [
        "//iris:float",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "sampled_spectrum_test",
    srcs = ["sampled_spectrum_test.cc"],
    deps = [
        ":sampled_spectrum",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "uniform_spectrum",
    srcs = ["uniform_spectrum.cc"],
    hdrs = ["uniform_spectrum.h"],
    deps = [
        "//iris:float",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "uniform_spectrum_test",
    srcs = ["uniform_spectrum_test.cc"],
    deps = [
        ":uniform_spectrum",
        "@com_google_googletest//:gtest_main",
    ],
)
