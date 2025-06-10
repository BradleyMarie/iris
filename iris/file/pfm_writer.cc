#include "iris/file/pfm_writer.h"

#include <bit>
#include <ostream>
#include <sstream>
#include <string>

#include "iris/color.h"
#include "iris/framebuffer.h"

namespace iris {
namespace file {

bool WritePfm(const Framebuffer& framebuffer, Color::Space color_space,
              std::ostream& output) {
  auto [size_y, size_x] = framebuffer.Size();

  std::stringstream header_builder(std::ios::in | std::ios::out |
                                   std::ios::binary);

  header_builder << "PF\n" << size_x << " " << size_y << "\n";
  if constexpr (std::endian::native == std::endian::little) {
    header_builder << "-";
  }
  header_builder << "1.0\n";

  std::string header = header_builder.str();
  output.write(header.c_str(), header.size());

  for (size_t y = 0; y < size_y; y++) {
    for (size_t x = 0; x < size_x; x++) {
      Color color = framebuffer.Get(y, x).ConvertTo(color_space);

      output.write(reinterpret_cast<const char*>(&color.c0), 4);
      static_assert(sizeof(float) == 4);

      output.write(reinterpret_cast<const char*>(&color.c1), 4);
      static_assert(sizeof(float) == 4);

      output.write(reinterpret_cast<const char*>(&color.c2), 4);
      static_assert(sizeof(float) == 4);
    }
  }

  return output.good();
}

}  // namespace file
}  // namespace iris
