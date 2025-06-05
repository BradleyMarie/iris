#ifndef _IRIS_RANDOM_MERSENNE_TWISTER_RANDOM_
#define _IRIS_RANDOM_MERSENNE_TWISTER_RANDOM_

#include <memory>

#include "iris/random.h"

namespace iris {
namespace random {

std::unique_ptr<Random> MakeMersenneTwisterRandom();

}  // namespace random
}  // namespace iris

#endif  // _IRIS_RANDOM_MERSENNE_TWISTER_RANDOM_
