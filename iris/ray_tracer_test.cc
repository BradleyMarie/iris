#include "iris/ray_tracer.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/bounding_box.h"
#include "iris/bxdf_allocator.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/emissive_material.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/float.h"
#include "iris/geometry.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/material.h"
#include "iris/materials/mock_material.h"
#include "iris/normal_map.h"
#include "iris/normal_maps/mock_normal_map.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/ray_differential.h"
#include "iris/reference_counted.h"
#include "iris/scene_objects.h"
#include "iris/scenes/list_scene.h"
#include "iris/spectra/mock_spectrum.h"
#include "iris/spectral_allocator.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {
namespace {

using ::iris::bxdfs::MockBxdf;
using ::iris::emissive_materials::MockEmissiveMaterial;
using ::iris::environmental_lights::MockEnvironmentalLight;
using ::iris::geometry::MockBasicGeometry;
using ::iris::geometry::MockGeometry;
using ::iris::materials::MockMaterial;
using ::iris::normal_maps::MockNormalMap;
using ::iris::scenes::MakeListSceneBuilder;
using ::iris::spectra::MockSpectrum;
using ::testing::_;
using ::testing::Invoke;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Return;

static const uint32_t g_data = 0xDEADBEEF;

class Wrapper final : public Geometry {
 public:
  Wrapper(ReferenceCounted<Geometry> geometry)
      : geometry_(std::move(geometry)) {}

  Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                              const void* additional_data) const override {
    std::unreachable();
  }

  ComputeHitPointResult ComputeHitPoint(
      const Ray& ray, geometric_t distance,
      const void* additional_data) const override {
    std::unreachable();
  }

  visual_t ComputeSurfaceArea(face_t face,
                              const Matrix* model_to_world) const override {
    std::unreachable();
  }

  BoundingBox ComputeBounds(const Matrix* model_to_world) const override {
    return geometry_->ComputeBounds(model_to_world);
  }

  std::span<const face_t> GetFaces() const override { return {}; }

 private:
  Hit* Trace(const Ray& ray, geometric_t minimum_distance,
             geometric_t maximum_distance, TraceMode trace_mode,
             HitAllocator& hit_allocator) const override {
    return geometry_->TraceAllHits(hit_allocator);
  }

  ReferenceCounted<Geometry> geometry_;
};

void MakeBasicGeometryImpl(ReferenceCounted<MockBasicGeometry> geometry,
                           const Ray& expected_ray,
                           const Point& expected_hit_point) {
  EXPECT_CALL(*geometry, GetFaces())
      .WillRepeatedly(Return(std::vector<face_t>({1})));
  EXPECT_CALL(*geometry, Trace(expected_ray, _, _, _, _))
      .WillOnce(Invoke([](const Ray& ray, geometric_t minimum_distance,
                          geometric_t maximum_distance,
                          Geometry::TraceMode trace_mode,
                          HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 2u, 3u, g_data);
      }));
  EXPECT_CALL(*geometry, ComputeHitPoint(expected_ray, 1.0, _))
      .WillOnce(Invoke([](const Ray& ray, const geometric_t distance,
                          const void* additional_data) {
        return Geometry::ComputeHitPointResult{ray.Endpoint(1.0),
                                               PositionError(0.0, 0.0, 0.0)};
      }));
  EXPECT_CALL(*geometry, ComputeSurfaceNormal(expected_hit_point, 2u, _))
      .WillRepeatedly(Invoke(
          [](const Point& hit_point, face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Vector(-2.0, 0.0, 0.0);
          }));
}

ReferenceCounted<MockBasicGeometry> MakeBasicGeometry(
    const Ray& expected_ray, const Point& expected_hit_point) {
  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  MakeBasicGeometryImpl(geometry, expected_ray, expected_hit_point);
  return geometry;
}

ReferenceCounted<MockGeometry> MakeGeometry(
    const Ray& expected_ray, const Point& expected_hit_point,
    const Material* material, const EmissiveMaterial* emissive_material) {
  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  MakeBasicGeometryImpl(geometry, expected_ray, expected_hit_point);
  EXPECT_CALL(*geometry, GetMaterial(2u)).WillOnce(Return(material));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u)).WillOnce(Return(nullptr));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillOnce(Return(emissive_material));
  return geometry;
}

