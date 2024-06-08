#include "iris/ray_tracer.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/bxdfs/mock_bxdf.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/environmental_lights/mock_environmental_light.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/materials/mock_material.h"
#include "iris/normal_maps/mock_normal_map.h"
#include "iris/scenes/list_scene.h"
#include "iris/spectra/mock_spectrum.h"

static const uint32_t g_data = 0xDEADBEEF;

void MakeBasicGeometryImpl(
    iris::ReferenceCounted<iris::geometry::MockBasicGeometry> geometry,
    const iris::Ray& expected_ray, const iris::Point& expected_hit_point) {
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1})));
  EXPECT_CALL(*geometry, Trace(expected_ray, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 2u, 3u, g_data);
          }));
  EXPECT_CALL(*geometry, ComputeHitPoint(expected_ray, 1.0, testing::_))
      .WillOnce(testing::Invoke([](const iris::Ray& ray,
                                   const iris::geometric_t distance,
                                   const void* additional_data) {
        return iris::Geometry::ComputeHitPointResult{
            ray.Endpoint(1.0), iris::PositionError(0.0, 0.0, 0.0)};
      }));
  EXPECT_CALL(*geometry,
              ComputeSurfaceNormal(expected_hit_point, 2u, testing::_))
      .WillRepeatedly(
          testing::Invoke([](const iris::Point& hit_point, iris::face_t face,
                             const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Vector(-2.0, 0.0, 0.0);
          }));
}

iris::ReferenceCounted<iris::geometry::MockBasicGeometry> MakeBasicGeometry(
    const iris::Ray& expected_ray, const iris::Point& expected_hit_point) {
  auto geometry =
      iris::MakeReferenceCounted<iris::geometry::MockBasicGeometry>();
  MakeBasicGeometryImpl(geometry, expected_ray, expected_hit_point);
  return geometry;
}

iris::ReferenceCounted<iris::geometry::MockGeometry> MakeGeometry(
    const iris::Ray& expected_ray, const iris::Point& expected_hit_point,
    const iris::Material* material,
    const iris::EmissiveMaterial* emissive_material) {
  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  MakeBasicGeometryImpl(geometry, expected_ray, expected_hit_point);
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*geometry, GetMaterial(2u, testing::_))
      .WillOnce(testing::Invoke(
          [material](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return material;
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u, testing::_))
      .WillOnce(testing::Invoke(
          [emissive_material](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return emissive_material;
          }));
  return geometry;
}

std::unique_ptr<iris::Material> MakeMaterial(
    std::array<iris::geometric_t, 2> expected, bool has_differentials = false) {
  auto material = std::make_unique<iris::materials::MockMaterial>();
  auto bxdf = std::make_unique<iris::bxdfs::MockBxdf>();

  EXPECT_CALL(*material, Evaluate(testing::_, testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [expected, has_differentials, bxdf = std::move(bxdf)](
              const iris::TextureCoordinates& texture_coordinates,
              iris::SpectralAllocator& spectral_allocator,
              iris::BxdfAllocator& allocator) {
            EXPECT_EQ(expected, texture_coordinates.uv);
            EXPECT_EQ(has_differentials,
                      texture_coordinates.differentials.has_value());
            return bxdf.get();
          }));

  return material;
}

TEST(RayTracerTest, NoGeometry) {
  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  auto objects = iris::SceneObjects::Builder().Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, WithEnvironmentalLight) {
  iris::spectra::MockSpectrum spectrum;
  iris::environmental_lights::MockEnvironmentalLight environmental_light;
  EXPECT_CALL(environmental_light,
              Emission(iris::Vector(1.0, 1.0, 1.0), testing::_, testing::_))
      .WillOnce(testing::Return(&spectrum));

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  auto objects = iris::SceneObjects::Builder().Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);
  iris::RayTracer ray_tracer(*scene, &environmental_light, 0.0,
                             internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_EQ(&spectrum, result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, NoBsdf) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));

  auto geometry = MakeBasicGeometry(ray, iris::Point(1.0, 1.0, 1.0));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, WithEmissiveMaterial) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  iris::emissive_materials::MockEmissiveMaterial emissive_material;
  EXPECT_CALL(emissive_material, Evaluate(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              iris::SpectralAllocator& spectral_allocator) {
            EXPECT_EQ(0.0, texture_coordinates.uv[0]);
            EXPECT_EQ(0.0, texture_coordinates.uv[1]);
            EXPECT_FALSE(texture_coordinates.differentials);
            return spectrum.get();
          }));

  auto geometry = MakeGeometry(ray, iris::Point(1.0, 1.0, 1.0), nullptr,
                               &emissive_material);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 1.0, 1.0),
                                        testing::IsFalse(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_EQ(spectrum.get(), result.emission);
  EXPECT_FALSE(result.surface_intersection);
}

