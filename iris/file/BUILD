load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "exr_writer",
    srcs = ["exr_writer.cc"],
    hdrs = ["exr_writer.h"],
    deps = [
        "//iris:framebuffer",
        "//third_party/tinyexr",
    ],
)

cc_test(
    name = "exr_writer_test",
    srcs = ["exr_writer_test.cc"],
    data = [
        "test_data/black.exr",
        "test_data/white.exr",
    ],
    deps = [
        ":exr_writer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "pfm_writer",
    srcs = ["pfm_writer.cc"],
    hdrs = ["pfm_writer.h"],
    deps = [
        "//iris:framebuffer",
    ],
)

cc_test(
    name = "pfm_writer_test",
    srcs = ["pfm_writer_test.cc"],
    data = [
        "test_data/black.pfm",
        "test_data/white.pfm",
    ],
    deps = [
        ":pfm_writer",
        "@com_google_googletest//:gtest_main",
    ],
)
