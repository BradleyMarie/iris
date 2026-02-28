#include "frontends/pbrt/samplers/stratified.h"

#include <cstdlib>

#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace samplers {
namespace {

using ::pbrt_proto::StratifiedSampler;
using ::testing::ExitedWithCode;

TEST(Stratified, Empty) {
  StratifiedSampler stratified;

  EXPECT_TRUE(MakeStratified(stratified));
}

TEST(Stratified, TooHighXSamples) {
  StratifiedSampler stratified;
  stratified.set_xsamples(80000);

  EXPECT_EXIT(MakeStratified(stratified), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: xsamples");
}

TEST(Stratified, TooHighYSamples) {
  StratifiedSampler stratified;
  stratified.set_ysamples(80000);

  EXPECT_EXIT(MakeStratified(stratified), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: ysamples");
}

}  // namespace
}  // namespace samplers
}  // namespace pbrt_frontend
}  // namespace iris
