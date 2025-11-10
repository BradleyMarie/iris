#include "iris/geometry/curve.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include <vector>

#include "iris/bounding_box.h"
#include "iris/emissive_material.h"
#include "iris/geometry.h"
#include "iris/geometry/internal/cubic_bezier_curve.h"
#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/integer.h"
#include "iris/material.h"
#include "iris/matrix.h"
#include "iris/normal_map.h"
#include "iris/point.h"
#include "iris/position_error.h"
#include "iris/ray.h"
#include "iris/reference_countable.h"
#include "iris/sampler.h"
#include "iris/texture_coordinates.h"
#include "iris/vector.h"

namespace iris {
namespace geometry {
namespace {

static const geometric_t kLnToLog4 = 1.0 / std::log(4.0);
static const geometric_t k6Sqrt2 = std::sqrt(36.0 * 2.0);
static const geometric_t kEpsilonFactor = 8.0 * 0.05;

size_t ComputeMaxDepth(const CubicBezierCurve& curve) {
  geometric_t numerator = k6Sqrt2 * curve.ComputeFlatness();
  geometric_t denominator = kEpsilonFactor * curve.MaxWidth();

  geometric_t num_refinement_steps =
      std::log(numerator / denominator) * kLnToLog4;
  num_refinement_steps =
      std::max(static_cast<geometric_t>(0.0), num_refinement_steps);
  num_refinement_steps =
      std::min(static_cast<geometric_t>(10.0), num_refinement_steps);

  return num_refinement_steps + static_cast<geometric_t>(0.5);
}

struct Shared {
  CubicBezierCurve curve;
  ReferenceCounted<Material> materials[1];
  ReferenceCounted<NormalMap> normal_maps[1];
  bool cylinder;
};

struct CurveHit {
  geometric_t distance;
  geometric_t width;
  geometric_t u;
  geometric_t v;
};

std::optional<CurveHit> Trace(const CubicBezierCurve& curve, geometric_t u0,
                              geometric_t u1) {
  if (curve[0].x * (curve[0].x - curve[1].x) -
          curve[0].y * (curve[1].y - curve[0].y) <
      static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  if (curve[3].x * (curve[3].x - curve[2].x) -
          curve[3].y * (curve[2].y - curve[3].y) <
      static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t x_component = curve[0].x - curve[3].x;
  geometric_t y_component = curve[0].y - curve[3].y;

  geometric_t numerator = x_component * curve[0].x + y_component * curve[0].y;
  geometric_t denominator =
      x_component * x_component + y_component * y_component;
  if (denominator == static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t w = numerator / denominator;
  w = std::max(static_cast<geometric_t>(0.0), w);
  w = std::min(static_cast<geometric_t>(1.0), w);

  geometric_t deriv_x, deriv_y;
  auto [on_curve, width] = curve.Evaluate(w, &deriv_x, &deriv_y);

  geometric_t distance_to_on_curve_squared =
      on_curve.x * on_curve.x + on_curve.y * on_curve.y;
  if (distance_to_on_curve_squared >
      width * width * static_cast<geometric_t>(0.25)) {
    return std::nullopt;
  }

  // This could check the maximum distance as well
  if (on_curve.z < static_cast<geometric_t>(0.0)) {
    return std::nullopt;
  }

  geometric_t u = std::lerp(u0, u1, w);

  geometric_t distance_to_on_curve = std::sqrt(distance_to_on_curve_squared);
  geometric_t edge_func = deriv_x * -on_curve.y + on_curve.x * deriv_y;
  geometric_t v =
      (edge_func > static_cast<geometric_t>(0.0))
          ? static_cast<geometric_t>(0.5) + distance_to_on_curve / width
          : static_cast<geometric_t>(0.5) - distance_to_on_curve / width;

  return CurveHit{on_curve.z, static_cast<geometric_t>(2.0) * width, u, v};
}

std::optional<CurveHit> RecursiveTrace(const CubicBezierCurve& curve,
                                       geometric_t u0, geometric_t u1,
                                       int remaining_depth) {
  static const Ray kRay(Point(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));

  if (BoundingBox bounds = curve.ComputeBounds();
      !bounds.Intersects(kRay, static_cast<geometric_t>(0.0),
                         std::numeric_limits<geometric_t>::infinity())) {
    return std::nullopt;
  }

  if (remaining_depth == 0) {
    return Trace(curve, u0, u1);
  }

  geometric_t midpoint = static_cast<geometric_t>(0.5) * (u0 + u1);
  auto [left, right] = curve.Subdivide();

  std::optional<CurveHit> first_hit =
      RecursiveTrace(left, u0, midpoint, remaining_depth - 1);
  std::optional<CurveHit> second_hit =
      RecursiveTrace(right, midpoint, u1, remaining_depth - 1);

  if (!first_hit) {
    return second_hit;
  }

  if (!second_hit) {
    return first_hit;
  }

  return first_hit->distance < second_hit->distance ? first_hit : second_hit;
}

Vector ComputeDpDv(const Ray& ray, const Vector& dp_du, geometric_t width,
                   geometric_t v, bool cylindrical) {
  Vector orthogonal = CrossProduct(ray.direction, dp_du);

  Vector dp_dv = orthogonal * (width / orthogonal.Length());
  if (!cylindrical) {
    return dp_dv;
  }

  geometric_t theta =
      std::lerp(static_cast<geometric_t>(0.5) * std::numbers::pi,
                static_cast<geometric_t>(-0.5) * std::numbers::pi, v);

  return Matrix::Rotation(theta, dp_du.x, dp_du.y, dp_du.z)->Multiply(dp_dv);
}

class Curve final : public Geometry {
 public:
  static constexpr face_t kFrontFace = 0u;

  struct AdditionalData {
    Vector direction;
    Vector dp_du;
    Vector dp_dv;
    geometric_t error;
    geometric_t u;
    geometric_t v;
  };

  Curve(std::shared_ptr<Shared> shared, geometric u0, geometric u1)
      : shared_(std::move(shared)), u0_(u0), u1_(u1) {}

  Vector ComputeSurfaceNormal(const Point& hit_point, face_t face,
                              const void* additional_data) const override;

  std::optional<TextureCoordinates> ComputeTextureCoordinates(
      const Point& hit_point, const std::optional<Differentials>& differentials,
      face_t face, const void* additional_data) const override;

  Geometry::ComputeShadingNormalResult ComputeShadingNormal(
      face_t face, const void* additional_data) const override;

  ComputeHitPointResult ComputeHitPoint(
      const Ray& ray, geometric_t distance,
      const void* additional_data) const override;

  const Material* GetMaterial(face_t face) const override;

  BoundingBox ComputeBounds(const Matrix* model_to_world) const override;

 private:
  Hit* Trace(const Ray& ray, geometric_t minimum_distance,
             geometric_t maximum_distance, TraceMode trace_mode,
             HitAllocator& hit_allocator) const override;

  std::shared_ptr<Shared> shared_;
  geometric u0_;
  geometric u1_;
};

Hit* Curve::Trace(const Ray& ray, geometric_t minimum_distance,
                  geometric_t maximum_distance, TraceMode trace_mode,
                  HitAllocator& hit_allocator) const {
  CubicBezierCurve model_curve = shared_->curve.ExtractSegment(u0_, u1_);

  Vector maybe_dx =
      CrossProduct(ray.direction, model_curve[3] - model_curve[0]);
  Vector up =
      maybe_dx.IsZero() ? CoordinateSystem(ray.direction).first : maybe_dx;

  Point look_at = ray.origin + ray.direction;
  std::expected<Matrix, const char*> transform =
      Matrix::LookAt(ray.origin.x, ray.origin.y, ray.origin.z, look_at.x,
                     look_at.y, look_at.z, up.x, up.y, up.z);
  if (!transform) {
    return nullptr;
  }

  CubicBezierCurve local_curve = model_curve.InverseTransform(*transform);

  std::optional<CurveHit> hit =
      RecursiveTrace(local_curve, u0_, u1_, ComputeMaxDepth(local_curve));
  if (!hit) {
    return nullptr;
  }

  Vector maybe_dp_du = shared_->curve.EvaluateDerivative(hit->u);
  Vector dp_du = maybe_dp_du.IsZero() ? shared_->curve.Diagonal() : maybe_dp_du;
  Vector dp_dv = ComputeDpDv(ray, dp_du, hit->width, hit->v, shared_->cylinder);

  return &hit_allocator.Allocate(
      nullptr, hit->distance, hit->width, kFrontFace, kFrontFace,
      /*is_chiral=*/false,
      AdditionalData{ray.direction, dp_du, dp_dv, hit->width, hit->u, hit->v});
}

Vector Curve::ComputeSurfaceNormal(const Point& hit_point, face_t face,
                                   const void* additional_data) const {
  const AdditionalData* data =
      static_cast<const AdditionalData*>(additional_data);
  return CrossProduct(data->dp_du, data->dp_dv).AlignAgainst(data->direction);
}

std::optional<Geometry::TextureCoordinates> Curve::ComputeTextureCoordinates(
    const Point& hit_point, const std::optional<Differentials>& differentials,
    face_t face, const void* additional_data) const {
  const AdditionalData* data =
      static_cast<const AdditionalData*>(additional_data);
  if (!differentials) {
    return Geometry::TextureCoordinates{face, {data->u, data->v}};
  }

  Vector dp_dx = differentials->dx - hit_point;
  Vector dp_dy = differentials->dy - hit_point;

  geometric_t dp_du_length_squared = DotProduct(data->dp_du, data->dp_du);
  geometric_t dp_dv_length_squared = DotProduct(data->dp_dv, data->dp_dv);

  return Geometry::TextureCoordinates{
      face,
      {data->u, data->v},
      /*du_dx=*/DotProduct(data->dp_du, dp_dx) / dp_du_length_squared,
      /*du_dy=*/DotProduct(data->dp_du, dp_dy) / dp_du_length_squared,
      /*dv_dx=*/DotProduct(data->dp_dv, dp_dx) / dp_dv_length_squared,
      /*dv_dy=*/DotProduct(data->dp_dv, dp_dy) / dp_dv_length_squared};
}

Geometry::ComputeShadingNormalResult Curve::ComputeShadingNormal(
    face_t face, const void* additional_data) const {
  const AdditionalData* data =
      static_cast<const AdditionalData*>(additional_data);
  return {std::nullopt, std::make_pair(data->dp_du, data->dp_dv),
          shared_->normal_maps[face].Get()};
}

Geometry::ComputeHitPointResult Curve::ComputeHitPoint(
    const Ray& ray, geometric_t distance, const void* additional_data) const {
  const AdditionalData* data =
      static_cast<const AdditionalData*>(additional_data);
  return ComputeHitPointResult{
      ray.Endpoint(distance),
      PositionError(data->error, data->error, data->error)};
}

const Material* Curve::GetMaterial(face_t face) const {
  return shared_->materials[face].Get();
}

BoundingBox Curve::ComputeBounds(const Matrix* model_to_world) const {
  return shared_->curve.ComputeBounds(model_to_world);
}

std::vector<ReferenceCounted<Geometry>> MakeCubicBezierCurve(
    const std::array<Point, 4>& control_points, uint32_t num_segments,
    geometric start_width, geometric end_width, bool cylinder,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<NormalMap> front_normal_map) {
  std::vector<ReferenceCounted<Geometry>> result;
  if (control_points[0] == control_points[1] &&
      control_points[1] == control_points[2] &&
      control_points[2] == control_points[3]) {
    return result;
  }

  start_width = std::max(static_cast<geometric>(0.0), start_width);
  end_width = std::max(static_cast<geometric>(0.0), end_width);

  if (start_width == static_cast<geometric>(0.0) &&
      end_width == static_cast<geometric>(0.0)) {
    return result;
  }

  std::shared_ptr<Shared> shared = std::make_shared<Shared>(
      Shared{CubicBezierCurve(control_points.data(), start_width, end_width),
             std::move(front_material), std::move(front_normal_map), cylinder});

  for (uint64_t i = 0; i < num_segments; i++) {
    geometric start =
        static_cast<double>(i) / static_cast<double>(num_segments);
    geometric end =
        static_cast<double>(i + 1) / static_cast<double>(num_segments);
    result.push_back(MakeReferenceCounted<Curve>(
        shared, static_cast<geometric>(start), static_cast<geometric>(end)));
  }

  return result;
}

}  // namespace

std::vector<ReferenceCounted<Geometry>> MakeFlatCubicBezierCurve(
    const std::array<Point, 4>& control_points, uint32_t num_segments,
    geometric start_width, geometric end_width,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<NormalMap> front_normal_map) {
  return MakeCubicBezierCurve(
      control_points, num_segments, start_width, end_width, /*cylinder=*/false,
      std::move(front_material), std::move(front_normal_map));
}

std::vector<ReferenceCounted<Geometry>> MakeCylindricalCubicBezierCurve(
    const std::array<Point, 4>& control_points, uint32_t num_segments,
    geometric start_width, geometric end_width,
    ReferenceCounted<Material> front_material,
    ReferenceCounted<NormalMap> front_normal_map) {
  return MakeCubicBezierCurve(
      control_points, num_segments, start_width, end_width, /*cylinder=*/true,
      std::move(front_material), std::move(front_normal_map));
}

}  // namespace geometry
}  // namespace iris
