load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bump_normal_map",
    srcs = ["bump_normal_map.cc"],
    hdrs = ["bump_normal_map.h"],
    deps = [
        "//iris:float",
        "//iris:normal_map",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris:vector",
        "//iris/textures:float_texture",
    ],
)

cc_test(
    name = "bump_normal_map_test",
    srcs = ["bump_normal_map_test.cc"],
    deps = [
        ":bump_normal_map",
        "//iris:float",
        "//iris:normal_map",
        "//iris:point",
        "//iris:reference_counted",
        "//iris:texture_coordinates",
        "//iris/textures:float_texture",
        "//iris/textures:image_texture",
        "//iris/textures:test_util",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_normal_map",
    testonly = True,
    hdrs = ["mock_normal_map.h"],
    deps = [
        "//iris:normal_map",
        "//iris:texture_coordinates",
        "//iris:vector",
        "@googletest//:gtest",
    ],
)
