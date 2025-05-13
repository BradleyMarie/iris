#include "frontends/pbrt/film/parse.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace pbrt_frontend {
namespace {

using ::pbrt_proto::v3::Film;

TEST(ParseAreaLightSource, Empty) {
  Film film;

  EXPECT_FALSE(ParseFilm(film));
}

TEST(ParseAreaLightSource, Image) {
  Film film;
  film.mutable_image();

  EXPECT_TRUE(ParseFilm(film));
}

}  // namespace
}  // namespace pbrt_frontend
}  // namespace iris
