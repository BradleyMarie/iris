// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#ifndef PBRT_UTIL_SOBOLMATRICES_H
#define PBRT_UTIL_SOBOLMATRICES_H

#include <cstdint>

namespace pbrt {

// Sobol Matrix Declarations
static constexpr int SobolMatrixSize = 52;
static constexpr int VdCSobolMatricesSize = 25;
static constexpr int VdCSobolMatricesInvSize = 26;
extern const uint64_t VdCSobolMatrices[VdCSobolMatricesSize][SobolMatrixSize];
extern const uint64_t VdCSobolMatricesInv[VdCSobolMatricesInvSize]
                                         [SobolMatrixSize];

}  // namespace pbrt

#endif  // PBRT_UTIL_SOBOLMATRICES_H
