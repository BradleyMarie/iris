#include "iris/internal/visibility_tester.h"

#include <memory>
#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/emissive_materials/mock_emissive_material.h"
#include "iris/geometry/mock_geometry.h"
#include "iris/internal/arena.h"
#include "iris/internal/ray_tracer.h"
#include "iris/scenes/list_scene.h"
#include "iris/spectra/mock_spectrum.h"

namespace iris {
namespace internal {
namespace {

using ::iris::emissive_materials::MockEmissiveMaterial;
using ::iris::geometry::MockBasicGeometry;
using ::iris::geometry::MockGeometry;
using ::iris::scenes::ListScene;
using ::iris::spectra::MockSpectrum;
using ::testing::_;
using ::testing::Invoke;
using ::testing::IsFalse;
using ::testing::Return;

static const uint32_t g_data = 0xDEADBEEF;

std::unique_ptr<EmissiveMaterial> MakeEmissiveMaterial(
    std::array<geometric, 2> expected, const Spectrum* spectrum = nullptr) {
  std::unique_ptr<MockEmissiveMaterial> result =
      std::make_unique<MockEmissiveMaterial>();
  EXPECT_CALL(*result, Evaluate(_, _))
      .WillRepeatedly(Invoke(
          [expected, spectrum](const TextureCoordinates& texture_coordinates,
                               SpectralAllocator& spectral_allocator) {
            EXPECT_EQ(expected, texture_coordinates.uv);
            EXPECT_FALSE(texture_coordinates.differentials);
            return spectrum;
          }));
  return result;
}

TEST(VisibilityTesterTest, MissesGeometry) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillOnce(Invoke(
          [](const Ray& ray, HitAllocator& hit_allocator) { return nullptr; }));

  SceneObjects::Builder builder;

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 50.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
}

TEST(VisibilityTesterTest, WrongFace) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockBasicGeometry> geometry =
      MakeReferenceCounted<MockBasicGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillOnce(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 4u, 5u, g_data);
      }));

  SceneObjects::Builder builder;

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 2));
}

TEST(VisibilityTesterTest, SucceedsSecondHit) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        iris::Hit* good_hit =
            &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
        return &hit_allocator.Allocate(good_hit, 0.5, 100, 200, g_data);
      }));

  SceneObjects::Builder builder;

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
}

TEST(VisibilityTesterTest, SceneTraceMisses) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
      }));

  SceneObjects::Builder builder;

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
}

TEST(VisibilityTesterTest, SceneTraceWrongGeometry) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
      }));

  ReferenceCounted<MockGeometry> scene_geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*scene_geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*scene_geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*scene_geometry, GetEmissiveMaterial(_))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*scene_geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
      }));

  SceneObjects::Builder builder;
  builder.Add(std::move(scene_geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry, nullptr, 1));
}

TEST(VisibilityTesterTest, SceneTraceWrongMatrix) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(testing::NotNull()))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(_))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry, Trace(_, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 1, 2, g_data);
      }));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry), Matrix::Translation(1.0, 0.0, 0.0).value());

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, SceneTraceWrongFace) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(_))
      .WillRepeatedly(Return(nullptr));
  {
    testing::InSequence sequence;
    EXPECT_CALL(*geometry, Trace(model_ray, _))
        .WillOnce(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
          return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
        }));
    EXPECT_CALL(*geometry, Trace(model_ray, _))
        .WillOnce(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
          return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 3, 4, g_data);
        }));
  }
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(
      visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1u));
}

TEST(VisibilityTesterTest, NoEmissiveMaterial) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(_))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, NoSpectrum) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0});

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, NoPdf) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 1u, _,
                                                Point(1.0, 0.0, 0.0)))
      .WillOnce(Return(std::nullopt));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, NegativePdf) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 1u, _,
                                                Point(1.0, 0.0, 0.0)))
      .WillOnce(Return(static_cast<visual_t>(-1.0)));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  EXPECT_FALSE(visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1));
}

