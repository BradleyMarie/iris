load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bump_normal_map",
    srcs = ["bump_normal_map.cc"],
    hdrs = ["bump_normal_map.h"],
    deps = [
        "//iris:float",
        "//iris:normal_map",
        "//iris:reference_countable",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris:vector",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "bump_normal_map_test",
    srcs = ["bump_normal_map_test.cc"],
    deps = [
        ":bump_normal_map",
        "//iris/textures:constant_texture",
        "//iris/textures:image_texture",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_normal_map",
    testonly = True,
    hdrs = ["mock_normal_map.h"],
    deps = [
        "//iris:float",
        "//iris:normal_map",
        "//iris:reference_countable",
        "//iris:texture_coordinates",
        "//iris:vector",
        "@com_google_googletest//:gtest",
    ],
)
