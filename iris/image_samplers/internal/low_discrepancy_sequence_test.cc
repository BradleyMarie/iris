#include "iris/image_samplers/internal/low_discrepancy_sequence.h"

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "iris/image_samplers/internal/mock_low_discrepancy_sequence.h"

namespace iris {
namespace image_samplers {
namespace internal {
namespace {

using ::testing::Invoke;
using ::testing::Return;

TEST(LowDiscrepancySequenceTest, NextIndex) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Next()).WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_EQ(2u, sequence.NextIndex(4));
  EXPECT_CALL(sequence, Next()).WillOnce(Return(static_cast<visual_t>(1.0)));
  EXPECT_EQ(3u, sequence.NextIndex(4));
}

TEST(LowDiscrepancySequenceTest, DiscardIndex) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Discard(2));
  sequence.DiscardIndex(2);
}

TEST(LowDiscrepancySequenceTest, NextGeometric) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Next()).WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_EQ(0.5, sequence.NextGeometric());
}

TEST(LowDiscrepancySequenceTest, DiscardGeometric) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Discard(2));
  sequence.DiscardGeometric(2);
}

TEST(LowDiscrepancySequenceTest, NextVisual) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Next()).WillOnce(Return(static_cast<visual_t>(0.5)));
  EXPECT_EQ(0.5, sequence.NextVisual());
}

TEST(LowDiscrepancySequenceTest, DiscardVisual) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Discard(2));
  sequence.DiscardVisual(2);
}

TEST(LowDiscrepancySequenceTest, Replicate) {
  MockLowDiscrepancySequence sequence;
  EXPECT_CALL(sequence, Duplicate()).WillOnce(Invoke([]() { return nullptr; }));
  EXPECT_FALSE(sequence.Replicate());
}

}  // namespace
}  // namespace internal
}  // namespace image_samplers
}  // namespace iris
