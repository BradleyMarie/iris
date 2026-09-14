#include "iris/sampler.h"

#include <utility>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "iris/float.h"
#include "iris/random/mock_random.h"
#include "iris/random_bitstreams/mock_random_bitstream.h"

namespace iris {
namespace {

using ::iris::random::MockRandom;
using ::iris::random_bitstreams::MockRandomBitstream;
using ::testing::InSequence;
using ::testing::Return;

TEST(Clone, CloneRoot) {
  MockRandomBitstream rng_bitstream;
  {
    InSequence sequence;
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x00000000u));
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x7FFFFFFFu));
  }

  MockRandom random;
  {
    InSequence sequence;
    EXPECT_CALL(random, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.75));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, &random, visual_storage, geometric_storage);
  Sampler clone = sampler.Clone();

  EXPECT_EQ(0.00, sampler.NextLinear1D());
  EXPECT_EQ(0.50, sampler.NextLinear1D());
  auto [u0, u1] = sampler.NextLinear2D();
  EXPECT_EQ(0.25, u0);
  EXPECT_EQ(0.75, u1);

  EXPECT_EQ(0.00, clone.NextLinear1D());
  EXPECT_EQ(0.50, clone.NextLinear1D());
  auto [clone_u0, clone_u1] = clone.NextLinear2D();
  EXPECT_EQ(0.25, clone_u0);
  EXPECT_EQ(0.75, clone_u1);
}

TEST(Clone, CloneChild) {
  MockRandomBitstream rng_bitstream;
  {
    InSequence sequence;
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x00000000u));
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x7FFFFFFFu));
  }

  MockRandom random;
  {
    InSequence sequence;
    EXPECT_CALL(random, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.75));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, &random, visual_storage, geometric_storage);
  Sampler child = sampler.Claim(2u, 1u);
  Sampler clone = child.Clone();

  EXPECT_EQ(0.00, child.NextLinear1D());
  EXPECT_EQ(0.50, child.NextLinear1D());
  auto [u0, u1] = child.NextLinear2D();
  EXPECT_EQ(0.25, u0);
  EXPECT_EQ(0.75, u1);

  EXPECT_EQ(0.00, clone.NextLinear1D());
  EXPECT_EQ(0.50, clone.NextLinear1D());
  auto [clone_u0, clone_u1] = clone.NextLinear2D();
  EXPECT_EQ(0.25, clone_u0);
  EXPECT_EQ(0.75, clone_u1);
}

TEST(Clone, ClaimRoot) {
  MockRandomBitstream rng_bitstream;
  {
    InSequence sequence;
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x00000000u));
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x7FFFFFFFu));
  }

  MockRandom random;
  {
    InSequence sequence;
    EXPECT_CALL(random, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.75));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.125));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.375));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, &random, visual_storage, geometric_storage);
  Sampler child = sampler.Claim(1u, 1u);

  EXPECT_EQ(0.00, child.NextLinear1D());
  auto [child_u0, child_u1] = child.NextLinear2D();
  EXPECT_EQ(0.25, child_u0);
  EXPECT_EQ(0.75, child_u1);

  EXPECT_EQ(0.50, sampler.NextLinear1D());
  auto [u0, u1] = sampler.NextLinear2D();
  EXPECT_EQ(0.125, u0);
  EXPECT_EQ(0.375, u1);
}

TEST(Clone, ClaimChild) {
  MockRandomBitstream rng_bitstream;
  {
    InSequence sequence;
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x00000000u));
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x7FFFFFFFu));
  }

  MockRandom random;
  {
    InSequence sequence;
    EXPECT_CALL(random, Next()).WillOnce(Return(0.25));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.75));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.125));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.375));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, &random, visual_storage, geometric_storage);
  Sampler child = sampler.Claim(1u, 1u);
  Sampler subchild = child.Claim(1u, 1u);

  EXPECT_EQ(0.00, subchild.NextLinear1D());
  auto [subchild_u0, subchild_u1] = subchild.NextLinear2D();
  EXPECT_EQ(0.25, subchild_u0);
  EXPECT_EQ(0.75, subchild_u1);

  EXPECT_EQ(0.50, sampler.NextLinear1D());
  auto [u0, u1] = sampler.NextLinear2D();
  EXPECT_EQ(0.125, u0);
  EXPECT_EQ(0.375, u1);
}

TEST(NextLinear1D, Generates) {
  MockRandomBitstream rng_bitstream;
  {
    InSequence sequence;
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x00000000u));
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x7FFFFFFFu));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, nullptr, visual_storage, geometric_storage);

  EXPECT_EQ(0.00, sampler.NextLinear1D());
  EXPECT_EQ(0.50, sampler.NextLinear1D());
}

TEST(NextLinear1D, ConsumesExisting) {
  MockRandomBitstream rng_bitstream;
  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, nullptr, visual_storage, geometric_storage);

  visual_storage.push_back(0.25);
  visual_storage.push_back(0.50);
  EXPECT_EQ(0.25, sampler.NextLinear1D());
  EXPECT_EQ(0.50, sampler.NextLinear1D());
}

TEST(NextIndex1D, ConsumesExisting) {
  MockRandomBitstream rng_bitstream;
  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, nullptr, visual_storage, geometric_storage);

  visual_storage.push_back(0.25);
  EXPECT_EQ(0u, sampler.NextIndex1D(2));
}

TEST(NextLinear2D, GeneratesFromBitstream) {
  MockRandomBitstream rng_bitstream;
  {
    InSequence sequence;
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x00000000u));
    EXPECT_CALL(rng_bitstream, Next()).WillOnce(Return(0x7FFFFFFFu));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, nullptr, visual_storage, geometric_storage);

  auto [u0, u1] = sampler.NextLinear2D();
  EXPECT_EQ(0.00, u0);
  EXPECT_EQ(0.50, u1);
}

TEST(NextLinear2D, GeneratesFromRandom) {
  MockRandomBitstream rng_bitstream;
  MockRandom random;
  {
    InSequence sequence;
    EXPECT_CALL(random, Next()).WillOnce(Return(0.0));
    EXPECT_CALL(random, Next()).WillOnce(Return(0.5));
  }

  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, &random, visual_storage, geometric_storage);

  auto [u0, u1] = sampler.NextLinear2D();
  EXPECT_EQ(0.00, u0);
  EXPECT_EQ(0.50, u1);
}

TEST(NextLinear2D, ConsumesExisting) {
  MockRandomBitstream rng_bitstream;
  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, nullptr, visual_storage, geometric_storage);

  geometric_storage.emplace_back(0.25, 0.25);
  geometric_storage.emplace_back(0.50, 0.50);
  EXPECT_EQ(0.25, sampler.NextLinear2D().first);
  EXPECT_EQ(0.50, sampler.NextLinear2D().first);
}

TEST(NextPolar, ConsumesExisting) {
  MockRandomBitstream rng_bitstream;
  std::vector<visual_t> visual_storage;
  std::vector<std::pair<geometric_t, geometric_t>> geometric_storage;
  Sampler sampler(rng_bitstream, nullptr, visual_storage, geometric_storage);

  geometric_storage.push_back({0.50, 0.75});
  auto [radius, theta] = sampler.NextPolar();
  EXPECT_NEAR(0.500, radius, 0.001);
  EXPECT_NEAR(1.570, theta, 0.001);
}

}  // namespace
}  // namespace iris