TEST(RayTracerTest, Minimal) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto material = MakeMaterial({0.0, 0.0});

  auto geometry =
      MakeGeometry(ray, iris::Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 1.0, 1.0),
                                        testing::IsFalse(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt, std::nullopt, nullptr};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithTextureCoordinates) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto material = MakeMaterial({1.0, 1.0});

  auto geometry =
      MakeGeometry(ray, iris::Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 1.0, 1.0),
                                        testing::IsFalse(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{1.0, 1.0};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt, std::nullopt, nullptr};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithMaterial) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto material = MakeMaterial({0.0, 0.0});

  auto geometry =
      MakeGeometry(ray, iris::Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 1.0, 1.0),
                                        testing::IsFalse(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt, std::nullopt, nullptr};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormal) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto material = MakeMaterial({0.0, 0.0});

  auto geometry =
      MakeGeometry(ray, iris::Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 1.0, 1.0),
                                        testing::IsFalse(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                iris::Vector(-1.0, -1.0, 0.0), std::nullopt, nullptr};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Normalize(iris::Vector(-1.0, -1.0, 0.0)),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormalMap) {
  iris::Ray ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0));
  auto material = MakeMaterial({0.0, 0.0});

  iris::normal_maps::MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(testing::_, testing::IsFalse(),
                                   iris::Vector(-1.0, 0.0, 0.0)))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              const std::optional<iris::NormalMap::Differentials>&
                  differentials,
              const iris::Vector& surface_normal) {
            EXPECT_EQ(0.0, texture_coordinates.uv[0]);
            EXPECT_EQ(0.0, texture_coordinates.uv[1]);
            EXPECT_FALSE(texture_coordinates.differentials);
            return iris::Vector(-1.0, -1.0, 0.0);
          }));

  auto geometry =
      MakeGeometry(ray, iris::Point(1.0, 1.0, 1.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 1.0, 1.0),
                                        testing::IsFalse(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt, std::nullopt, &normal_map};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 1.0, 1.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(1.0, 1.0, 1.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  EXPECT_FALSE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Normalize(iris::Vector(-1.0, -1.0, 0.0)),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithXYDifferentials) {
  iris::Ray ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  auto material = MakeMaterial({1.0, 1.0}, true);

  iris::normal_maps::MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(testing::_, testing::IsTrue(),
                                   iris::Vector(-1.0, 0.0, 0.0)))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              const std::optional<iris::NormalMap::Differentials>&
                  differentials,
              const iris::Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(iris::NormalMap::Differentials::DX_DY,
                      differentials->type);
            EXPECT_EQ(iris::Vector(0.0, 0.0, 1.0), differentials->dp.first);
            EXPECT_EQ(iris::Vector(0.0, 1.0, 0.0), differentials->dp.second);
            return iris::Vector(-1.0, 0.0, 0.0);
          }));

  auto geometry =
      MakeGeometry(ray, iris::Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(2.0, 0.0, 0.0),
                                        testing::IsTrue(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{1.0, 1.0}, {{1.0, 0.0, 0.0, 1.0}}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt, std::nullopt, &normal_map};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(iris::Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithUVDifferentials) {
  iris::Ray ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  auto material = MakeMaterial({1.0, 1.0}, true);

  iris::normal_maps::MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(testing::_, testing::IsTrue(),
                                   iris::Vector(-1.0, 0.0, 0.0)))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              const std::optional<iris::NormalMap::Differentials>&
                  differentials,
              const iris::Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(iris::NormalMap::Differentials::DU_DV,
                      differentials->type);
            EXPECT_EQ(iris::Vector(1.0, 0.0, 1.0), differentials->dp.first);
            EXPECT_EQ(iris::Vector(1.0, 1.0, 0.0), differentials->dp.second);
            return iris::Vector(-1.0, 0.0, 0.0);
          }));

  auto geometry =
      MakeGeometry(ray, iris::Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(2.0, 0.0, 0.0),
                                        testing::IsTrue(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{1.0, 1.0}, {{1.0, 0.0, 0.0, 1.0}}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt,
                {{iris::Vector(1.0, 0.0, 1.0), iris::Vector(1.0, 1.0, 0.0)}},
                &normal_map};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(iris::Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormalAndXYDifferentialsNoRotation) {
  iris::Ray ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  auto material = MakeMaterial({1.0, 1.0}, true);

  iris::normal_maps::MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(testing::_, testing::IsTrue(),
                                   iris::Vector(-1.0, 0.0, 0.0)))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              const std::optional<iris::NormalMap::Differentials>&
                  differentials,
              const iris::Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(iris::NormalMap::Differentials::DX_DY,
                      differentials->type);
            EXPECT_NEAR(0.0, differentials->dp.first.x, 0.001);
            EXPECT_NEAR(0.0, differentials->dp.first.y, 0.001);
            EXPECT_NEAR(1.0, differentials->dp.first.z, 0.001);
            EXPECT_NEAR(0.0, differentials->dp.second.x, 0.001);
            EXPECT_NEAR(1.0, differentials->dp.second.y, 0.001);
            EXPECT_NEAR(0.0, differentials->dp.second.z, 0.001);
            return iris::Vector(-1.0, 0.0, 0.0);
          }));

  auto geometry =
      MakeGeometry(ray, iris::Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(2.0, 0.0, 0.0),
                                        testing::IsTrue(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{1.0, 1.0}, {{1.0, 0.0, 0.0, 1.0}}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                iris::Vector(-1.0, 0.0, 0.0), std::nullopt, &normal_map};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(iris::Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithNormalAndXYDifferentials) {
  iris::Ray ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  auto material = MakeMaterial({1.0, 1.0}, true);

  iris::normal_maps::MockNormalMap normal_map;
  EXPECT_CALL(normal_map,
              Evaluate(testing::_, testing::IsTrue(),
                       iris::Normalize(iris::Vector(-1.0, -1.0, -1.0))))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              const std::optional<iris::NormalMap::Differentials>&
                  differentials,
              const iris::Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(iris::NormalMap::Differentials::DX_DY,
                      differentials->type);
            EXPECT_NEAR(-0.57735025, differentials->dp.first.x, 0.001);
            EXPECT_NEAR(-0.21132485, differentials->dp.first.y, 0.001);
            EXPECT_NEAR(+0.78867506, differentials->dp.first.z, 0.001);
            EXPECT_NEAR(-0.57735025, differentials->dp.second.x, 0.001);
            EXPECT_NEAR(+0.78867506, differentials->dp.second.y, 0.001);
            EXPECT_NEAR(-0.21132481, differentials->dp.second.z, 0.001);
            return iris::Vector(-1.0, 0.0, 0.0);
          }));

  auto geometry =
      MakeGeometry(ray, iris::Point(2.0, 0.0, 0.0), material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(2.0, 0.0, 0.0),
                                        testing::IsTrue(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Point& hit_point,
             const std::optional<iris::Geometry::Differentials>& differentials,
             iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{1.0, 1.0}, {{1.0, 0.0, 0.0, 1.0}}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                iris::Vector(-1.0, -1.0, -1.0), std::nullopt, &normal_map};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(iris::RayDifferential(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0))));
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(iris::Point(2.0, 0.0, 0.0),
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(iris::Point(2.0, 0.0, 1.0),
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(iris::Point(2.0, 1.0, 0.0),
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithUVDifferentialsWithTransform) {
  iris::Matrix model_to_world = iris::Matrix::Scalar(2.0, 2.0, 2.0).value();
  iris::RayDifferential trace_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0)));
  iris::Ray model_ray = model_to_world.InverseMultiplyWithError(trace_ray);
  iris::Ray model_dx_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dx);
  iris::Ray model_dy_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dy);
  iris::Point expected_model_hit_point = model_ray.Endpoint(1.0);
  iris::Point expected_world_dx_hit_point =
      model_to_world.Multiply(model_dx_ray.Endpoint(1.0));
  iris::Point expected_world_dy_hit_point =
      model_to_world.Multiply(model_dy_ray.Endpoint(1.0));

  auto material = MakeMaterial({1.0, 1.0}, true);

  iris::normal_maps::MockNormalMap normal_map;
  EXPECT_CALL(normal_map, Evaluate(testing::_, testing::IsTrue(),
                                   iris::Vector(-1.0, 0.0, 0.0)))
      .WillOnce(testing::Invoke(
          [&](const iris::TextureCoordinates& texture_coordinates,
              const std::optional<iris::NormalMap::Differentials>&
                  differentials,
              const iris::Vector& surface_normal) {
            EXPECT_EQ(1.0, texture_coordinates.uv[0]);
            EXPECT_EQ(1.0, texture_coordinates.uv[1]);
            EXPECT_EQ(iris::NormalMap::Differentials::DU_DV,
                      differentials->type);
            EXPECT_EQ(iris::Vector(2.0, 0.0, 2.0), differentials->dp.first);
            EXPECT_EQ(iris::Vector(2.0, 2.0, 0.0), differentials->dp.second);
            return iris::Vector(-1.0, 0.0, 0.0);
          }));

  auto geometry = MakeGeometry(model_ray, expected_model_hit_point,
                               material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(model_to_world))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(expected_model_hit_point,
                                        testing::IsTrue(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{1.0, 1.0}, {{1.0, 0.0, 0.0, 1.0}}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt,
                {{iris::Vector(1.0, 0.0, 1.0), iris::Vector(1.0, 1.0, 0.0)}},
                &normal_map};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry), model_to_world);

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(trace_ray);
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(model_to_world.MultiplyWithError(expected_model_hit_point).first,
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(expected_world_dx_hit_point,
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(expected_world_dy_hit_point,
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}

TEST(RayTracerTest, WithTransform) {
  iris::Matrix model_to_world =
      iris::Matrix::Translation(0.0, 0.0, 1.0).value();
  iris::RayDifferential trace_ray(
      iris::Ray(iris::Point(1.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 0.0, 1.0), iris::Vector(1.0, 0.0, 0.0)),
      iris::Ray(iris::Point(1.0, 1.0, 0.0), iris::Vector(1.0, 0.0, 0.0)));
  iris::Ray model_ray = model_to_world.InverseMultiplyWithError(trace_ray);
  iris::Ray model_dx_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dx);
  iris::Ray model_dy_ray =
      model_to_world.InverseMultiplyWithError(trace_ray.differentials->dy);
  iris::Point expected_model_hit_point = model_ray.Endpoint(1.0);
  iris::Point expected_model_dx_hit_point = model_dx_ray.Endpoint(1.0);
  iris::Point expected_model_dy_hit_point = model_dy_ray.Endpoint(1.0);
  iris::Point expected_world_dx_hit_point =
      model_to_world.Multiply(model_dx_ray.Endpoint(1.0));
  iris::Point expected_world_dy_hit_point =
      model_to_world.Multiply(model_dy_ray.Endpoint(1.0));

  auto material = MakeMaterial({1.0, 1.0}, true);

  auto geometry = MakeGeometry(model_ray, expected_model_hit_point,
                               material.get(), nullptr);
  EXPECT_CALL(*geometry, ComputeBounds(model_to_world))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(expected_model_hit_point,
                                        testing::IsTrue(), 2u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            EXPECT_EQ(expected_model_dx_hit_point, differentials->dx);
            EXPECT_EQ(expected_model_dy_hit_point, differentials->dy);
            return iris::TextureCoordinates{{1.0, 1.0}, {{1.0, 0.0, 0.0, 1.0}}};
          }));
  EXPECT_CALL(*geometry, ComputeShadingNormal(2u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Geometry::ComputeShadingNormalResult{
                std::nullopt, std::nullopt, nullptr};
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry), model_to_world);

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer internal_ray_tracer;
  iris::internal::Arena arena;
  iris::RayTracer ray_tracer(*scene, nullptr, 0.0, internal_ray_tracer, arena);

  auto result = ray_tracer.Trace(trace_ray);
  EXPECT_FALSE(result.emission);
  ASSERT_TRUE(result.surface_intersection);
  EXPECT_EQ(model_to_world.MultiplyWithError(expected_model_hit_point).first,
            result.surface_intersection->hit_point.ApproximateLocation());
  ASSERT_TRUE(result.surface_intersection->differentials);
  EXPECT_EQ(expected_world_dx_hit_point,
            result.surface_intersection->differentials->dx);
  EXPECT_EQ(expected_world_dy_hit_point,
            result.surface_intersection->differentials->dy);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->surface_normal);
  EXPECT_EQ(iris::Vector(-1.0, 0.0, 0.0),
            result.surface_intersection->shading_normal);
}