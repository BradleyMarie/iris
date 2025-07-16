#include "frontends/pbrt/samplers/parse.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::pbrt_proto::v3::Sampler;

TEST(ParseSampler, Empty) {
  Sampler sampler;
  EXPECT_FALSE(ParseSampler(sampler));
}

TEST(ParseSampler, Halton) {
  Sampler sampler;
  sampler.mutable_halton();
  EXPECT_TRUE(ParseSampler(sampler));
}

TEST(ParseSampler, MaxMinDist) {
  Sampler sampler;
  sampler.mutable_maxmindist();
  EXPECT_TRUE(ParseSampler(sampler));
}

TEST(ParseSampler, Random) {
  Sampler sampler;
  sampler.mutable_random();
  EXPECT_TRUE(ParseSampler(sampler));
}

TEST(ParseSampler, Sobol) {
  Sampler sampler;
  sampler.mutable_sobol();
  EXPECT_TRUE(ParseSampler(sampler));
}

TEST(ParseSampler, Stratified) {
  Sampler sampler;
  sampler.mutable_stratified();
  EXPECT_TRUE(ParseSampler(sampler));
}

TEST(ParseSampler, ZeroTwoSequence) {
  Sampler sampler;
  sampler.mutable_zerotwosequence();
  EXPECT_TRUE(ParseSampler(sampler));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
