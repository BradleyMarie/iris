load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "matte_material",
    srcs = ["matte_material.cc"],
    hdrs = ["matte_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:lambertian_brdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "matte_material_test",
    srcs = ["matte_material_test.cc"],
    deps = [
        ":matte_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_material",
    testonly = True,
    hdrs = ["mock_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:material",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "plastic_material",
    srcs = ["plastic_material.cc"],
    hdrs = ["plastic_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:composite_bxdf",
        "//iris/bxdfs:lambertian_brdf",
        "//iris/bxdfs:microfacet_bxdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "plastic_material_test",
    srcs = ["plastic_material_test.cc"],
    deps = [
        ":plastic_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@com_google_googletest//:gtest_main",
    ],
)
