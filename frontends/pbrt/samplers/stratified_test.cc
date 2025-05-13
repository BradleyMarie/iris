#include "frontends/pbrt/samplers/stratified.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::v3::Sampler;
using ::testing::ExitedWithCode;

TEST(Stratified, Empty) {
  Sampler::Stratified stratified;

  EXPECT_TRUE(MakeStratified(stratified));
}

TEST(Stratified, TooLowXSamples) {
  Sampler::Stratified stratified;
  stratified.set_xsamples(-1);

  EXPECT_EXIT(MakeStratified(stratified), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: xsamples");
}

TEST(Stratified, TooHighXSamples) {
  Sampler::Stratified stratified;
  stratified.set_xsamples(80000);

  EXPECT_EXIT(MakeStratified(stratified), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: xsamples");
}

TEST(Stratified, TooLowYSamples) {
  Sampler::Stratified stratified;
  stratified.set_ysamples(-1);

  EXPECT_EXIT(MakeStratified(stratified), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: ysamples");
}

TEST(Stratified, TooHighYSamples) {
  Sampler::Stratified stratified;
  stratified.set_ysamples(80000);

  EXPECT_EXIT(MakeStratified(stratified), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: ysamples");
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