std::unique_ptr<Material> MakeMaterial(const Point& expected_hit_point,
                                       std::array<geometric_t, 2> expected_uv,
                                       bool has_differentials = false) {
  std::unique_ptr<MockBxdf> bxdf = std::make_unique<MockBxdf>();
  EXPECT_CALL(*bxdf, IsDiffuse(_)).WillOnce(Return(false));

  std::unique_ptr<MockMaterial> material = std::make_unique<MockMaterial>();
  EXPECT_CALL(*material, Evaluate(_, _, _))
      .WillOnce(Invoke([expected_hit_point, expected_uv, has_differentials,
                        bxdf = std::move(bxdf)](
                           const TextureCoordinates& texture_coordinates,
                           SpectralAllocator& spectral_allocator,
                           BxdfAllocator& allocator) {
        EXPECT_EQ(expected_hit_point, texture_coordinates.p);
        EXPECT_EQ(expected_uv[0], texture_coordinates.uv[0]);
        EXPECT_EQ(expected_uv[1], texture_coordinates.uv[1]);
        return bxdf.get();
      }));

  return material;
}

TEST(RayTracerTest, NoGeometry) {
  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  SceneObjects objects = SceneObjects::Builder().Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, WithEnvironmentalLight) {
  MockSpectrum spectrum;
  MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light, Emission(Vector(1.0, 1.0, 1.0), _, _))
      .WillOnce(Return(&spectrum));

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  SceneObjects objects = SceneObjects::Builder().Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);
  RayTracer ray_tracer(*scene, &environmental_light, 0.0, internal_ray_tracer,
                       arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_EQ(&spectrum, result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, NoBsdf) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));

  ReferenceCounted<MockBasicGeometry> geometry =
      MakeBasicGeometry(ray, Point(1.0, 1.0, 1.0));
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, WithEmissiveMaterial) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  MockEmissiveMaterial emissive_material;
  EXPECT_CALL(emissive_material, Evaluate(_, _))
      .WillOnce(Invoke([&](const TextureCoordinates& texture_coordinates,
                           SpectralAllocator& spectral_allocator) {
        EXPECT_EQ(0.0, texture_coordinates.uv[0]);
        EXPECT_EQ(0.0, texture_coordinates.uv[1]);
        EXPECT_EQ(0.0, texture_coordinates.du_dx);
        EXPECT_EQ(0.0, texture_coordinates.du_dy);
        EXPECT_EQ(0.0, texture_coordinates.dv_dx);
        EXPECT_EQ(0.0, texture_coordinates.dv_dy);
        return spectrum.get();
      }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), nullptr, &emissive_material);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_EQ(spectrum.get(), result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, WithNestedEmissiveMaterial) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  MakeBasicGeometryImpl(geometry, ray, Point(1.0, 1.0, 1.0));
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, GetMaterial(2u)).WillOnce(Return(nullptr));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(_)).Times(0);

  SceneObjects::Builder builder;
  builder.Add(MakeReferenceCounted<Wrapper>(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_EQ(nullptr, result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, Minimal) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {0.0, 0.0});

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    nullptr};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithTextureCoordinates) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {1.0, 1.0});

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Geometry::TextureCoordinates{{1.0, 1.0}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    nullptr};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithMaterial) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {0.0, 0.0});

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    nullptr};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithIdentityNormal) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {0.0, 0.0});

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{Vector(-1.0, 0.0, 0.0),
                                                    std::nullopt, nullptr};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormal) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {0.0, 0.0});

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{Vector(-1.0, -1.0, 0.0),
                                                    std::nullopt, nullptr};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Normalize(Vector(-1.0, -1.0, 0.0)),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithIdentityNormalMap) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {0.0, 0.0});

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(_, IsFalse(), Vector(-1.0, 0.0, 0.0)))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(0.0, texture_coordinates.uv[0]);
            EXPECT_EQ(0.0, texture_coordinates.uv[1]);
            EXPECT_EQ(0.0, texture_coordinates.du_dx);
            EXPECT_EQ(0.0, texture_coordinates.du_dy);
            EXPECT_EQ(0.0, texture_coordinates.dv_dx);
            EXPECT_EQ(0.0, texture_coordinates.dv_dy);
            return surface_normal;
          }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormalMap) {
  Ray ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(1.0, 1.0, 1.0), {0.0, 0.0});

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(_, IsFalse(), Vector(-1.0, 0.0, 0.0)))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(0.0, texture_coordinates.uv[0]);
            EXPECT_EQ(0.0, texture_coordinates.uv[1]);
            EXPECT_EQ(0.0, texture_coordinates.du_dx);
            EXPECT_EQ(0.0, texture_coordinates.du_dy);
            EXPECT_EQ(0.0, texture_coordinates.dv_dx);
            EXPECT_EQ(0.0, texture_coordinates.dv_dy);
            return Vector(-1.0, -1.0, 0.0);
          }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 1.0, 1.0), IsFalse(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(0.0, 0.0, 0.0), Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Normalize(Vector(-1.0, -1.0, 0.0)),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithXYDifferentials) {
  Ray ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(2.0, 0.0, 0.0), {1.0, 1.0}, true);

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(_, IsTrue(), Vector(-1.0, 0.0, 0.0)))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(NormalMap::Differentials::DX_DY, differentials->type);
            EXPECT_EQ(Vector(0.0, 0.0, 1.0), differentials->dp.first);
            EXPECT_EQ(Vector(0.0, 1.0, 0.0), differentials->dp.second);
            return Vector(-1.0, 0.0, 0.0);
          }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(2.0, 0.0, 0.0), IsTrue(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Geometry::TextureCoordinates{{1.0, 1.0}, 1.0, 0.0, 0.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 0.0, 1.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithUVDifferentials) {
  Ray ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(2.0, 0.0, 0.0), {1.0, 1.0}, true);

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(_, IsTrue(), Vector(-1.0, 0.0, 0.0)))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(NormalMap::Differentials::DU_DV, differentials->type);
            EXPECT_EQ(Vector(1.0, 0.0, 1.0), differentials->dp.first);
            EXPECT_EQ(Vector(1.0, 1.0, 0.0), differentials->dp.second);
            return Vector(-1.0, 0.0, 0.0);
          }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(2.0, 0.0, 0.0), IsTrue(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Geometry::TextureCoordinates{{1.0, 1.0}, 1.0, 0.0, 0.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{
            std::nullopt,
            {{Vector(1.0, 0.0, 1.0), Vector(1.0, 1.0, 0.0)}},
            &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 0.0, 1.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormalAndXYDifferentialsNoRotation) {
  Ray ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(2.0, 0.0, 0.0), {1.0, 1.0}, true);

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(_, IsTrue(), Vector(-1.0, 0.0, 0.0)))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(NormalMap::Differentials::DX_DY, differentials->type);
            EXPECT_NEAR(0.0, differentials->dp.first.x, 0.001);
            EXPECT_NEAR(0.0, differentials->dp.first.y, 0.001);
            EXPECT_NEAR(1.0, differentials->dp.first.z, 0.001);
            EXPECT_NEAR(0.0, differentials->dp.second.x, 0.001);
            EXPECT_NEAR(1.0, differentials->dp.second.y, 0.001);
            EXPECT_NEAR(0.0, differentials->dp.second.z, 0.001);
            return Vector(-1.0, 0.0, 0.0);
          }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(2.0, 0.0, 0.0), IsTrue(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Geometry::TextureCoordinates{{1.0, 1.0}, 1.0, 0.0, 0.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{Vector(-1.0, 0.0, 0.0),
                                                    std::nullopt, &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 0.0, 1.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormalAndXYDifferentials) {
  Ray ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  std::unique_ptr<Material> material =
      MakeMaterial(Point(2.0, 0.0, 0.0), {1.0, 1.0}, true);

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map,
              Evaluate(_, IsTrue(), Normalize(Vector(-1.0, -1.0, -1.0))))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(NormalMap::Differentials::DX_DY, differentials->type);
            EXPECT_NEAR(-0.57735025, differentials->dp.first.x, 0.001);
            EXPECT_NEAR(-0.21132485, differentials->dp.first.y, 0.001);
            EXPECT_NEAR(+0.78867506, differentials->dp.first.z, 0.001);
            EXPECT_NEAR(-0.57735025, differentials->dp.second.x, 0.001);
            EXPECT_NEAR(+0.78867506, differentials->dp.second.y, 0.001);
            EXPECT_NEAR(-0.21132481, differentials->dp.second.z, 0.001);
            return Vector(-1.0, 0.0, 0.0);
          }));

  ReferenceCounted<MockGeometry> geometry =
      MakeGeometry(ray, Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(2.0, 0.0, 0.0), IsTrue(), 2u, _))
      .WillOnce(
          Invoke([](const Point& hit_point,
                    const std::optional<Geometry::Differentials>& differentials,
                    face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Geometry::TextureCoordinates{{1.0, 1.0}, 1.0, 0.0, 0.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{Vector(-1.0, -1.0, -1.0),
                                                    std::nullopt, &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(
      RayDifferential(Ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 0.0, 1.0), Vector(1.0, 0.0, 0.0)),
                      Ray(Point(1.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithUVDifferentialsWithTransform) {
  Matrix model_to_world = Matrix::Scalar(2.0, 2.0, 2.0).value();
  RayDifferential trace_ray(Ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0)),
                            Ray(Point(1.0, 0.0, 1.0), Vector(1.0, 0.0, 0.0)),
                            Ray(Point(1.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0)));
  Ray model_ray = model_to_world.InverseMultiplyWithError(trace_ray);
  Ray model_dx_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dx);
  Ray model_dy_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dy);
  Point expected_model_hit_point = model_ray.Endpoint(1.0);
  Point expected_world_dx_hit_point =
      model_to_world.Multiply(model_dx_ray.Endpoint(1.0));
  Point expected_world_dy_hit_point =
      model_to_world.Multiply(model_dy_ray.Endpoint(1.0));

  std::unique_ptr<Material> material =
      MakeMaterial(expected_model_hit_point, {1.0, 1.0}, true);

  MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(_, IsTrue(), Vector(-1.0, 0.0, 0.0)))
      .WillOnce(Invoke(
          [&](const TextureCoordinates& texture_coordinates,
              const std::optional<NormalMap::Differentials>& differentials,
              const Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(NormalMap::Differentials::DU_DV, differentials->type);
            EXPECT_EQ(Vector(2.0, 0.0, 2.0), differentials->dp.first);
            EXPECT_EQ(Vector(2.0, 2.0, 0.0), differentials->dp.second);
            return Vector(-1.0, 0.0, 0.0);
          }));

  ReferenceCounted<MockGeometry> geometry = MakeGeometry(
      model_ray, expected_model_hit_point, material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(&model_to_world))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(expected_model_hit_point,
                                                   IsTrue(), 2u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return Geometry::TextureCoordinates{{1.0, 1.0}, 1.0, 0.0, 0.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{
            std::nullopt,
            {{Vector(1.0, 0.0, 1.0), Vector(1.0, 1.0, 0.0)}},
            &normal_map};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry), model_to_world);

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(trace_ray);
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(model_to_world.MultiplyWithError(expected_model_hit_point).first,
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(expected_world_dx_hit_point,
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(expected_world_dy_hit_point,
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithTransform) {
  Matrix model_to_world = Matrix::Translation(0.0, 0.0, 1.0).value();
  RayDifferential trace_ray(Ray(Point(1.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0)),
                            Ray(Point(1.0, 0.0, 1.0), Vector(1.0, 0.0, 0.0)),
                            Ray(Point(1.0, 1.0, 0.0), Vector(1.0, 0.0, 0.0)));
  Ray model_ray = model_to_world.InverseMultiplyWithError(trace_ray);
  Ray model_dx_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dx);
  Ray model_dy_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dy);
  Point expected_model_hit_point = model_ray.Endpoint(1.0);
  Point expected_model_dx_hit_point = model_dx_ray.Endpoint(1.0);
  Point expected_model_dy_hit_point = model_dy_ray.Endpoint(1.0);
  Point expected_world_dx_hit_point =
      model_to_world.Multiply(model_dx_ray.Endpoint(1.0));
  Point expected_world_dy_hit_point =
      model_to_world.Multiply(model_dy_ray.Endpoint(1.0));

  std::unique_ptr<Material> material =
      MakeMaterial(expected_model_hit_point, {1.0, 1.0}, true);

  ReferenceCounted<MockGeometry> geometry = MakeGeometry(
      model_ray, expected_model_hit_point, material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(&model_to_world))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(expected_model_hit_point,
                                                   IsTrue(), 2u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            EXPECT_EQ(expected_model_dx_hit_point, differentials->dx);
            EXPECT_EQ(expected_model_dy_hit_point, differentials->dy);
            return Geometry::TextureCoordinates{{1.0, 1.0}, 1.0, 0.0, 0.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, _))
      .WillOnce(Invoke([&](face_t face, const void* additional_data) {
        EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
        return Geometry::ComputeShadingNormalResult{std::nullopt, std::nullopt,
                                                    nullptr};
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry), model_to_world);

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = MakeListSceneBuilder()->Build(objects);

  internal::RayTracer internal_ray_tracer;
  internal::Arena arena;
  RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  RayTracer::TraceResult result = ray_tracer.Trace(trace_ray);
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(model_to_world.MultiplyWithError(expected_model_hit_point).first,
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(expected_world_dx_hit_point,
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(expected_world_dy_hit_point,
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

}  // namespace
}  // namespace iris
