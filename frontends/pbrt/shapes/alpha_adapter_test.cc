#include "frontends/pbrt/shapes/alpha_adapter.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"
#include "iris/textures/texture2d.h"

namespace iris {
namespace pbrt_frontend {
namespace shapes {
namespace {

using ::iris::textures::ValueTexture2D;

class AlphaHits : public ValueTexture2D<visual> {
 public:
  visual Evaluate(const TextureCoordinates& coords) const override {
    return 0.001;
  }
};

class AlphaMisses : public ValueTexture2D<visual> {
 public:
  visual Evaluate(const TextureCoordinates& coords) const override {
    return 0.0;
  }
};

TEST(AlphaAdapter, Hits) {
  AlphaAdapter alpha(MakeReferenceCounted<AlphaHits>());
  EXPECT_TRUE(alpha.Evaluate({0.0, 0.0}));
}

TEST(AlphaAdapter, Misses) {
  AlphaAdapter alpha(MakeReferenceCounted<AlphaMisses>());
  EXPECT_FALSE(alpha.Evaluate({0.0, 0.0}));
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris