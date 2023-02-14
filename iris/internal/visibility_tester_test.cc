#include "iris/internal/visibility_tester.h"

#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/scenes/list_scene.h"
#include "iris/spectra/mock_spectrum.h"

static const uint32_t g_data = 0xDEADBEEF;

std::unique_ptr<iris::EmissiveMaterial> MakeEmissiveMaterial(
    std::array<iris::geometric, 2> expected,
    const iris::Spectrum* spectrum = nullptr) {
  auto result =
      std::make_unique<iris::emissive_materials::MockEmissiveMaterial>();
  EXPECT_CALL(*result, Evaluate(testing::_, testing::_))
      .WillRepeatedly(testing::Invoke(
          [expected, spectrum](
              const iris::TextureCoordinates& texture_coordinates,
              iris::SpectralAllocator& spectral_allocator) {
            EXPECT_EQ(expected, texture_coordinates.uv);
            EXPECT_FALSE(texture_coordinates.derivatives);
            return spectrum;
          }));
  return result;
}

TEST(VisibilityTesterTest, MissesGeometry) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry =
      iris::MakeReferenceCounted<iris::geometry::MockBasicGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return nullptr;
          }));

  auto builder = iris::SceneObjects::Builder();

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 50.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1, 1.0,
                                         nullptr));
}

TEST(VisibilityTesterTest, WrongFace) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry =
      iris::MakeReferenceCounted<iris::geometry::MockBasicGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillOnce(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 4u, 5u, g_data);
          }));

  auto builder = iris::SceneObjects::Builder();

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 2, 1.0,
                                         nullptr));
}

TEST(VisibilityTesterTest, SucceedsSecondHit) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke([](const iris::Ray& ray,
                                         iris::HitAllocator& hit_allocator) {
        auto* good_hit = &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
        return &hit_allocator.Allocate(good_hit, 0.5, 100, 200, g_data);
      }));

  auto builder = iris::SceneObjects::Builder();

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1, 1.0,
                                         nullptr));
}

TEST(VisibilityTesterTest, SceneTraceMisses) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));

  auto builder = iris::SceneObjects::Builder().Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(builder);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1, 1.0,
                                         nullptr));
}

TEST(VisibilityTesterTest, SceneTraceWrongGeometry) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));

  auto scene_geometry =
      iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*scene_geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*scene_geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*scene_geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*scene_geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(scene_geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1, 1.0,
                                         nullptr));
}

TEST(VisibilityTesterTest, SceneTraceWrongMatrix) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry,
              ComputeBounds(iris::Matrix::Translation(1.0, 0.0, 0.0).value()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(testing::_, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry),
              iris::Matrix::Translation(1.0, 0.0, 0.0).value());

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1,
                                         1.0, nullptr));
}

TEST(VisibilityTesterTest, SceneTraceWrongFace) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  {
    testing::InSequence sequence;
    EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
        .WillOnce(testing::Invoke(
            [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
              return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
            }));
    EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
        .WillOnce(testing::Invoke(
            [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
              return &hit_allocator.Allocate(nullptr, 1.0, 3, 4, g_data);
            }));
  }
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1u,
                                         1.0, nullptr));
}

TEST(VisibilityTesterTest, NoEmissiveMaterial) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return nullptr;
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                                   1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1,
                                         1.0, nullptr));
}

TEST(VisibilityTesterTest, NoSpectrum) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0});

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return emissive_material.get();
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                                   1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1,
                                         1.0, nullptr));
}

TEST(VisibilityTesterTest, Succeeds) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return emissive_material.get();
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                                   1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1,
                                          1.0, nullptr);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
}

TEST(VisibilityTesterTest, SucceedsWithPdf) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return emissive_material.get();
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                                   1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputeSurfaceNormal(iris::Point(1.0, 0.0, 0.0), 1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Vector(-1.0, 0.0, 0.0);
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  iris::visual_t pdf;
  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1,
                                          0.5, &pdf);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, pdf);
}

TEST(VisibilityTesterTest, SucceedsWithTransformWithPdf) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.5, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry,
              ComputeBounds(iris::Matrix::Scalar(2.0, 1.0, 1.0).value()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return emissive_material.get();
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(iris::Point(0.5, 0.0, 0.0),
                                                   1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputeSurfaceNormal(iris::Point(0.5, 0.0, 0.0), 1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::Vector(-1.0, 0.0, 0.0);
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry), iris::Matrix::Scalar(2.0, 1.0, 1.0).value());

  auto objects = builder.Build();
  const auto* matrix = objects.GetGeometry(0).second;
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  iris::visual_t pdf;
  auto result =
      visibility_tester.Visible(world_ray, *geometry_ptr, matrix, 1, 0.5, &pdf);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, pdf);
}

TEST(VisibilityTesterTest, SucceedsWithCoordinates) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.5, 0.5}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, IsEmissive(testing::_))
      .WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*geometry, ComputeBounds(iris::Matrix::Identity()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u, testing::_))
      .WillOnce(
          testing::Invoke([&](iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return emissive_material.get();
          }));
  EXPECT_CALL(*geometry, ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                                   1u, testing::_))
      .WillOnce(
          testing::Invoke([&](const iris::Point& hit_point, iris::face_t face,
                              const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{0.5, 0.5}};
          }));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1,
                                          1.0, nullptr);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
}