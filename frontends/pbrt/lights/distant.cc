#include "frontends/pbrt/lights/distant.h"

#include "iris/lights/directional_light.h"

namespace iris {
namespace pbrt_frontend {
namespace lights {
namespace {

using ::iris::lights::DirectionalLight;

static const std::unordered_map<std::string_view, Parameter::Type>
    g_parameters = {
        {"from", Parameter::POINT3},
        {"L", Parameter::SPECTRUM},
        {"scale", Parameter::SPECTRUM},
        {"to", Parameter::POINT3},
};

static const Point kDefaultFrom(static_cast<geometric>(0.0),
                                static_cast<geometric>(0.0),
                                static_cast<geometric>(0.0));
static const Color kDefaultColor({static_cast<visual_t>(1.0),
                                  static_cast<visual_t>(1.0),
                                  static_cast<visual_t>(1.0)},
                                 Color::Space::RGB);
static const Point kDefaultTo(static_cast<geometric>(0.0),
                              static_cast<geometric>(0.0),
                              static_cast<geometric>(1.0));

class DistantBuilder
    : public ObjectBuilder<std::variant<ReferenceCounted<Light>,
                                        ReferenceCounted<EnvironmentalLight>>,
                           SpectrumManager&, const Matrix&> {
 public:
  DistantBuilder() : ObjectBuilder(g_parameters) {}

  std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
  Build(const std::unordered_map<std::string_view, Parameter>& parameters,
        SpectrumManager& spectrum_manager,
        const Matrix& model_to_world) const override;
};

std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>
DistantBuilder::Build(
    const std::unordered_map<std::string_view, Parameter>& parameters,
    SpectrumManager& spectrum_manager, const Matrix& model_to_world) const {
  std::optional<Point> origin;
  ReferenceCounted<Spectrum> spectrum = {
      spectrum_manager.AllocateSpectrum(kDefaultColor)};
  ReferenceCounted<Spectrum> scalar = {
      spectrum_manager.AllocateSpectrum(kDefaultColor)};
  std::optional<Point> destination;

  auto from = parameters.find("from");
  if (from != parameters.end()) {
    origin.emplace(from->second.GetPoint3Values(1).front());
  }

  auto l = parameters.find("L");
  if (l != parameters.end()) {
    spectrum = l->second.GetSpectra(1).front();
  }

  auto scale = parameters.find("scale");
  if (scale != parameters.end()) {
    scalar = scale->second.GetSpectra(1).front();
  }

  auto to = parameters.find("to");
  if (to != parameters.end()) {
    destination.emplace(to->second.GetPoint3Values(1).front());
  }

  Point world_origin = model_to_world.Multiply(origin.value_or(kDefaultFrom));
  Point world_destination =
      model_to_world.Multiply(destination.value_or(kDefaultTo));

  if (world_origin == world_destination) {
    std::cerr << "ERROR: from and to must not be the same point" << std::endl;
    exit(EXIT_FAILURE);
  }

  ReferenceCounted<Spectrum> scaled =
      spectrum_manager.AllocateSpectrum(spectrum, scalar);
  if (!scaled) {
    return ReferenceCounted<Light>();
  }

  return MakeReferenceCounted<DirectionalLight>(
      world_origin - world_destination, std::move(scaled));
}

};  // namespace

extern const std::unique_ptr<const ObjectBuilder<
    std::variant<ReferenceCounted<Light>, ReferenceCounted<EnvironmentalLight>>,
    SpectrumManager&, const Matrix&>>
    g_distant_builder = std::make_unique<DistantBuilder>();

}  // namespace lights
}  // namespace pbrt_frontend
}  // namespace iris