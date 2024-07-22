#include "iris/environmental_lights/internal/distribution_2d.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace iris {
namespace environmental_lights {
namespace internal {
namespace {

std::vector<Distribution1D> ComputeRows(std::span<const visual> values,
                                        std::pair<size_t, size_t> size) {
  assert(size.first * size.second == values.size());

  std::vector<Distribution1D> rows;
  rows.reserve(size.first);

  for (size_t y = 0; y < size.first; y += 1) {
    auto start = values.begin() + size.second * y;
    auto end = start + size.second;
    rows.emplace_back(std::span<const visual>(start, end));
  }

  return rows;
}

Distribution1D ComputeRowDistribution(const std::vector<Distribution1D>& rows) {
  std::vector<visual> averages;
  for (const auto& entry : rows) {
    averages.push_back(entry.Average());
  }
  return Distribution1D(std::move(averages));
}

}  // namespace

Distribution2D::Distribution2D(std::span<const visual> values,
                               std::pair<size_t, size_t> size)
    : Distribution2D(ComputeRows(values, size), size) {}

Distribution2D::Distribution2D(std::vector<Distribution1D> rows,
                               std::pair<size_t, size_t> size)
    : row_distribution_(ComputeRowDistribution(rows)),
      rows_(std::move(rows)),
      size_(size) {}

std::pair<geometric_t, geometric_t> Distribution2D::Sample(
    Sampler& sampler, visual_t* pdf, size_t* offset) const {
  visual_t marginal_pdf;
  size_t offset_y;
  geometric_t v =
      row_distribution_.SampleContinuous(sampler, &marginal_pdf, &offset_y);

  visual_t conditional_pdf;
  size_t offset_x;
  geometric_t u =
      rows_[offset_y].SampleContinuous(sampler, &conditional_pdf, &offset_x);

  if (pdf) {
    *pdf = marginal_pdf * conditional_pdf;
  }

  if (offset) {
    *offset = offset_y * size_.second + offset_x;
  }

  return std::make_pair(u, v);
}

visual_t Distribution2D::Pdf(visual_t u, visual_t v) const {
  assert(0.0 <= u && u < 1.0);
  assert(0.0 <= v && v < 1.0);

  size_t y = v * static_cast<geometric_t>(size_.first);

  visual_t pdf_u = rows_[y].PdfContinuous(u);
  visual_t pdf_v = row_distribution_.PdfContinuous(v);

  return pdf_u * pdf_v;
}

}  // namespace internal
}  // namespace environmental_lights
}  // namespace iris