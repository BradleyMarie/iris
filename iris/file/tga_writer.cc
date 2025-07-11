#include "iris/file/tga_writer.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>
#include <vector>

#include "iris/color.h"
#include "iris/framebuffer.h"
#include "third_party/stb/stb_image_write.h"

namespace iris {
namespace file {
namespace {

visual_t GammaCorrect(visual_t value) {
  if (value <= static_cast<visual_t>(0.0031308)) {
    return static_cast<visual_t>(12.92) * value;
  }

  return static_cast<visual_t>(1.055) *
             std::pow(value, static_cast<visual_t>(1.0 / 2.4)) -
         static_cast<visual_t>(0.055);
}

uint8_t ToByte(visual_t value) {
  visual_t corrected =
      std::clamp(static_cast<visual_t>(255.0) * GammaCorrect(value),
                 static_cast<visual_t>(0.0), static_cast<visual_t>(255.0));
  return static_cast<uint8_t>(corrected);
}

void WriteFunc(void* context, void* data, int size) {
  if (size > 0) {
    std::ostream* output = static_cast<std::ostream*>(context);
    output->write(static_cast<char*>(data), size);
  }
}

}  // namespace

bool WriteTga(const Framebuffer& framebuffer, Color::Space color_space,
              std::ostream& output) {
  auto [size_y, size_x] = framebuffer.Size();
  if (size_x > std::numeric_limits<int>::max() ||
      size_y > std::numeric_limits<int>::max()) {
    return false;
  }

  std::vector<uint8_t> image;
  for (size_t y = 0; y < size_y; y++) {
    for (size_t x = 0; x < size_x; x++) {
      Color color = framebuffer.Get(y, x).ConvertTo(color_space);
      image.push_back(ToByte(color.r));
      image.push_back(ToByte(color.g));
      image.push_back(ToByte(color.b));
    }
  }

  if (!stbi_write_tga_to_func(WriteFunc, &output, static_cast<int>(size_x),
                              static_cast<int>(size_y),
                              /*comp=*/3, image.data())) {
    return false;
  }

  return output.good();
}

}  // namespace file
}  // namespace iris
