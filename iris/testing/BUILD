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
        "//iris/internal:arena",
    ],
)

cc_library(
    name = "intersector",
    testonly = True,
    srcs = ["intersector.cc"],
    hdrs = ["intersector.h"],
    deps = [
        "//iris:intersector",
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
        "//iris:light_sample_allocator",
        "//iris:light_sampler",
        "//iris:light_scene",
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
        "//iris:environmental_light",
        "//iris:ray_tracer",
        "//iris:scene",
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
        "//iris:geometry",
        "//iris:matrix",
        "//iris:scene",
        "//iris:visibility_tester",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
        "//iris/internal:visibility_tester",
    ],
)
