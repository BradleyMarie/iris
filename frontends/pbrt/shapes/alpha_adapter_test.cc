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

TEST(AlphaAdapter, Nullptr) {
  EXPECT_FALSE(MakeAlphaAdapter(ReferenceCounted<ValueTexture2D<visual>>()));
}

TEST(AlphaAdapter, Hits) {
  EXPECT_TRUE(MakeAlphaAdapter(MakeReferenceCounted<AlphaHits>())
                  ->Evaluate({0.0, 0.0}));
}

TEST(AlphaAdapter, Misses) {
  EXPECT_FALSE(MakeAlphaAdapter(MakeReferenceCounted<AlphaMisses>())
                   ->Evaluate({0.0, 0.0}));
}

}  // namespace
}  // namespace shapes
}  // namespace pbrt_frontend
}  // namespace iris