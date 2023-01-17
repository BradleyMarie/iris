#include "iris/file/exr_writer.h"

#include <sstream>

#include "third_party/tinyexr/tinyexr.h"

namespace iris {
namespace file {

bool WriteExr(const Framebuffer& framebuffer, Color::Space color_space,
              std::ostream& output) {
  auto size = framebuffer.Size();

  std::vector<float> image;
  for (size_t y = 0; y < size.first; y++) {
    for (size_t x = 0; x < size.second; x++) {
      auto color = framebuffer.Get(y, x);
      auto rgb_color = color.ConvertTo(Color::LINEAR_SRGB);
      image.push_back(static_cast<float>(rgb_color.r));
      image.push_back(static_cast<float>(rgb_color.g));
      image.push_back(static_cast<float>(rgb_color.b));
    }
  }

  if (std::numeric_limits<int>::max() < size.second ||
      std::numeric_limits<int>::max() < size.first) {
    return false;
  }

  unsigned char* buffer;
  int bytes_written =
      SaveEXRToMemory(image.data(), /*width=*/static_cast<int>(size.second),
                      /*height=*/static_cast<int>(size.first), /*components=*/3,
                      /*save_as_fp16=*/1, /*buffer=*/&buffer, /*err=*/nullptr);

  if (bytes_written < 0) {
    return false;
  }

  output.write(reinterpret_cast<const char*>(buffer),
               static_cast<std::streamsize>(bytes_written));

  free(buffer);

  return true;
}

}  // namespace file
}  // namespace iris