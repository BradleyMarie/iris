load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "fourier_material",
    srcs = ["fourier_material.cc"],
    hdrs = ["fourier_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:fourier_bxdf",
        "//iris/reflectors:uniform_reflector",
    ],
)

cc_test(
    name = "fourier_material_test",
    srcs = ["fourier_material_test.cc"],
    deps = [
        ":fourier_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "glass_material",
    srcs = ["glass_material.cc"],
    hdrs = ["glass_material.h"],
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
        "//iris/bxdfs:microfacet_bxdf",
        "//iris/bxdfs:specular_dielectric_bxdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "glass_material_test",
    srcs = ["glass_material_test.cc"],
    deps = [
        ":glass_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
    ],
)

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
        "//iris/bxdfs:lambertian_bxdf",
        "//iris/bxdfs:oren_nayar_bxdf",
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
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "metal_material",
    srcs = ["metal_material.cc"],
    hdrs = ["metal_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:microfacet_bxdf",
        "//iris/reflectors:uniform_reflector",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "metal_material_test",
    srcs = ["metal_material_test.cc"],
    deps = [
        ":metal_material",
        "//iris/spectra:mock_spectrum",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mirror_material",
    srcs = ["mirror_material.cc"],
    hdrs = ["mirror_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:mirror_brdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "mirror_material_test",
    srcs = ["mirror_material_test.cc"],
    deps = [
        ":mirror_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mix_material",
    srcs = ["mix_material.cc"],
    hdrs = ["mix_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:attenuated_bxdf",
        "//iris/bxdfs:composite_bxdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "mix_material_test",
    srcs = ["mix_material_test.cc"],
    deps = [
        ":mix_material",
        ":mock_material",
        "//iris/bxdfs:mock_bxdf",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
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
        "@googletest//:gtest",
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
        "//iris/bxdfs:lambertian_bxdf",
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
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "substrate_material",
    srcs = ["substrate_material.cc"],
    hdrs = ["substrate_material.h"],
    deps = [
        "//iris:bxdf",
        "//iris:bxdf_allocator",
        "//iris:float",
        "//iris:material",
        "//iris:reference_counted",
        "//iris:reflector",
        "//iris:spectral_allocator",
        "//iris:texture_coordinates",
        "//iris/bxdfs:ashikhmin_shirley_brdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "substrate_material_test",
    srcs = ["substrate_material_test.cc"],
    deps = [
        ":substrate_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "translucent_material",
    srcs = ["translucent_material.cc"],
    hdrs = ["translucent_material.h"],
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
        "//iris/bxdfs:lambertian_bxdf",
        "//iris/bxdfs:microfacet_bxdf",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "translucent_material_test",
    srcs = ["translucent_material_test.cc"],
    deps = [
        ":translucent_material",
        "//iris/reflectors:mock_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "uber_material",
    srcs = ["uber_material.cc"],
    hdrs = ["uber_material.h"],
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
        "//iris/bxdfs:lambertian_bxdf",
        "//iris/bxdfs:microfacet_bxdf",
        "//iris/bxdfs:specular_dielectric_bxdf",
        "//iris/bxdfs:transparent_btdf",
        "//iris/reflectors:uniform_reflector",
        "//iris/textures:texture2d",
    ],
)

cc_test(
    name = "uber_material_test",
    srcs = ["uber_material_test.cc"],
    deps = [
        ":uber_material",
        "//iris/reflectors:uniform_reflector",
        "//iris/testing:bxdf_allocator",
        "//iris/testing:spectral_allocator",
        "//iris/textures:constant_texture",
        "@googletest//:gtest_main",
    ],
)
