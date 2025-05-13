#include "frontends/pbrt/film/image.h"

#include <cstdlib>
#include <memory>

#include "frontends/pbrt/film/result.h"
#include "googletest/include/gtest/gtest.h"
#include "pbrt_proto/v3/pbrt.pb.h"

namespace iris {
namespace pbrt_frontend {
namespace film {
namespace {

using ::pbrt_proto::v3::Film;
using ::testing::ExitedWithCode;

TEST(Image, Empty) {
  Film::Image image;

  std::unique_ptr<FilmResult> result = MakeImage(image);
  ASSERT_TRUE(result);

  EXPECT_EQ("pbrt.exr", result->filename);
  EXPECT_EQ(720u, result->resolution.first);
  EXPECT_EQ(1280u, result->resolution.second);
  EXPECT_FALSE(result->skip_pixel_function({0, 0}, {720, 1280}));
  EXPECT_FALSE(result->skip_pixel_function({719, 1279}, {720, 1280}));
  EXPECT_TRUE(result->skip_pixel_function({720, 1280}, {720, 1280}));
  EXPECT_EQ(35.0, result->diagonal);
  EXPECT_FALSE(result->max_sample_luminance);
}

TEST(Image, NegativeCropWindow0) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(-1.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, NegativeCropWindow1) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(-1.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, NegativeCropWindow2) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(-1.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, NegativeCropWindow3) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(-1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, TooLargeCropWindow0) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(2.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, TooLargeCropWindow1) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(2.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, TooLargeCropWindow2) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(2.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, TooLargeCropWindow3) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(2.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: cropwindow");
}

TEST(Image, BadCropWindow0) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(0.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(1.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid values for parameter list: cropwindow");
}

TEST(Image, BadCropWindow1) {
  Film::Image image;
  image.mutable_cropwindow()->set_x_min(0.0);
  image.mutable_cropwindow()->set_x_max(1.0);
  image.mutable_cropwindow()->set_y_min(0.0);
  image.mutable_cropwindow()->set_y_max(0.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Invalid values for parameter list: cropwindow");
}

TEST(Image, Diagonal) {
  Film::Image image;
  image.set_diagonal(0.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: diagonal");
}

TEST(Image, MaxSampleLuminance) {
  Film::Image image;
  image.set_maxsampleluminance(0.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: maxsampleluminance");
}

TEST(Image, Scale) {
  Film::Image image;
  image.set_scale(0.0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: scale");
}

TEST(Image, XResolutionNegative) {
  Film::Image image;
  image.set_xresolution(0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: xresolution");
}

TEST(Image, XResolutionTooLarge) {
  Film::Image image;
  image.set_xresolution(16385);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: xresolution");
}

TEST(Image, YResolutionNegative) {
  Film::Image image;
  image.set_yresolution(0);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: yresolution");
}

TEST(Image, YResolutionTooLarge) {
  Film::Image image;
  image.set_yresolution(16385);

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Out of range value for parameter: yresolution");
}

TEST(Image, BadExtension) {
  Film::Image image;
  image.set_filename("image.txt");

  EXPECT_EXIT(MakeImage(image), ExitedWithCode(EXIT_FAILURE),
              "ERROR: Unsupported file extension for parameter: filename");
}

}  // namespace
}  // namespace film
}  // namespace pbrt_frontend
}  // namespace iris