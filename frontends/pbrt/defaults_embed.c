#include "frontends/pbrt/defaults_embed.h"

const char kDefaults[] = {
#embed "../../external/pbrt_proto+/pbrt_proto/v3/defaults.txtpb"
};

const size_t kDefaultsSize = sizeof(kDefaults);
