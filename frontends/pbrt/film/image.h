#ifndef _FRONTENDS_PBRT_FILM_IMAGE_
#define _FRONTENDS_PBRT_FILM_IMAGE_

#include <memory>

#include "frontends/pbrt/film/result.h"
#include "pbrt_proto/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace film {

std::unique_ptr<FilmResult> MakeImage(const pbrt_proto::RgbFilm& image);

}  // namespace film
}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_FILM_IMAGE_
