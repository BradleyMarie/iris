#ifndef _IRIS_BXDFS_FOURIER_BXDF_
#define _IRIS_BXDFS_FOURIER_BXDF_

#include <cstddef>
#include <span>
#include <utility>

#include "iris/bxdf.h"
#include "iris/bxdf_allocator.h"
#include "iris/float.h"

namespace iris {
namespace bxdfs {

const Bxdf* MakeFourierBxdf(
    BxdfAllocator& bxdf_allocator, const Reflector* reflectance,
    std::span<const geometric> elevational_samples, std::span<const visual> cdf,
    std::span<const std::pair<size_t, size_t>> coefficient_extents,
    std::span<const visual> y_coefficients, visual eta_transmitted);

const Bxdf* MakeFourierBxdf(
    BxdfAllocator& bxdf_allocator, const Reflector* r, const Reflector* g,
    const Reflector* b, std::span<const geometric> elevational_samples,
    std::span<const visual> cdf,
    std::span<const std::pair<size_t, size_t>> coefficient_extents,
    std::span<const visual> y_coefficients,
    std::span<const visual> r_coefficients,
    std::span<const visual> b_coefficients, visual eta_transmitted);

}  // namespace bxdfs
}  // namespace iris

#endif  // _IRIS_BXDFS_FOURIER_BXDF_
