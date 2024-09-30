#define _USE_MATH_DEFINES
#include "frontends/pbrt/cameras/perspective.h"

#include <optional>

#include "iris/cameras/pinhole_camera.h"
#include "iris/cameras/thin_lens_camera.h"

namespace iris {
namespace pbrt_frontend {
namespace cameras {
namespace {

constexpr geometric_t kDefaultFocalDistance = 1000000.0;
constexpr geometric_t kDefaultLensRadius = 0.0;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"focaldistance", Parameter::FLOAT},
        {"fov", Parameter::FLOAT},
        {"frameaspectratio", Parameter::FLOAT},
        {"halffov", Parameter::FLOAT},
        {"lensradius", Parameter::FLOAT},
        {"shutteropen", Parameter::FLOAT},
        {"shutterclose", Parameter::FLOAT},
};

class PerspectiveObjectBuilder
    : public ObjectBuilder<std::function<std::unique_ptr<Camera>(
                               const std::pair<size_t, size_t>&)>,
                           const MatrixManager::Transformation&> {
 public:
  PerspectiveObjectBuilder() : ObjectBuilder(g_parameters) {}

  std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        const MatrixManager::Transformation& transformation) const override;
};

std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
PerspectiveObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MatrixManager::Transformation& transformation) const {
  std::optional<geometric_t> aspect_ratio;
  geometric_t focus_distance = kDefaultFocalDistance;
  std::optional<geometric_t> fov;
  std::optional<geometric_t> half_fov;
  geometric_t lens_radius = kDefaultLensRadius;
  std::optional<std::array<geometric_t, 4>> screen_window;

  auto focaldistance = parameters.find("focaldistance");
  if (focaldistance != parameters.end()) {
    auto value = focaldistance->second.GetFloatValues(1).front();
    if (value <= 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: focaldistance"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    focus_distance = static_cast<geometric_t>(value);
  }

  auto fullfov = parameters.find("fov");
  if (fullfov != parameters.end()) {
    auto value = fullfov->second.GetFloatValues(1).front();
    if (value <= 0.0 || value >= 180.0) {
      std::cerr << "ERROR: Out of range value for parameter: fov" << std::endl;
      exit(EXIT_FAILURE);
    }

    fov = static_cast<geometric_t>(value);
  }

  auto frameaspectratio = parameters.find("frameaspectratio");
  if (frameaspectratio != parameters.end()) {
    auto value = frameaspectratio->second.GetFloatValues(1).front();
    if (value <= 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: frameaspectratio"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    aspect_ratio = static_cast<geometric_t>(value);
  }

  auto halffov = parameters.find("halffov");
  if (halffov != parameters.end()) {
    auto value = halffov->second.GetFloatValues(1).front();
    if (value <= 0.0 || value >= 90.0) {
      std::cerr << "ERROR: Out of range value for parameter: halffov"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    half_fov = static_cast<geometric_t>(value);
  }

  auto lensradius = parameters.find("lensradius");
  if (lensradius != parameters.end()) {
    auto value = lensradius->second.GetFloatValues(1).front();
    if (value < 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: lensradius"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    lens_radius = static_cast<geometric_t>(value);
  }

  if (fov && half_fov) {
    std::cerr << "ERROR: Cannot specify parameters together: fov, halffov"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (fov) {
    half_fov = static_cast<geometric_t>(fov.value() * 0.5);
  }

  if (half_fov) {
    *half_fov *= static_cast<geometric_t>(M_PI / 180.0);
  } else {
    half_fov = static_cast<geometric_t>(M_PI_4);
  }

  return [aspect_ratio, half_fov, focus_distance, lens_radius, screen_window,
          transformation](const std::pair<size_t, size_t>& image_dimensions)
             -> std::unique_ptr<Camera> {
    geometric_t actual_aspect_ratio = aspect_ratio.value_or(
        static_cast<intermediate_t>(image_dimensions.second) /
        static_cast<intermediate_t>(image_dimensions.first));

    std::array<geometric_t, 2> half_frame_size;
    if (actual_aspect_ratio > 1.0) {
      half_frame_size[0] = actual_aspect_ratio;
      half_frame_size[1] = 1.0;
    } else {
      half_frame_size[0] = 1.0;
      half_frame_size[1] = 1.0 / actual_aspect_ratio;
    }

    if (lens_radius == kDefaultLensRadius) {
      return std::make_unique<iris::cameras::PinholeCamera>(
          transformation.start, half_frame_size, half_fov.value());
    }

    return std::make_unique<iris::cameras::ThinLensCamera>(
        transformation.start, half_frame_size, half_fov.value(), lens_radius,
        focus_distance);
  };
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>>
    g_perspective_builder = std::make_unique<PerspectiveObjectBuilder>();

}  // namespace cameras
}  // namespace pbrt_frontend
}  // namespace iris