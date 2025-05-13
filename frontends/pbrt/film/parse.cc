#include "frontends/pbrt/film/parse.h"

#include <memory>

#include "frontends/pbrt/film/image.h"
#include "frontends/pbrt/film/result.h"

namespace iris {
namespace pbrt_frontend {

using ::pbrt_proto::v3::Film;

std::unique_ptr<FilmResult> ParseFilm(const Film& film) {
  std::unique_ptr<FilmResult> result;
  switch (film.film_type_case()) {
    case Film::kImage:
      result = film::MakeImage(film.image());
      break;
    case Film::FILM_TYPE_NOT_SET:
      break;
  }

  return result;
}

}  // namespace pbrt_frontend
}  // namespace iris
