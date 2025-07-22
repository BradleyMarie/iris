load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "constant",
    srcs = ["constant.cc"],
    hdrs = ["constant.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "constant_test",
    srcs = ["constant_test.cc"],
    deps = [
        ":constant",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "imagemap",
    srcs = ["imagemap.cc"],
    hdrs = ["imagemap.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:image_manager",
        "//frontends/pbrt:texture_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris/textures:float_texture",
        "//iris/textures:image_texture",
        "//iris/textures:reflector_texture",
        "//iris/textures:scaled_texture",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "imagemap_test",
    srcs = ["imagemap_test.cc"],
    data = [
        "test_data/image",
        "test_data/image.png",
        "test_data/image.txt",
    ],
    deps = [
        ":imagemap",
        "//frontends/pbrt:image_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mix",
    srcs = ["mix.cc"],
    hdrs = ["mix.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris/textures:float_texture",
        "//iris/textures:mixed_texture",
        "//iris/textures:reflector_texture",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "mix_test",
    srcs = ["mix_test.cc"],
    deps = [
        ":mix",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    visibility = ["//frontends/pbrt:__subpackages__"],
    deps = [
        ":constant",
        ":imagemap",
        ":mix",
        ":scale",
        "//frontends/pbrt:image_manager",
        "//frontends/pbrt:texture_manager",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":parse",
        "//frontends/pbrt:image_manager",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "scale",
    srcs = ["scale.cc"],
    hdrs = ["scale.h"],
    deps = [
        "//frontends/pbrt:defaults",
        "//frontends/pbrt:texture_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris/textures:float_texture",
        "//iris/textures:reflector_texture",
        "//iris/textures:scaled_texture",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "scale_test",
    srcs = ["scale_test.cc"],
    deps = [
        ":scale",
        "//frontends/pbrt:texture_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)
