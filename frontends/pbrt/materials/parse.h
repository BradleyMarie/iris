#ifndef _FRONTENDS_PBRT_MATERIALS_PARSE_
#define _FRONTENDS_PBRT_MATERIALS_PARSE_

#include "frontends/pbrt/materials/material_builder.h"

namespace iris {
namespace pbrt_frontend {
namespace materials {

const MaterialBuilder& Parse(Tokenizer& tokenizer);

void ParseNamed(Tokenizer& tokenizer, const std::filesystem::path& search_root,
                MaterialManager& material_manager,
                SpectrumManager& spectrum_manager,
                TextureManager& texture_manager);

const MaterialBuilder& Default();

}  // namespace materials
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATERIALS_PARSE_