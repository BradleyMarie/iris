load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "blackbody_spectrum",
    srcs = ["blackbody_spectrum.cc"],
    hdrs = ["blackbody_spectrum.h"],
    deps = [
        "//iris:float",
        "//iris:reference_counted",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "blackbody_spectrum_test",
    srcs = ["blackbody_spectrum_test.cc"],
    deps = [
        ":blackbody_spectrum",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:spectrum",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_spectrum",
    testonly = True,
    hdrs = ["mock_spectrum.h"],
    deps = [
        "//iris:float",
        "//iris:spectrum",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sampled_spectrum",
    srcs = ["sampled_spectrum.cc"],
    hdrs = ["sampled_spectrum.h"],
    deps = [
        "//iris:float",
        "//iris:reference_counted",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "sampled_spectrum_test",
    srcs = ["sampled_spectrum_test.cc"],
    deps = [
        ":sampled_spectrum",
        "//iris:float",
        "//iris:reference_counted",
        "//iris:spectrum",
        "@googletest//:gtest_main",
    ],
)
