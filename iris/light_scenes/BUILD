load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "all_light_scene",
    srcs = ["all_light_scene.cc"],
    hdrs = ["all_light_scene.h"],
    deps = [
        "//iris:light_sample",
        "//iris:light_sample_allocator",
        "//iris:light_scene",
        "//iris:point",
        "//iris:power_matcher",
        "//iris:random",
        "//iris:scene_objects",
    ],
)

cc_test(
    name = "all_light_scene_test",
    srcs = ["all_light_scene_test.cc"],
    deps = [
        ":all_light_scene",
        "//iris/lights:mock_light",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/scenes:list_scene",
        "//iris/testing:light_sample_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_light_scene",
    testonly = True,
    hdrs = ["mock_light_scene.h"],
    deps = [
        "//iris:light_scene",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "one_light_scene",
    srcs = ["one_light_scene.cc"],
    hdrs = ["one_light_scene.h"],
    deps = [
        "//iris:light_sample",
        "//iris:light_sample_allocator",
        "//iris:light_scene",
        "//iris:point",
        "//iris:power_matcher",
        "//iris:random",
        "//iris:scene_objects",
    ],
)

cc_test(
    name = "one_light_scene_test",
    srcs = ["one_light_scene_test.cc"],
    deps = [
        ":one_light_scene",
        "//iris/lights:mock_light",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/scenes:list_scene",
        "//iris/testing:light_sample_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "power_light_scene",
    srcs = ["power_light_scene.cc"],
    hdrs = ["power_light_scene.h"],
    deps = [
        "//iris:light_sample",
        "//iris:light_sample_allocator",
        "//iris:light_scene",
        "//iris:point",
        "//iris:power_matcher",
        "//iris:random",
        "//iris:scene_objects",
    ],
)

cc_test(
    name = "power_light_scene_test",
    srcs = ["power_light_scene_test.cc"],
    deps = [
        ":power_light_scene",
        "//iris/geometry:mock_geometry",
        "//iris/lights:mock_light",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/scenes:list_scene",
        "//iris/testing:light_sample_allocator",
        "@com_google_googletest//:gtest_main",
    ],
)
