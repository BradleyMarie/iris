#include "iris/file/exr_writer.h"

#include <OpenEXR/ImfIO.h>
#include <OpenEXR/ImfRgbaFile.h>

#include <limits>
#include <ostream>
#include <sstream>
#include <vector>

#include "iris/color.h"
#include "iris/framebuffer.h"

namespace iris {
namespace file {
namespace {

class OStreamWrapper : public Imf::OStream {
 public:
  OStreamWrapper(std::ostream& stream)
      : Imf::OStream("std::ostream"), stream_(stream) {}

  void write(const char c[], int n) override { stream_.write(c, n); }
  uint64_t tellp() override { return static_cast<uint64_t>(stream_.tellp()); }
  void seekp(uint64_t pos) override {
    stream_.seekp(static_cast<std::streamoff>(pos));
  }

 private:
  std::ostream& stream_;
};

}  // namespace

bool WriteExr(const Framebuffer& framebuffer, std::ostream& output) {
  auto [size_y, size_x] = framebuffer.Size();

  std::vector<Imf::Rgba> image;
  for (size_t y = 0; y < size_y; y++) {
    for (size_t x = 0; x < size_x; x++) {
      Color color = framebuffer.Get(y, x);

      // Per the OpenEXR spec, a file doesn't have a chromaticities attribute is
      // assumed to have a white point primaries that match Rec. ITU-R BT.709-3.
      // These primaries and whitepoint are also shared by linear SRGB.
      Color rgb_color = color.ConvertTo(Color::LINEAR_SRGB);

      image.emplace_back(half(rgb_color.r), half(rgb_color.g),
                         half(rgb_color.b), /*a=*/half(1.0));
    }
  }

  OStreamWrapper stream_wrapper(output);
  Imf::RgbaOutputFile file(stream_wrapper, Imf::Header(size_x, size_y),
                           Imf::WRITE_RGBA);

  file.setFrameBuffer(image.data(), 1, size_x);
  file.writePixels(size_y);

  return output.good();
}

}  // namespace file
}  // namespace iris
