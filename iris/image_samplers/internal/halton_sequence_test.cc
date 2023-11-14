#include "iris/image_samplers/internal/halton_sequence.h"

#include "googletest/include/gtest/gtest.h"

TEST(HaltonSequenceTest, StartLimits) {
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({1, 59049u}, {0, 0}, 0));
  EXPECT_TRUE(sequence.Start({59049u, 1}, {0, 0}, 0));
  EXPECT_TRUE(sequence.Start({59049u, 59049u}, {0, 0}, 0));
}

TEST(HaltonSequenceTest, StartSampleLimit) {
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 138083));
  EXPECT_FALSE(sequence.Start({255, 255}, {0, 0}, 138084));
}

TEST(HaltonSequenceTest, Samples) {
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({2, 2}, {0, 0}, 0));
  for (uint32_t i = 0; i < 4; i++) {
    auto value = sequence.Next();

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
    auto value = sequence.Next();

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
    auto value = sequence.Next();

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
    auto value = sequence.Next();

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
    auto value = sequence.Next();

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
    auto value = sequence.Next();

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
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  for (uint32_t i = 0; i < 256; i++) {
    auto value = sequence.Next();
    ASSERT_TRUE(value);
    EXPECT_GE(*value, 0.0);
    EXPECT_LE(*value, 1.0);
  }
  EXPECT_FALSE(sequence.Next());
}

TEST(HaltonSequenceTest, Discard) {
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  for (uint32_t i = 0; i < 256; i++) {
    sequence.Discard(1);
  }
  EXPECT_FALSE(sequence.Next());
}

TEST(HaltonSequenceTest, DiscardMultiple) {
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  sequence.Discard(300);
  EXPECT_FALSE(sequence.Next());
}

TEST(HaltonSequenceTest, SampleWeight) {
  iris::image_samplers::internal::HaltonSequence sequence;
  EXPECT_TRUE(sequence.Start({255, 255}, {0, 0}, 0));
  EXPECT_EQ(1.0, sequence.SampleWeight(1));
  EXPECT_EQ(0.5, sequence.SampleWeight(2));
  EXPECT_NEAR(1.0 / 23014.0, sequence.SampleWeight(23014), 0.000001);
  EXPECT_NEAR(1.0 / 23014.0, sequence.SampleWeight(23015), 0.000001);
}