TEST(VisibilityTesterTest, Succeeds) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 1u, _,
                                                Point(1.0, 0.0, 0.0)))
      .WillOnce(Return(1.0));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  std::optional<VisibilityTester::VisibleResult> result =
      visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(Point(1.0, 0.0, 0.0), result->hit_point);
}

TEST(VisibilityTesterTest, SucceedsWithPdf) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 1u, _,
                                                Point(1.0, 0.0, 0.0)))
      .WillOnce(Return(2.0));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  std::optional<VisibilityTester::VisibleResult> result =
      visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, result->pdf);
}

TEST(VisibilityTesterTest, SucceedsWithTransformWithPdf) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(0.5, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.0, 0.0}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(testing::NotNull()))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(0.5, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return std::nullopt;
          }));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 1u, _,
                                                Point(0.5, 0.0, 0.0)))
      .WillOnce(Return(2.0));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry), Matrix::Scalar(2.0, 1.0, 1.0).value());

  SceneObjects objects = builder.Build();
  const Matrix* matrix = objects.GetGeometry(0).second;
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  std::optional<VisibilityTester::VisibleResult> result =
      visibility_tester.Visible(world_ray, *geometry_ptr, matrix, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, result->pdf);
}

TEST(VisibilityTesterTest, SucceedsWithCoordinates) {
  Ray world_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));
  Ray model_ray(Point(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0));

  std::unique_ptr<MockSpectrum> spectrum = std::make_unique<MockSpectrum>();
  std::unique_ptr<EmissiveMaterial> emissive_material =
      MakeEmissiveMaterial({0.5, 0.5}, spectrum.get());

  ReferenceCounted<MockGeometry> geometry =
      MakeReferenceCounted<MockGeometry>();
  EXPECT_CALL(*geometry, ComputeBounds(nullptr))
      .WillOnce(
          Return(BoundingBox(Point(0.0, 0.0, 0.0), Point(0.0, 1.0, 2.0))));
  EXPECT_CALL(*geometry, GetFaces())
      .WillOnce(Return(std::vector<face_t>({1, 2})));
  EXPECT_CALL(*geometry, Trace(model_ray, _))
      .WillRepeatedly(Invoke([](const Ray& ray, HitAllocator& hit_allocator) {
        return &hit_allocator.Allocate(nullptr, 1.0, 0.0, 1, 2, g_data);
      }));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(1u))
      .WillRepeatedly(Return(emissive_material.get()));
  EXPECT_CALL(*geometry, GetEmissiveMaterial(2u))
      .WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*geometry,
              ComputeTextureCoordinates(Point(1.0, 0.0, 0.0), IsFalse(), 1u, _))
      .WillOnce(Invoke(
          [&](const Point& hit_point,
              const std::optional<Geometry::Differentials>& differentials,
              face_t face, const void* additional_data) {
            EXPECT_EQ(g_data, *static_cast<const uint32_t*>(additional_data));
            return TextureCoordinates{{0.5, 0.5}};
          }));
  EXPECT_CALL(*geometry, ComputePdfBySolidAngle(Point(0.0, 0.0, 0.0), 1u, _,
                                                Point(1.0, 0.0, 0.0)))
      .WillOnce(Return(2.0));
  const Geometry* geometry_ptr = geometry.Get();

  SceneObjects::Builder builder;
  builder.Add(std::move(geometry));

  SceneObjects objects = builder.Build();
  std::unique_ptr<Scene> scene = ListScene::Builder::Create()->Build(objects);

  RayTracer ray_tracer;
  Arena arena;
  VisibilityTester visibility_tester(*scene, 0.0, ray_tracer, arena);

  std::optional<VisibilityTester::VisibleResult> result =
      visibility_tester.Visible(world_ray, *geometry_ptr, nullptr, 1);
  ASSERT_TRUE(result);
  EXPECT_EQ(spectrum.get(), &result->emission);
  EXPECT_EQ(Point(1.0, 0.0, 0.0), result->hit_point);
  EXPECT_EQ(2.0, result->pdf);
}

}  // namespace
}  // namespace internal
}  // namespace iris