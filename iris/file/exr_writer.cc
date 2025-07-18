#include "iris/file/exr_writer.h"

#include <limits>
#include <ostream>
#include <sstream>
#include <vector>

#include "iris/color.h"
#include "iris/framebuffer.h"
#include "third_party/tinyexr/tinyexr.h"

namespace iris {
namespace file {

bool WriteExr(const Framebuffer& framebuffer, std::ostream& output) {
  auto [size_y, size_x] = framebuffer.Size();

  std::vector<float> image;
  for (size_t y = 0; y < size_y; y++) {
    for (size_t x = 0; x < size_x; x++) {
      Color color = framebuffer.Get(y, x);

      // Per the OpenEXR spec, a file doesn't have a chromaticities attribute is
      // assumed to have a white point primaries that match Rec. ITU-R BT.709-3.
      // These primaries and whitepoint are also shared by linear SRGB.
      Color rgb_color = color.ConvertTo(Color::LINEAR_SRGB);

      image.push_back(static_cast<float>(rgb_color.r));
      image.push_back(static_cast<float>(rgb_color.g));
      image.push_back(static_cast<float>(rgb_color.b));
    }
  }

  if (std::numeric_limits<int>::max() < size_x ||
      std::numeric_limits<int>::max() < size_y) {
    return false;
  }

  unsigned char* buffer;
  int bytes_written =
      SaveEXRToMemory(image.data(), /*width=*/static_cast<int>(size_x),
                      /*height=*/static_cast<int>(size_y), /*components=*/3,
                      /*save_as_fp16=*/1, /*buffer=*/&buffer, /*err=*/nullptr);
  if (bytes_written < 0) {
    return false;
  }

  output.write(reinterpret_cast<const char*>(buffer),
               static_cast<std::streamsize>(bytes_written));

  free(buffer);

  return output.good();
}

}  // namespace file
}  // namespace iris
