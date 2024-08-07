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
            EXPECT_FALSE(texture_coordinates.differentials);
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 2));
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
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
  EXPECT_CALL(*scene_geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*scene_geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*scene_geometry, GetEmissiveMaterial(testing::_))
      .WillRepeatedly(testing::Return(nullptr));
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
}

TEST(VisibilityTesterTest, SceneTraceWrongMatrix) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(testing::NotNull()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(testing::_))
      .WillRepeatedly(testing::Return(nullptr));
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, SceneTraceWrongFace) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(testing::_))
      .WillRepeatedly(testing::Return(nullptr));
  {
    testing::InSequence sequence;
    EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
        .WillOnce(testing::Invoke(
            [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
              return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
            }));
    EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
        .WillOnce(testing::Invoke(
            [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
              return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 3, 4, g_data);
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

  EXPECT_FALSE(
      visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1u));
}

TEST(VisibilityTesterTest, NoEmissiveMaterial) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(testing::_))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, NoSpectrum) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0});

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
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

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, NoPdf) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 0.0), 1u, testing::_,
                                     iris::Point(1.0, 0.0, 0.0)))
      .WillOnce(testing::Return(std::nullopt));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, NegativePdf) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 0.0), 1u, testing::_,
                                     iris::Point(1.0, 0.0, 0.0)))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(-1.0)));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, Succeeds) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 0.0), 1u, testing::_,
                                     iris::Point(1.0, 0.0, 0.0)))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1);
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
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 0.0), 1u, testing::_,
                                     iris::Point(1.0, 0.0, 0.0)))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(2.0)));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, result->pdf);
}

TEST(VisibilityTesterTest, SucceedsWithTransformWithPdf) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(0.5, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(testing::NotNull()))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(0.5, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry,
              ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 0.0), 1u, testing::_,
                                     iris::Point(0.5, 0.0, 0.0)))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(2.0)));
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

  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, matrix, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, result->pdf);
}

TEST(VisibilityTesterTest, SucceedsWithCoordinates) {
  iris::Ray world_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));
  iris::Ray model_ray(iris::Point(0.0, 0.0, 0.0), iris::Vector(1.0, 0.0, 0.0));

  auto spectrum = std::make_unique<iris::spectra::MockSpectrum>();
  auto emissive_material = MakeEmissiveMaterial({0.5, 0.5}, spectrum.get());

  auto geometry = iris::MakeReferenceCounted<iris::geometry::MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(testing::Return(iris::BoundingBox(iris::Point(0.0, 0.0, 0.0),
                                                  iris::Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(testing::Return(std::vector<iris::face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, testing::_))
      .WillRepeatedly(testing::Invoke(
          [](const iris::Ray& ray, iris::HitAllocator& hit_allocator) {
            return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
          }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(testing::Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(testing::Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(iris::Point(1.0, 0.0, 0.0),
                                        testing::IsFalse(), 1u, testing::_))
      .WillOnce(testing::Invoke(
          [&](const iris::Point& hit_point,
              const std::optional<iris::Geometry::Differentials>& differentials,
              iris::face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return iris::TextureCoordinates{{0.5, 0.5}};
          }));
  EXPECT_CALL(*geometry,
              ComputePdfBySolidAngle(iris::Point(0.0, 0.0, 0.0), 1u, testing::_,
                                     iris::Point(1.0, 0.0, 0.0)))
      .WillOnce(testing::Return(static_cast<iris::visual_t>(2.0)));
  auto geometry_ptr = geometry.Get();

  auto builder = iris::SceneObjects::Builder();
  builder.Add(std::move(geometry));

  auto objects = builder.Build();
  auto scene = iris::scenes::ListScene::Builder::Create()->Build(objects);

  iris::internal::RayTracer ray_tracer;
  iris::internal::Arena arena;
  iris::internal::VisibilityTester visibility_tester(*scene, 0.0, ray_tracer,
                                                     arena);

  auto result = visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(iris::Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, result->pdf);
}