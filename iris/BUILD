load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//visibility:private"],
    features = [
        "layering_check",
        "parse_headers",
    ],
)

cc_library(
    name = "albedo_matcher",
    hdrs = ["albedo_matcher.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":reflector",
    ],
)

cc_library(
    name = "bounding_box",
    srcs = ["bounding_box.cc"],
    hdrs = ["bounding_box.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":point",
        ":ray",
        ":vector",
    ],
)

cc_test(
    name = "bounding_box_test",
    srcs = ["bounding_box_test.cc"],
    deps = [
        ":bounding_box",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bsdf",
    srcs = ["bsdf.cc"],
    hdrs = ["bsdf.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bxdf",
        ":float",
        ":reflector",
        ":sampler",
        ":spectral_allocator",
        ":vector",
    ],
)

cc_test(
    name = "bsdf_test",
    srcs = ["bsdf_test.cc"],
    deps = [
        ":bsdf",
        ":bxdf",
        ":float",
        ":sampler",
        ":vector",
        "//iris/bxdfs:mock_bxdf",
        "//iris/internal:arena",
        "//iris/random:mock_random",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bxdf_allocator",
    srcs = ["bxdf_allocator.cc"],
    hdrs = ["bxdf_allocator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bxdf",
        "//iris/internal:arena",
    ],
)

cc_test(
    name = "bxdf_allocator_test",
    srcs = ["bxdf_allocator_test.cc"],
    deps = [
        ":bxdf",
        ":bxdf_allocator",
        ":vector",
        "//iris/bxdfs:lambertian_bxdf",
        "//iris/bxdfs:mock_bxdf",
        "//iris/internal:arena",
        "//iris/reflectors:mock_reflector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bxdf",
    hdrs = ["bxdf.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":reflector",
        ":sampler",
        ":spectral_allocator",
        ":vector",
    ],
)

cc_library(
    name = "camera",
    hdrs = ["camera.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":ray_differential",
    ],
)

cc_library(
    name = "color_matcher",
    hdrs = ["color_matcher.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":color",
        ":float",
        ":spectrum",
    ],
)

cc_library(
    name = "color",
    srcs = ["color.cc"],
    hdrs = ["color.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
    ],
)

cc_test(
    name = "color_test",
    srcs = ["color_test.cc"],
    deps = [
        ":color",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "emissive_material",
    hdrs = ["emissive_material.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":power_matcher",
        ":reference_countable",
        ":spectral_allocator",
        ":spectrum",
        ":texture_coordinates",
    ],
)

cc_library(
    name = "environmental_light",
    hdrs = ["environmental_light.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":power_matcher",
        ":reference_countable",
        ":sampler",
        ":spectral_allocator",
        ":spectrum",
        ":vector",
    ],
)

cc_library(
    name = "float",
    hdrs = ["float.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "framebuffer",
    srcs = ["framebuffer.cc"],
    hdrs = ["framebuffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":color",
        ":float",
    ],
)

cc_test(
    name = "framebuffer_test",
    srcs = ["framebuffer_test.cc"],
    deps = [
        ":color",
        ":framebuffer",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "geometry",
    srcs = ["geometry.cc"],
    hdrs = ["geometry.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bounding_box",
        ":emissive_material",
        ":float",
        ":hit",
        ":hit_allocator",
        ":integer",
        ":material",
        ":matrix",
        ":normal_map",
        ":point",
        ":position_error",
        ":random",
        ":ray",
        ":reference_countable",
        ":sampler",
        ":texture_coordinates",
        ":vector",
        "//iris/internal:hit",
        "//iris/internal:hit_arena",
    ],
)

cc_test(
    name = "geometry_test",
    srcs = ["geometry_test.cc"],
    deps = [
        ":float",
        ":geometry",
        ":hit",
        ":hit_allocator",
        ":point",
        ":ray",
        ":sampler",
        ":vector",
        "//iris/geometry:mock_geometry",
        "//iris/internal:hit_arena",
        "//iris/random:mock_random",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "image_sampler",
    hdrs = ["image_sampler.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":random",
    ],
)

cc_library(
    name = "hit",
    hdrs = ["hit.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
    ],
)

cc_library(
    name = "hit_allocator",
    srcs = ["hit_allocator.cc"],
    hdrs = ["hit_allocator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":hit",
        ":integer",
        ":ray",
        "//iris/internal:hit_arena",
    ],
)

cc_test(
    name = "hit_allocator_test",
    srcs = ["hit_allocator_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":hit",
        ":hit_allocator",
        ":point",
        ":ray",
        ":vector",
        "//iris/internal:hit_arena",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "hit_point",
    srcs = ["hit_point.cc"],
    hdrs = ["hit_point.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":point",
        ":position_error",
        ":ray",
        ":vector",
    ],
)

cc_test(
    name = "hit_point_test",
    srcs = ["hit_point_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":hit_point",
        ":point",
        ":position_error",
        ":ray",
        ":vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "integer",
    hdrs = ["integer.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "integrator",
    hdrs = ["integrator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":albedo_matcher",
        ":light_sampler",
        ":random",
        ":ray_differential",
        ":ray_tracer",
        ":spectral_allocator",
        ":spectrum",
        ":visibility_tester",
    ],
)

cc_library(
    name = "intersector",
    srcs = ["intersector.cc"],
    hdrs = ["intersector.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":geometry",
        ":hit",
        ":hit_allocator",
        ":matrix",
        ":ray",
        "//iris/internal:hit",
    ],
)

cc_test(
    name = "intersector_test",
    srcs = ["intersector_test.cc"],
    deps = [
        ":float",
        ":hit",
        ":hit_allocator",
        ":intersector",
        ":matrix",
        ":point",
        ":ray",
        ":vector",
        "//iris/geometry:mock_geometry",
        "//iris/internal:hit_arena",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "light",
    hdrs = ["light.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":hit_point",
        ":power_matcher",
        ":random",
        ":ray",
        ":reference_countable",
        ":spectral_allocator",
        ":spectrum",
        ":vector",
        ":visibility_tester",
    ],
)

cc_library(
    name = "light_sample_allocator",
    srcs = ["light_sample_allocator.cc"],
    hdrs = ["light_sample_allocator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":light",
        ":light_sample",
        "//iris/internal:arena",
    ],
)

cc_test(
    name = "light_sample_allocator_test",
    srcs = ["light_sample_allocator_test.cc"],
    deps = [
        ":float",
        ":light_sample",
        ":light_sample_allocator",
        "//iris/internal:arena",
        "//iris/lights:mock_light",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "light_sample",
    hdrs = ["light_sample.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":light",
    ],
)

cc_library(
    name = "light_sampler",
    srcs = ["light_sampler.cc"],
    hdrs = ["light_sampler.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":light_sample",
        ":light_sample_allocator",
        ":light_scene",
        ":point",
        ":random",
    ],
)

cc_test(
    name = "light_sampler_test",
    srcs = ["light_sampler_test.cc"],
    deps = [
        ":float",
        ":light_sample",
        ":light_sample_allocator",
        ":light_sampler",
        ":point",
        ":random",
        "//iris/internal:arena",
        "//iris/light_scenes:mock_light_scene",
        "//iris/lights:mock_light",
        "//iris/random:mock_random",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "light_scene",
    hdrs = ["light_scene.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":light_sample",
        ":light_sample_allocator",
        ":point",
        ":random",
    ],
)

cc_library(
    name = "material",
    hdrs = ["material.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bxdf",
        ":bxdf_allocator",
        ":reference_countable",
        ":spectral_allocator",
        ":texture_coordinates",
    ],
)

cc_library(
    name = "matrix",
    srcs = ["matrix.cc"],
    hdrs = ["matrix.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bounding_box",
        ":float",
        ":point",
        ":position_error",
        ":ray",
        ":vector",
    ],
)

cc_test(
    name = "matrix_test",
    srcs = ["matrix_test.cc"],
    deps = [
        ":bounding_box",
        ":float",
        ":matrix",
        ":point",
        ":position_error",
        ":ray",
        ":vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "normal_map",
    hdrs = ["normal_map.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_countable",
        ":texture_coordinates",
        ":vector",
    ],
)

cc_library(
    name = "point",
    srcs = ["point.cc"],
    hdrs = ["point.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":vector",
    ],
)

cc_test(
    name = "point_test",
    srcs = ["point_test.cc"],
    deps = [
        ":point",
        ":vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "position_error",
    srcs = ["position_error.cc"],
    hdrs = ["position_error.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
    ],
)

cc_test(
    name = "position_error_test",
    srcs = ["position_error_test.cc"],
    deps = [
        ":position_error",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "power_matcher",
    hdrs = ["power_matcher.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":spectrum",
    ],
)

cc_library(
    name = "random_bitstream",
    srcs = ["random_bitstream.cc"],
    hdrs = ["random_bitstream.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":random",
    ],
)

cc_library(
    name = "random",
    srcs = ["random.cc"],
    hdrs = ["random.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
    ],
)

cc_test(
    name = "random_test",
    srcs = ["random_test.cc"],
    deps = [
        ":random",
        "//iris/random:mock_random",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "ray_differential",
    hdrs = ["ray_differential.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ray",
    ],
)

cc_test(
    name = "ray_differential_test",
    srcs = ["ray_differential_test.cc"],
    deps = [
        ":ray",
        ":ray_differential",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "ray",
    hdrs = ["ray.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":point",
        ":vector",
    ],
)

cc_test(
    name = "ray_test",
    srcs = ["ray_test.cc"],
    deps = [
        ":point",
        ":ray",
        ":vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "ray_tracer",
    srcs = ["ray_tracer.cc"],
    hdrs = ["ray_tracer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bsdf",
        ":bxdf_allocator",
        ":environmental_light",
        ":float",
        ":geometry",
        ":hit_point",
        ":matrix",
        ":normal_map",
        ":point",
        ":ray",
        ":ray_differential",
        ":scene",
        ":spectral_allocator",
        ":spectrum",
        ":texture_coordinates",
        ":vector",
        "//iris/internal:ray_tracer",
    ],
)

cc_test(
    name = "ray_tracer_test",
    srcs = ["ray_tracer_test.cc"],
    deps = [
        ":bounding_box",
        ":bxdf_allocator",
        ":emissive_material",
        ":float",
        ":geometry",
        ":hit_allocator",
        ":integer",
        ":material",
        ":normal_map",
        ":point",
        ":position_error",
        ":ray",
        ":ray_differential",
        ":ray_tracer",
        ":reference_counted",
        ":scene_objects",
        ":spectral_allocator",
        ":texture_coordinates",
        ":vector",
        "//iris/bxdfs:mock_bxdf",
        "//iris/emissive_materials:mock_emissive_material",
        "//iris/environmental_lights:mock_environmental_light",
        "//iris/geometry:mock_geometry",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
        "//iris/materials:mock_material",
        "//iris/normal_maps:mock_normal_map",
        "//iris/scenes:list_scene",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "reference_countable",
    srcs = ["reference_countable.cc"],
    hdrs = ["reference_countable.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "reference_counted",
    hdrs = ["reference_counted.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_countable",
    ],
)

cc_test(
    name = "reference_counted_test",
    srcs = ["reference_counted_test.cc"],
    deps = [
        ":reference_counted",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "reflector",
    hdrs = ["reflector.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":reference_countable",
    ],
)

cc_library(
    name = "renderer",
    srcs = ["renderer.cc"],
    hdrs = ["renderer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":albedo_matcher",
        ":camera",
        ":color",
        ":color_matcher",
        ":environmental_light",
        ":float",
        ":framebuffer",
        ":image_sampler",
        ":integrator",
        ":light_sample_allocator",
        ":light_sampler",
        ":light_scene",
        ":power_matcher",
        ":random",
        ":random_bitstream",
        ":ray_differential",
        ":ray_tracer",
        ":scene",
        ":scene_objects",
        ":spectral_allocator",
        ":spectrum",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
        "//iris/internal:visibility_tester",
    ],
)

cc_test(
    name = "renderer_test",
    srcs = ["renderer_test.cc"],
    deps = [
        ":color",
        ":float",
        ":framebuffer",
        ":image_sampler",
        ":light_scene",
        ":ray_differential",
        ":renderer",
        ":scene",
        ":scene_objects",
        "//iris/albedo_matchers:mock_albedo_matcher",
        "//iris/cameras:mock_camera",
        "//iris/color_matchers:mock_color_matcher",
        "//iris/image_samplers:mock_image_sampler",
        "//iris/integrators:mock_integrator",
        "//iris/light_scenes:all_light_scene",
        "//iris/power_matchers:mock_power_matcher",
        "//iris/random_bitstreams:mock_random_bitstream",
        "//iris/scenes:list_scene",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "sampler",
    srcs = ["sampler.cc"],
    hdrs = ["sampler.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":random",
    ],
)

cc_test(
    name = "sampler_test",
    srcs = ["sampler_test.cc"],
    deps = [
        ":sampler",
        "//iris/random:mock_random",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "scene_objects",
    srcs = ["scene_objects.cc"],
    hdrs = ["scene_objects.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bounding_box",
        ":environmental_light",
        ":geometry",
        ":light",
        ":matrix",
        ":reference_counted",
        "//iris/internal:area_light",
        "//iris/internal:environmental_light",
    ],
)

cc_test(
    name = "scene_objects_test",
    srcs = ["scene_objects_test.cc"],
    deps = [
        ":bounding_box",
        ":geometry",
        ":integer",
        ":light",
        ":matrix",
        ":point",
        ":ray",
        ":reference_counted",
        ":scene_objects",
        ":vector",
        "//iris/emissive_materials:mock_emissive_material",
        "//iris/environmental_lights:mock_environmental_light",
        "//iris/geometry:mock_geometry",
        "//iris/lights:mock_light",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "scene",
    hdrs = ["scene.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":intersector",
        ":ray",
    ],
)

cc_library(
    name = "spectrum",
    hdrs = ["spectrum.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":reference_countable",
    ],
)

cc_library(
    name = "spectral_allocator",
    srcs = ["spectral_allocator.cc"],
    hdrs = ["spectral_allocator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":reflector",
        ":spectrum",
        "//iris/internal:arena",
    ],
)

cc_test(
    name = "spectral_allocator_test",
    srcs = ["spectral_allocator_test.cc"],
    deps = [
        ":bounding_box",
        ":float",
        ":geometry",
        ":integer",
        ":light",
        ":matrix",
        ":point",
        ":ray",
        ":reference_counted",
        ":reflector",
        ":spectral_allocator",
        ":spectrum",
        ":vector",
        "//iris/internal:arena",
        "//iris/reflectors:mock_reflector",
        "//iris/spectra:mock_spectrum",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "texture_coordinates",
    srcs = ["texture_coordinates.cc"],
    hdrs = ["texture_coordinates.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":integer",
        ":point",
        ":vector",
    ],
)

cc_test(
    name = "texture_coordinates_test",
    srcs = ["texture_coordinates_test.cc"],
    deps = [
        ":point",
        ":texture_coordinates",
        ":vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "vector",
    srcs = ["vector.cc"],
    hdrs = ["vector.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
    ],
)

cc_test(
    name = "vector_test",
    srcs = ["vector_test.cc"],
    deps = [
        ":vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "visibility_tester",
    srcs = ["visibility_tester.cc"],
    hdrs = ["visibility_tester.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":float",
        ":ray",
        ":scene",
        "//iris/internal:ray_tracer",
    ],
)

cc_test(
    name = "visibility_tester_test",
    srcs = ["visibility_tester_test.cc"],
    deps = [
        ":bounding_box",
        ":float",
        ":hit_allocator",
        ":point",
        ":ray",
        ":reference_counted",
        ":scene",
        ":scene_objects",
        ":vector",
        ":visibility_tester",
        "//iris/geometry:mock_geometry",
        "//iris/internal:arena",
        "//iris/internal:ray_tracer",
        "//iris/scenes:list_scene",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
