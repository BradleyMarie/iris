load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = [
    "//iris:__subpackages__",
])

cc_library(
    name = "bxdf_allocator",
    testonly = True,
    srcs = ["bxdf_allocator.cc"],
    hdrs = ["bxdf_allocator.h"],
    deps = [
        "//iris:bxdf_allocator",
        "//iris/internal:arena",
    ],
)

cc_library(
    name = "hit_allocator",
    testonly = True,
    srcs = ["hit_allocator.cc"],
    hdrs = ["hit_allocator.h"],
    deps = [
        "//iris:hit_allocator",
        "//iris:integer",
        "//iris:ray",
        "//iris/internal:hit",
        "//iris/internal:hit_arena",
    ],
)

cc_library(
    name = "intersector",
    testonly = True,
    srcs = ["intersector.cc"],
    hdrs = ["intersector.h"],
    deps = [
        "//iris:float",
        "//iris:hit",
        "//iris:intersector",
        "//iris:ray",
        "//iris/internal:hit_arena",
    ],
)

cc_library(
    name = "light_sample_allocator",
    testonly = True,
    srcs = ["light_sample_allocator.cc"],
    hdrs = ["light_sample_allocator.h"],
    deps = [
        "//iris:light_sample_allocator",
        "//iris/internal:arena",
    ],
)

cc_library(
    name = "light_sampler",
    testonly = True,
    srcs = ["light_sampler.cc"],
    hdrs = ["light_sampler.h"],
    deps = [
        "//iris:float",
        "//iris:light",
        "//iris:light_sample",
        "//iris:light_sample_allocator",
        "//iris:light_sampler",
        "//iris:light_scene",
        "//iris:point",
        "//iris:power_matcher",
        "//iris:random",
        "//iris/internal:arena",
        "//iris/random:mock_random",
    ],
)

cc_library(
    name = "ray_tracer",
    testonly = True,
    srcs = ["ray_tracer.cc"],
    hdrs = ["ray_tracer.h"],
    deps = [
        "//iris:bounding_box",
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:environmental_light",
        "//iris:float",
        "//iris:geometry",
        "//iris:hit_allocator",
        "//iris:integer",
        "//iris:intersector",
        "//iris:matrix",
        "//iris:point",
        "//iris:position_error",
        "//iris:power_matcher",
        "//iris:ray",
        "//iris:ray_tracer",
        "//iris:scene",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris:texture_coordinates",
        "//iris:vector",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
    ],
)

cc_library(
    name = "spectral_allocator",
    testonly = True,
    srcs = ["spectral_allocator.cc"],
    hdrs = ["spectral_allocator.h"],
    deps = [
        "//iris:spectral_allocator",
        "//iris/internal:arena",
    ],
)

cc_library(
    name = "visibility_tester",
    testonly = True,
    srcs = ["visibility_tester.cc"],
    hdrs = ["visibility_tester.h"],
    deps = [
        "//iris:bounding_box",
        "//iris:float",
        "//iris:geometry",
        "//iris:hit_allocator",
        "//iris:integer",
        "//iris:intersector",
        "//iris:matrix",
        "//iris:point",
        "//iris:position_error",
        "//iris:ray",
        "//iris:scene",
        "//iris:vector",
        "//iris:visibility_tester",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
        "//iris/internal:visibility_tester",
    ],
)
