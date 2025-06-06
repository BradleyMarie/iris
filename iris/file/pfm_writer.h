#ifndef _IRIS_FILE_PFM_WRITER_
#define _IRIS_FILE_PFM_WRITER_

#include <ostream>

#include "iris/color.h"
#include "iris/framebuffer.h"

namespace iris {
namespace file {

bool WritePfm(const Framebuffer& framebuffer, Color::Space color_space,
              std::ostream& output);

}  // namespace file
}  // namespace iris

#endif  // _IRIS_FILE_PFM_WRITER_
