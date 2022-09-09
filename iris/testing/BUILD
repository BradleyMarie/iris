load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = [
    "//iris:__subpackages__",
])

cc_library(
    name = "light_scene_tester",
    testonly = True,
    srcs = ["light_scene_tester.cc"],
    hdrs = ["light_scene_tester.h"],
    deps = [
        "//iris:light_sample",
        "//iris:light_sample_allocator",
        "//iris:light_scene",
        "//iris:point",
        "//iris:random",
        "//iris/internal:arena",
    ],
)

cc_library(
    name = "light_tester",
    testonly = True,
    srcs = ["light_tester.cc"],
    hdrs = ["light_tester.h"],
    deps = [
        "//iris:float",
        "//iris:light",
        "//iris:point",
        "//iris:random",
        "//iris:scene",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
        "//iris/internal:visibility_tester",
    ],
)

cc_library(
    name = "mock_visibility_tester",
    testonly = True,
    srcs = ["mock_visibility_tester.cc"],
    hdrs = ["mock_visibility_tester.h"],
    deps = [
        "//iris:float",
        "//iris:intersector",
        "//iris:ray",
        "//iris:scene",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "scene",
    testonly = True,
    srcs = ["scene.cc"],
    hdrs = ["scene.h"],
    deps = [
        "//iris:float",
        "//iris:geometry",
        "//iris:hit",
        "//iris:matrix",
        "//iris:ray",
        "//iris:scene",
        "//iris/internal:hit",
        "//iris/internal:hit_arena",
        "@com_google_googletest//:gtest",
    ],
)