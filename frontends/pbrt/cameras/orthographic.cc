#include "frontends/pbrt/cameras/orthographic.h"

#include "iris/cameras/orthographic_camera.h"

namespace iris::pbrt_frontend::cameras {
namespace {

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"focaldistance", Parameter::FLOAT},
        {"frameaspectratio", Parameter::FLOAT},
        {"lensradius", Parameter::FLOAT},
};

class OrthographicObjectBuilder
    : public ObjectBuilder<std::function<std::unique_ptr<Camera>(
                               const std::pair<size_t, size_t>&)>,
                           const MatrixManager::Transformation&> {
 public:
  OrthographicObjectBuilder() : ObjectBuilder(g_parameters) {}

  std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        const MatrixManager::Transformation& transformation) const override;
};

std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>
OrthographicObjectBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    const MatrixManager::Transformation& transformation) const {
  std::optional<geometric_t> aspect_ratio;

  auto frameaspectratio = parameters.find("frameaspectratio");
  if (frameaspectratio != parameters.end()) {
    auto value = frameaspectratio->second.GetFloatValues(1).front();
    if (value <= 0.0) {
      std::cerr << "ERROR: Out of range value for parameter: frameaspectratio"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    aspect_ratio = value;
  }

  return [aspect_ratio,
          transformation](const std::pair<size_t, size_t>& image_dimensions) {
    geometric_t actual_aspect_ratio = aspect_ratio.value_or(
        static_cast<intermediate_t>(image_dimensions.second) /
        static_cast<intermediate_t>(image_dimensions.first));

    std::array<geometric_t, 4> frame_bounds;
    if (actual_aspect_ratio < 1.0) {
      actual_aspect_ratio = 1.0 / actual_aspect_ratio;
      frame_bounds[0] = -1.0;
      frame_bounds[1] = -actual_aspect_ratio;
      frame_bounds[2] = 1.0;
      frame_bounds[3] = actual_aspect_ratio;
    } else {
      frame_bounds[0] = -actual_aspect_ratio;
      frame_bounds[1] = -1.0;
      frame_bounds[2] = actual_aspect_ratio;
      frame_bounds[3] = 1.0;
    }

    return std::make_unique<iris::cameras::OrthographicCamera>(
        transformation.start, frame_bounds);
  };
}

}  // namespace

const std::unique_ptr<const ObjectBuilder<
    std::function<std::unique_ptr<Camera>(const std::pair<size_t, size_t>&)>,
    const MatrixManager::Transformation&>>
    g_orthographic_builder = std::make_unique<OrthographicObjectBuilder>();

}  // namespace iris::pbrt_frontend::cameras