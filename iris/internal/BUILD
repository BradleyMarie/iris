load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//iris:__pkg__",
        "//iris/testing:__pkg__",
    ],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "area_light",
    srcs = ["area_light.cc"],
    hdrs = ["area_light.h"],
    deps = [
        ":visibility_tester",
        "//iris:float",
        "//iris:geometry",
        "//iris:hit_point",
        "//iris:integer",
        "//iris:light",
        "//iris:matrix",
        "//iris:power_matcher",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:spectral_allocator",
        "//iris:spectrum",
    ],
)

cc_test(
    name = "area_light_test",
    srcs = ["area_light_test.cc"],
    deps = [
        ":area_light",
        "//iris:hit_allocator",
        "//iris:hit_point",
        "//iris:integer",
        "//iris:light",
        "//iris:point",
        "//iris:position_error",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:sampler",
        "//iris:vector",
        "//iris:visibility_tester",
        "//iris/emissive_materials:mock_emissive_material",
        "//iris/geometry:mock_geometry",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "//iris/testing:visibility_tester",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "arena",
    srcs = ["arena.cc"],
    hdrs = ["arena.h"],
)

cc_test(
    name = "arena_test",
    srcs = ["arena_test.cc"],
    deps = [
        ":arena",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "environmental_light",
    srcs = ["environmental_light.cc"],
    hdrs = ["environmental_light.h"],
    deps = [
        "//iris:environmental_light",
        "//iris:float",
        "//iris:geometry",
        "//iris:hit_point",
        "//iris:integer",
        "//iris:light",
        "//iris:matrix",
        "//iris:power_matcher",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
    ],
)

cc_test(
    name = "environmental_light_test",
    srcs = ["environmental_light_test.cc"],
    deps = [
        ":environmental_light",
        "//iris:float",
        "//iris:hit_point",
        "//iris:light",
        "//iris:point",
        "//iris:position_error",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:sampler",
        "//iris:vector",
        "//iris/environmental_lights:mock_environmental_light",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:spectral_allocator",
        "//iris/testing:visibility_tester",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "hit_arena",
    srcs = ["hit_arena.cc"],
    hdrs = ["hit_arena.h"],
    deps = [
        ":arena",
        ":hit",
        "//iris:float",
        "//iris:integer",
    ],
)

cc_test(
    name = "hit_arena_test",
    srcs = ["hit_arena_test.cc"],
    deps = [
        ":hit",
        ":hit_arena",
        "//iris/geometry:mock_geometry",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "hit",
    hdrs = ["hit.h"],
    deps = [
        "//iris:hit",
        "//iris:integer",
        "//iris:ray",
    ],
)

cc_library(
    name = "ray_tracer",
    srcs = ["ray_tracer.cc"],
    hdrs = ["ray_tracer.h"],
    deps = [
        ":hit",
        ":hit_arena",
        "//iris:float",
        "//iris:intersector",
        "//iris:ray",
        "//iris:scene",
    ],
)

cc_test(
    name = "ray_tracer_test",
    srcs = ["ray_tracer_test.cc"],
    deps = [
        ":hit",
        ":ray_tracer",
        "//iris:bounding_box",
        "//iris:geometry",
        "//iris:hit_allocator",
        "//iris:point",
        "//iris:ray",
        "//iris:scene",
        "//iris:scene_objects",
        "//iris:vector",
        "//iris/geometry:mock_geometry",
        "//iris/scenes:list_scene",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "visibility_tester",
    srcs = ["visibility_tester.cc"],
    hdrs = ["visibility_tester.h"],
    deps = [
        ":arena",
        ":hit",
        ":ray_tracer",
        "//iris:float",
        "//iris:geometry",
        "//iris:integer",
        "//iris:matrix",
        "//iris:point",
        "//iris:ray",
        "//iris:scene",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:visibility_tester",
    ],
)

cc_test(
    name = "visibility_tester_test",
    srcs = ["visibility_tester_test.cc"],
    deps = [
        ":arena",
        ":ray_tracer",
        ":visibility_tester",
        "//iris:bounding_box",
        "//iris:emissive_material",
        "//iris:float",
        "//iris:geometry",
        "//iris:hit_allocator",
        "//iris:integer",
        "//iris:matrix",
        "//iris:point",
        "//iris:ray",
        "//iris:reference_counted",
        "//iris:scene",
        "//iris:scene_objects",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:texture_coordinates",
        "//iris:vector",
        "//iris/emissive_materials:mock_emissive_material",
        "//iris/geometry:mock_geometry",
        "//iris/scenes:list_scene",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
