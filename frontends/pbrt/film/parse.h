#ifndef _FRONTENDS_PBRT_FILM_PARSE_
#define _FRONTENDS_PBRT_FILM_PARSE_

#include <memory>

#include "frontends/pbrt/film/result.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {

std::unique_ptr<FilmResult> ParseFilm(const pbrt_proto::v3::Film& film);

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_FILM_PARSE_
