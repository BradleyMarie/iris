#ifndef _FRONTENDS_PBRT_MATERIALS_FOURIER_
#define _FRONTENDS_PBRT_MATERIALS_FOURIER_

#include <filesystem>

#include "frontends/pbrt/materials/result.h"
#include "frontends/pbrt/spectrum_manager.h"
#include "frontends/pbrt/texture_manager.h"
#include "pbrt_proto/v3/v3.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

MaterialResult MakeFourier(
    const pbrt_proto::v3::Material::Fourier& glass,
    const pbrt_proto::v3::Shape::MaterialOverrides& overrides,
    const std::filesystem::path& search_root, TextureManager& texture_manager,
    SpectrumManager& spectrum_manager);

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_FOURIER_
