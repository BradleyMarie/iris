#include "iris/image_samplers/internal/halton_sequence.h"

#include <cstdint>
#include <optional>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

TEST(HaltonSequenceTest, StartLimits) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({1, 59049u}, {0, 0}, 0));
  EXPECT_TRUE(sequence.Start({59049u, 1}, {0, 0}, 0));
  EXPECT_TRUE(sequence.Start({59049u, 59049u}, {0, 0}, 0));
}

TEST(HaltonSequenceTest, StartSampleLimit) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 138083));
  EXPECT_FALSE(sequence.Start({255, 255}, {0, 0}, 138084));
}

TEST(HaltonSequenceTest, Samples) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({2, 2}, {0, 0}, 0));
  for (uint32_t i = 0; i < 4; i++) {
    std::optional<geometric_t> value = sequence.Next();

    ASSERT_TRUE(value);
    if (i < 2) {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 0.5 + 0.0001);
    } else {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 1.0);
    }
  }

  EXPECT_TRUE(sequence.Start({2, 2}, {0, 0}, 1));
  for (uint32_t i = 0; i < 4; i++) {
    std::optional<geometric_t> value = sequence.Next();

    ASSERT_TRUE(value);
    if (i < 2) {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 0.5 + 0.0001);
    } else {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 1.0);
    }
  }

  EXPECT_TRUE(sequence.Start({2, 2}, {0, 1}, 1));
  for (uint32_t i = 0; i < 4; i++) {
    std::optional<geometric_t> value = sequence.Next();

    ASSERT_TRUE(value);
    if (i == 0) {
      EXPECT_GE(*value, 0.5);
      EXPECT_LE(*value, 1.0);
    } else if (i == 1) {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 0.5 + 0.0001);
    } else {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 1.0);
    }
  }

  EXPECT_TRUE(sequence.Start({2, 2}, {1, 0}, 1));
  for (uint32_t i = 0; i < 4; i++) {
    std::optional<geometric_t> value = sequence.Next();

    ASSERT_TRUE(value);
    if (i == 0) {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 0.5 + 0.0001);
    } else if (i == 1) {
      EXPECT_GE(*value, 0.5);
      EXPECT_LE(*value, 1.0);
    } else {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 1.0);
    }
  }

  EXPECT_TRUE(sequence.Start({2, 2}, {1, 1}, 0));
  for (uint32_t i = 0; i < 4; i++) {
    std::optional<geometric_t> value = sequence.Next();

    ASSERT_TRUE(value);
    if (i < 2) {
      EXPECT_GE(*value, 0.5 - 0.0001);
      EXPECT_LE(*value, 1.0);
    } else {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 1.0);
    }
  }

  EXPECT_TRUE(sequence.Start({2, 2}, {1, 1}, 1));
  for (uint32_t i = 0; i < 4; i++) {
    std::optional<geometric_t> value = sequence.Next();

    ASSERT_TRUE(value);
    if (i < 2) {
      EXPECT_GE(*value, 0.5 - 0.0001);
      EXPECT_LE(*value, 1.0);
    } else {
      EXPECT_GE(*value, 0.0);
      EXPECT_LE(*value, 1.0);
    }
  }
}

TEST(HaltonSequenceTest, SamplesLimit) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  for (uint32_t i = 0; i < 256; i++) {
    std::optional<geometric_t> value = sequence.Next();
    ASSERT_TRUE(value);
    EXPECT_GE(*value, 0.0);
    EXPECT_LE(*value, 1.0);
  }
  EXPECT_FALSE(sequence.Next());
}

TEST(HaltonSequenceTest, Discard) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  for (uint32_t i = 0; i < 256; i++) {
    sequence.Discard(1);
  }
  EXPECT_FALSE(sequence.Next());
}

TEST(HaltonSequenceTest, DiscardMultiple) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  sequence.Discard(300);
  EXPECT_FALSE(sequence.Next());
}

TEST(HaltonSequenceTest, SampleWeight) {
  HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  EXPECT_EQ(1.0, sequence.SampleWeight(1));
  EXPECT_EQ(0.5, sequence.SampleWeight(2));
  EXPECT_NEAR(1.0 / 23014.0, sequence.SampleWeight(23014), 0.000001);
  EXPECT_NEAR(1.0 / 23014.0, sequence.SampleWeight(23015), 0.000001);
}

}  // namespace
}  // namespace internal
}  // namespace image_samplers
}  // namespace iris
