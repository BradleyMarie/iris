#ifndef _IRIS_RANDOM_MERSENNE_TWISTER_RANDOM_BITSTREAM_
#define _IRIS_RANDOM_MERSENNE_TWISTER_RANDOM_BITSTREAM_

#include <memory>

#include "iris/random_bitstream.h"

namespace iris {
namespace random_bitstreams {

std::unique_ptr<RandomBitstream> MakeMersenneTwisterRandomBitstream();

}  // namespace random_bitstreams
}  // namespace iris

#endif  // _IRIS_RANDOM_MERSENNE_TWISTER_RANDOM_BITSTREAM_
