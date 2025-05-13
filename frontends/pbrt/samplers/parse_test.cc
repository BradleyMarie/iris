#include "frontends/pbrt/samplers/parse.h"

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::pbrt_proto::v3::Sampler;

TEST(ParseSamper, Empty) {
  Sampler sampler;
  EXPECT_FALSE(ParseSamper(sampler));
}

TEST(ParseSamper, Halton) {
  Sampler sampler;
  sampler.mutable_halton();
  EXPECT_TRUE(ParseSamper(sampler));
}

TEST(ParseSamper, MaxMinDist) {
  Sampler sampler;
  sampler.mutable_maxmindist();
  EXPECT_FALSE(ParseSamper(sampler));
}

TEST(ParseSamper, Random) {
  Sampler sampler;
  sampler.mutable_random();
  EXPECT_TRUE(ParseSamper(sampler));
}

TEST(ParseSamper, Sobol) {
  Sampler sampler;
  sampler.mutable_sobol();
  EXPECT_TRUE(ParseSamper(sampler));
}

TEST(ParseSamper, Stratified) {
  Sampler sampler;
  sampler.mutable_stratified();
  EXPECT_TRUE(ParseSamper(sampler));
}

TEST(ParseSamper, ZeroTwoSequence) {
  Sampler sampler;
  sampler.mutable_zerotwosequence();
  EXPECT_FALSE(ParseSamper(sampler));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
