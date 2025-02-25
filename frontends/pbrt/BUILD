load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//frontends/pbrt:__subpackages__"])

cc_library(
    name = "build_objects",
    hdrs = ["build_objects.h"],
    deps = [
        ":object_builder",
        ":parameter_list",
        ":spectrum_manager",
        ":texture_manager",
        ":tokenizer",
    ],
)

cc_test(
    name = "build_objects_test",
    srcs = ["build_objects_test.cc"],
    deps = [
        ":build_objects",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "color",
    hdrs = ["color.h"],
    deps = [
        "//iris:float",
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
        "//frontends/pbrt/spectrum_managers:color_spectrum_manager",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
    ],
)

cc_binary(
    name = "iris",
    srcs = ["main.cc"],
    deps = [
        ":parser",
        ":tokenizer",
        "//frontends/pbrt/spectrum_managers:color_spectrum_manager",
        "//iris/color_matchers:cie_color_matcher",
        "//iris/random:mersenne_twister_random",
        "@abseil-cpp//absl/flags:config",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/flags:usage",
    ],
)

cc_library(
    name = "material_manager",
    srcs = ["material_manager.cc"],
    hdrs = ["material_manager.h"],
    deps = [
        "//frontends/pbrt/materials:material_builder",
    ],
)

cc_test(
    name = "material_manager_test",
    srcs = ["material_manager_test.cc"],
    deps = [
        ":material_manager",
        "//iris/materials:mock_material",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "matrix_manager",
    srcs = ["matrix_manager.cc"],
    hdrs = ["matrix_manager.h"],
    deps = [
        ":quoted_string",
        ":tokenizer",
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
    name = "object_builder",
    hdrs = ["object_builder.h"],
    deps = [
        ":parameter",
        ":parameter_list",
    ],
)

cc_test(
    name = "object_builder_test",
    srcs = ["object_builder_test.cc"],
    deps = [
        ":object_builder",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "parameter_list",
    srcs = ["parameter_list.cc"],
    hdrs = ["parameter_list.h"],
    deps = [
        ":color",
        ":quoted_string",
        ":tokenizer",
        "//iris:point",
        "//iris:vector",
    ],
)

cc_test(
    name = "parameter_list_test",
    srcs = ["parameter_list_test.cc"],
    deps = [
        ":parameter_list",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "parameter",
    srcs = ["parameter.cc"],
    hdrs = ["parameter.h"],
    deps = [
        ":color",
        ":parameter_list",
        ":spectrum_manager",
        ":texture_manager",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:spectrum",
        "//iris/textures:texture2d",
        "@libspd//libspd/readers:emissive_spd_reader",
        "@libspd//libspd/readers:reflective_spd_reader",
    ],
)

cc_test(
    name = "parameter_test",
    srcs = ["parameter_test.cc"],
    data = [
        "test_data/emissive.spd",
        "test_data/include_empty.pbrt",
        "test_data/reflective.spd",
    ],
    deps = [
        ":parameter",
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "parser",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    deps = [
        ":build_objects",
        ":image_manager",
        ":matrix_manager",
        ":quoted_string",
        ":renderable",
        ":spectrum_manager",
        ":texture_manager",
        ":tokenizer",
        "//frontends/pbrt/area_lights:parse",
        "//frontends/pbrt/cameras:parse",
        "//frontends/pbrt/film:parse",
        "//frontends/pbrt/integrators:parse",
        "//frontends/pbrt/lights:parse",
        "//frontends/pbrt/materials:material_builder",
        "//frontends/pbrt/materials:parse",
        "//frontends/pbrt/samplers:parse",
        "//frontends/pbrt/shapes:parse",
        "//frontends/pbrt/textures:parse",
        "//iris:image_sampler",
        "//iris:integrator",
        "//iris:light_scene",
        "//iris/geometry:bvh_aggregate",
        "//iris/scenes:bvh_scene",
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
        "//frontends/pbrt/spectrum_managers:test_spectrum_manager",
        "//iris/albedo_matchers:mock_albedo_matcher",
        "//iris/color_matchers:mock_color_matcher",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random:mersenne_twister_random",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
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
        ":color",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectrum",
    ],
)

cc_library(
    name = "texture_manager",
    srcs = ["texture_manager.cc"],
    hdrs = ["texture_manager.h"],
    deps = [
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
        "//iris/reflectors:mock_reflector",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "quoted_string",
    srcs = ["quoted_string.cc"],
    hdrs = ["quoted_string.h"],
)

cc_test(
    name = "quoted_string_test",
    srcs = ["quoted_string_test.cc"],
    deps = [
        ":quoted_string",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "tokenizer",
    srcs = ["tokenizer.cc"],
    hdrs = ["tokenizer.h"],
)

cc_test(
    name = "tokenizer_test",
    srcs = ["tokenizer_test.cc"],
    data = [
        "test_data/include_empty.pbrt",
    ],
    deps = [
        ":tokenizer",
        "@bazel_tools//tools/cpp/runfiles",
        "@googletest//:gtest_main",
    ],
)
