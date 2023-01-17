#ifndef _IRIS_FILE_EXR_WRITER_
#define _IRIS_FILE_EXR_WRITER_

#include <ostream>

#include "iris/framebuffer.h"

namespace iris {
namespace file {

bool WriteExr(const Framebuffer& framebuffer, Color::Space color_space,
              std::ostream& output);

}  // namespace file
}  // namespace iris

#endif  // _IRIS_FILE_EXR_WRITER_