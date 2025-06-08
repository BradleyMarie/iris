load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//frontends/pbrt:__subpackages__"])

cc_library(
    name = "color",
    hdrs = ["color.h"],
    deps = [
        "//iris:float",
    ],
)

cc_binary(
    name = "defaults_embed_generate",
    srcs = ["defaults_embed_generate.cc"],
)

genrule(
    name = "defaults_embed_src",
    srcs = ["@pbrt_proto//pbrt_proto/v3:defaults_binpb"],
    outs = ["defaults_embed.c"],
    cmd = "./$(location :defaults_embed_generate) $(location @pbrt_proto//pbrt_proto/v3:defaults_binpb) > $@",
    tools = [":defaults_embed_generate"],
)

cc_library(
    name = "defaults_embed",
    srcs = [":defaults_embed.c"],
    hdrs = ["defaults_embed.h"],
)

cc_library(
    name = "defaults",
    srcs = ["defaults.cc"],
    hdrs = ["defaults.h"],
    deps = [
        ":defaults_embed",
        "@abseil-cpp//absl/log:check",
        "@pbrt_proto//pbrt_proto/v3:defaults_cc_proto",
    ],
)

cc_library(
    name = "directives",
    srcs = ["directives.cc"],
    hdrs = ["directives.h"],
    deps = [
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "directives_test",
    srcs = ["directives_test.cc"],
    data = [
        "test_data/cornell_box.pbrt",
    ],
    deps = [
        ":directives",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "file",
    srcs = ["file.cc"],
    hdrs = ["file.h"],
    deps = [
        "@abseil-cpp//absl/status:statusor",
        "@pbrt_proto//pbrt_proto/v3:convert",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "file_test",
    srcs = ["file_test.cc"],
    data = [
        "test_data/cornell_box.pbrt",
        "test_data/cornell_box.pbrt.v3.binpb",
        "test_data/cornell_box.pbrt.v3.txtpb",
        "test_data/invalid.pbrt.v3.binpb",
        "test_data/invalid.pbrt.v3.txtpb",
        "test_data/rgba8.png",
    ],
    deps = [
        ":file",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "image_manager",
    srcs = ["image_manager.cc"],
    hdrs = ["image_manager.h"],
    deps = [
        ":spectrum_manager",
        ":texture_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris/textures:image_texture",
        "//third_party/stb:stb_image",
    ],
)

cc_test(
    name = "image_manager_test",
    srcs = ["image_manager_test.cc"],
    data = [
        "test_data/g16.png",
        "test_data/g8.png",
        "test_data/ga16.png",
        "test_data/ga8.png",
        "test_data/rgb16.png",
        "test_data/rgb8.png",
        "test_data/rgba16.png",
        "test_data/rgba8.png",
    ],
    deps = [
        ":image_manager",
        ":texture_manager",
        "//frontends/pbrt/spectrum_managers:color_spectrum_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris/textures:image_texture",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
    ],
)

cc_binary(
    name = "iris",
    srcs = ["main.cc"],
    deps = [
        ":directives",
        ":file",
        ":parser",
        "//iris:framebuffer",
        "//iris:renderer",
        "//iris/random:mersenne_twister_random",
        "@abseil-cpp//absl/flags:config",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/flags:usage",
        "@pbrt_proto//pbrt_proto/v3:convert",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "material_manager",
    srcs = ["material_manager.cc"],
    hdrs = ["material_manager.h"],
    deps = [
        "//frontends/pbrt/materials:result",
        "@pbrt_proto//pbrt_proto/v3:defaults_cc_proto",
    ],
)

cc_test(
    name = "material_manager_test",
    srcs = ["material_manager_test.cc"],
    deps = [
        ":material_manager",
        "//frontends/pbrt/materials:result",
        "//iris/materials:mock_material",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:defaults_cc_proto",
    ],
)

cc_library(
    name = "matrix_manager",
    srcs = ["matrix_manager.cc"],
    hdrs = ["matrix_manager.h"],
    deps = [
        "//iris:matrix",
    ],
)

cc_test(
    name = "matrix_manager_test",
    srcs = ["matrix_manager_test.cc"],
    deps = [
        ":matrix_manager",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "parser",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    deps = [
        ":directives",
        ":file",
        ":image_manager",
        ":matrix_manager",
        ":renderable",
        ":spectrum_manager",
        ":texture_manager",
        "//frontends/pbrt/area_lights:parse",
        "//frontends/pbrt/cameras:parse",
        "//frontends/pbrt/film:parse",
        "//frontends/pbrt/film:result",
        "//frontends/pbrt/integrators:parse",
        "//frontends/pbrt/lights:parse",
        "//frontends/pbrt/materials:parse",
        "//frontends/pbrt/samplers:parse",
        "//frontends/pbrt/shapes:parse",
        "//frontends/pbrt/spectrum_managers:color_spectrum_manager",
        "//frontends/pbrt/textures:parse",
        "//iris:float",
        "//iris:image_sampler",
        "//iris:integrator",
        "//iris:light_scene",
        "//iris:scene",
        "//iris/geometry:bvh_aggregate",
        "//iris/scenes:bvh_scene",
        "@pbrt_proto//pbrt_proto/v3:defaults_cc_proto",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_test(
    name = "parser_test",
    srcs = ["parser_test.cc"],
    data = [
        "test_data/include_circular_first.pbrt",
        "test_data/include_circular_second.pbrt",
        "test_data/include_circular_self.pbrt",
        "test_data/include_circular_third.pbrt",
        "test_data/include_empty.pbrt",
    ],
    deps = [
        ":parser",
        "//iris/random:mersenne_twister_random",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "renderable",
    srcs = ["renderable.cc"],
    hdrs = ["renderable.h"],
    deps = [
        "//iris:camera",
        "//iris:color_matcher",
        "//iris:framebuffer",
        "//iris:image_sampler",
        "//iris:integrator",
        "//iris:light_scene",
        "//iris:random",
        "//iris:renderer",
        "//iris:scene",
        "//iris:scene_objects",
    ],
)

cc_test(
    name = "renderable_test",
    srcs = ["renderable_test.cc"],
    deps = [
        ":renderable",
        "//iris/albedo_matchers:mock_albedo_matcher",
        "//iris/cameras:mock_camera",
        "//iris/color_matchers:mock_color_matcher",
        "//iris/image_samplers:mock_image_sampler",
        "//iris/integrators:mock_integrator",
        "//iris/light_scenes:all_light_scene",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mock_random",
        "//iris/scenes:list_scene",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "spectrum_manager",
    hdrs = ["spectrum_manager.h"],
    deps = [
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectrum",
        "@pbrt_proto//pbrt_proto/v3:pbrt_cc_proto",
    ],
)

cc_library(
    name = "texture_manager",
    srcs = ["texture_manager.cc"],
    hdrs = ["texture_manager.h"],
    deps = [
        ":spectrum_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris/reflectors:uniform_reflector",
        "//iris/textures:constant_texture",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "texture_manager_test",
    srcs = ["texture_manager_test.cc"],
    deps = [
        ":texture_manager",
        "//frontends/pbrt/spectrum_managers:color_spectrum_manager",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
    ],
)
