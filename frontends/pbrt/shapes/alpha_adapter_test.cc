#include "frontends/pbrt/shapes/alpha_adapter.h"

#include "googletest/include/gtest/gtest.h"
#include "iris/reference_counted.h"

class AlphaHits : public iris::textures::ValueTexture2D<iris::visual> {
 public:
  iris::visual Evaluate(const iris::TextureCoordinates& coords) const override {
    return 0.001;
  }
};

class AlphaMisses : public iris::textures::ValueTexture2D<iris::visual> {
 public:
  iris::visual Evaluate(const iris::TextureCoordinates& coords) const override {
    return 0.0;
  }
};

TEST(AlphaAdapter, Hits) {
  iris::pbrt_frontend::shapes::internal::AlphaAdapter alpha(
      iris::MakeReferenceCounted<AlphaHits>());
  EXPECT_TRUE(alpha.Evaluate({0.0, 0.0}));
}

TEST(AlphaAdapter, Misses) {
  iris::pbrt_frontend::shapes::internal::AlphaAdapter alpha(
      iris::MakeReferenceCounted<AlphaMisses>());
  EXPECT_FALSE(alpha.Evaluate({0.0, 0.0}));
}