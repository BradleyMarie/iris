load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "tinyexr",
    srcs = ["tinyexr.cc"],
    hdrs = ["tinyexr.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":miniz",
    ],
)

cc_library(
    name = "miniz",
    srcs = ["miniz.c"],
    hdrs = ["miniz.h"],
    visibility = ["//visibility:private"],
)
