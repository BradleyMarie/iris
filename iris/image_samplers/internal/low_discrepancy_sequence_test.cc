#include "iris/image_samplers/internal/low_discrepancy_sequence.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_samplers/internal/mock_low_discrepancy_sequence.h"

TEST(LowDiscrepancySequenceTest, NextIndex) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Next())
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_EQ(2u, sequence.NextIndex(4));
  EXPECT_CALL(sequence, Next())
      .WillOnce(testing::Return(static_cast<iris::visual_t>(1.0)));
  EXPECT_EQ(3u, sequence.NextIndex(4));
}

TEST(LowDiscrepancySequenceTest, DiscardIndex) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Discard(2));
  sequence.DiscardIndex(2);
}

TEST(LowDiscrepancySequenceTest, NextGeometric) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Next())
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_EQ(0.5, sequence.NextGeometric());
}

TEST(LowDiscrepancySequenceTest, DiscardGeometric) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Discard(2));
  sequence.DiscardGeometric(2);
}

TEST(LowDiscrepancySequenceTest, NextVisual) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Next())
      .WillOnce(testing::Return(static_cast<iris::visual_t>(0.5)));
  EXPECT_EQ(0.5, sequence.NextVisual());
}

TEST(LowDiscrepancySequenceTest, DiscardVisual) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Discard(2));
  sequence.DiscardVisual(2);
}

TEST(LowDiscrepancySequenceTest, Replicate) {
  iris::image_samplers::internal::MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Duplicate()).WillOnce(testing::Invoke([]() {
    return nullptr;
  }));
  EXPECT_FALSE(sequence.Replicate());
}