#include "frontends/pbrt/image_manager.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include "third_party/stb/stb_image.h"

namespace iris::pbrt_frontend {
namespace {

visual GammaCorrect(visual_t value) {
  if (value <= static_cast<visual_t>(0.04045)) {
    return value / static_cast<visual_t>(12.92);
  }

  return std::pow(
      (value + static_cast<visual_t>(0.055)) / static_cast<visual_t>(1.055),
      static_cast<visual_t>(2.4));
}
const std::array<visual, 256>& FloatLUT(bool gamma_correct) {
  static const std::array<visual, 256> gamma_correct_lut = {
      GammaCorrect(static_cast<visual>(0.0 / 255.0)),
      GammaCorrect(static_cast<visual>(1.0 / 255.0)),
      GammaCorrect(static_cast<visual>(2.0 / 255.0)),
      GammaCorrect(static_cast<visual>(3.0 / 255.0)),
      GammaCorrect(static_cast<visual>(4.0 / 255.0)),
      GammaCorrect(static_cast<visual>(5.0 / 255.0)),
      GammaCorrect(static_cast<visual>(6.0 / 255.0)),
      GammaCorrect(static_cast<visual>(7.0 / 255.0)),
      GammaCorrect(static_cast<visual>(8.0 / 255.0)),
      GammaCorrect(static_cast<visual>(9.0 / 255.0)),
      GammaCorrect(static_cast<visual>(10.0 / 255.0)),
      GammaCorrect(static_cast<visual>(11.0 / 255.0)),
      GammaCorrect(static_cast<visual>(12.0 / 255.0)),
      GammaCorrect(static_cast<visual>(13.0 / 255.0)),
      GammaCorrect(static_cast<visual>(14.0 / 255.0)),
      GammaCorrect(static_cast<visual>(15.0 / 255.0)),
      GammaCorrect(static_cast<visual>(16.0 / 255.0)),
      GammaCorrect(static_cast<visual>(17.0 / 255.0)),
      GammaCorrect(static_cast<visual>(18.0 / 255.0)),
      GammaCorrect(static_cast<visual>(19.0 / 255.0)),
      GammaCorrect(static_cast<visual>(20.0 / 255.0)),
      GammaCorrect(static_cast<visual>(21.0 / 255.0)),
      GammaCorrect(static_cast<visual>(22.0 / 255.0)),
      GammaCorrect(static_cast<visual>(23.0 / 255.0)),
      GammaCorrect(static_cast<visual>(24.0 / 255.0)),
      GammaCorrect(static_cast<visual>(25.0 / 255.0)),
      GammaCorrect(static_cast<visual>(26.0 / 255.0)),
      GammaCorrect(static_cast<visual>(27.0 / 255.0)),
      GammaCorrect(static_cast<visual>(28.0 / 255.0)),
      GammaCorrect(static_cast<visual>(29.0 / 255.0)),
      GammaCorrect(static_cast<visual>(30.0 / 255.0)),
      GammaCorrect(static_cast<visual>(31.0 / 255.0)),
      GammaCorrect(static_cast<visual>(32.0 / 255.0)),
      GammaCorrect(static_cast<visual>(33.0 / 255.0)),
      GammaCorrect(static_cast<visual>(34.0 / 255.0)),
      GammaCorrect(static_cast<visual>(35.0 / 255.0)),
      GammaCorrect(static_cast<visual>(36.0 / 255.0)),
      GammaCorrect(static_cast<visual>(37.0 / 255.0)),
      GammaCorrect(static_cast<visual>(38.0 / 255.0)),
      GammaCorrect(static_cast<visual>(39.0 / 255.0)),
      GammaCorrect(static_cast<visual>(40.0 / 255.0)),
      GammaCorrect(static_cast<visual>(41.0 / 255.0)),
      GammaCorrect(static_cast<visual>(42.0 / 255.0)),
      GammaCorrect(static_cast<visual>(43.0 / 255.0)),
      GammaCorrect(static_cast<visual>(44.0 / 255.0)),
      GammaCorrect(static_cast<visual>(45.0 / 255.0)),
      GammaCorrect(static_cast<visual>(46.0 / 255.0)),
      GammaCorrect(static_cast<visual>(47.0 / 255.0)),
      GammaCorrect(static_cast<visual>(48.0 / 255.0)),
      GammaCorrect(static_cast<visual>(49.0 / 255.0)),
      GammaCorrect(static_cast<visual>(50.0 / 255.0)),
      GammaCorrect(static_cast<visual>(51.0 / 255.0)),
      GammaCorrect(static_cast<visual>(52.0 / 255.0)),
      GammaCorrect(static_cast<visual>(53.0 / 255.0)),
      GammaCorrect(static_cast<visual>(54.0 / 255.0)),
      GammaCorrect(static_cast<visual>(55.0 / 255.0)),
      GammaCorrect(static_cast<visual>(56.0 / 255.0)),
      GammaCorrect(static_cast<visual>(57.0 / 255.0)),
      GammaCorrect(static_cast<visual>(58.0 / 255.0)),
      GammaCorrect(static_cast<visual>(59.0 / 255.0)),
      GammaCorrect(static_cast<visual>(60.0 / 255.0)),
      GammaCorrect(static_cast<visual>(61.0 / 255.0)),
      GammaCorrect(static_cast<visual>(62.0 / 255.0)),
      GammaCorrect(static_cast<visual>(63.0 / 255.0)),
      GammaCorrect(static_cast<visual>(64.0 / 255.0)),
      GammaCorrect(static_cast<visual>(65.0 / 255.0)),
      GammaCorrect(static_cast<visual>(66.0 / 255.0)),
      GammaCorrect(static_cast<visual>(67.0 / 255.0)),
      GammaCorrect(static_cast<visual>(68.0 / 255.0)),
      GammaCorrect(static_cast<visual>(69.0 / 255.0)),
      GammaCorrect(static_cast<visual>(70.0 / 255.0)),
      GammaCorrect(static_cast<visual>(71.0 / 255.0)),
      GammaCorrect(static_cast<visual>(72.0 / 255.0)),
      GammaCorrect(static_cast<visual>(73.0 / 255.0)),
      GammaCorrect(static_cast<visual>(74.0 / 255.0)),
      GammaCorrect(static_cast<visual>(75.0 / 255.0)),
      GammaCorrect(static_cast<visual>(76.0 / 255.0)),
      GammaCorrect(static_cast<visual>(77.0 / 255.0)),
      GammaCorrect(static_cast<visual>(78.0 / 255.0)),
      GammaCorrect(static_cast<visual>(79.0 / 255.0)),
      GammaCorrect(static_cast<visual>(80.0 / 255.0)),
      GammaCorrect(static_cast<visual>(81.0 / 255.0)),
      GammaCorrect(static_cast<visual>(82.0 / 255.0)),
      GammaCorrect(static_cast<visual>(83.0 / 255.0)),
      GammaCorrect(static_cast<visual>(84.0 / 255.0)),
      GammaCorrect(static_cast<visual>(85.0 / 255.0)),
      GammaCorrect(static_cast<visual>(86.0 / 255.0)),
      GammaCorrect(static_cast<visual>(87.0 / 255.0)),
      GammaCorrect(static_cast<visual>(88.0 / 255.0)),
      GammaCorrect(static_cast<visual>(89.0 / 255.0)),
      GammaCorrect(static_cast<visual>(90.0 / 255.0)),
      GammaCorrect(static_cast<visual>(91.0 / 255.0)),
      GammaCorrect(static_cast<visual>(92.0 / 255.0)),
      GammaCorrect(static_cast<visual>(93.0 / 255.0)),
      GammaCorrect(static_cast<visual>(94.0 / 255.0)),
      GammaCorrect(static_cast<visual>(95.0 / 255.0)),
      GammaCorrect(static_cast<visual>(96.0 / 255.0)),
      GammaCorrect(static_cast<visual>(97.0 / 255.0)),
      GammaCorrect(static_cast<visual>(98.0 / 255.0)),
      GammaCorrect(static_cast<visual>(99.0 / 255.0)),
      GammaCorrect(static_cast<visual>(100.0 / 255.0)),
      GammaCorrect(static_cast<visual>(101.0 / 255.0)),
      GammaCorrect(static_cast<visual>(102.0 / 255.0)),
      GammaCorrect(static_cast<visual>(103.0 / 255.0)),
      GammaCorrect(static_cast<visual>(104.0 / 255.0)),
      GammaCorrect(static_cast<visual>(105.0 / 255.0)),
      GammaCorrect(static_cast<visual>(106.0 / 255.0)),
      GammaCorrect(static_cast<visual>(107.0 / 255.0)),
      GammaCorrect(static_cast<visual>(108.0 / 255.0)),
      GammaCorrect(static_cast<visual>(109.0 / 255.0)),
      GammaCorrect(static_cast<visual>(110.0 / 255.0)),
      GammaCorrect(static_cast<visual>(111.0 / 255.0)),
      GammaCorrect(static_cast<visual>(112.0 / 255.0)),
      GammaCorrect(static_cast<visual>(113.0 / 255.0)),
      GammaCorrect(static_cast<visual>(114.0 / 255.0)),
      GammaCorrect(static_cast<visual>(115.0 / 255.0)),
      GammaCorrect(static_cast<visual>(116.0 / 255.0)),
      GammaCorrect(static_cast<visual>(117.0 / 255.0)),
      GammaCorrect(static_cast<visual>(118.0 / 255.0)),
      GammaCorrect(static_cast<visual>(119.0 / 255.0)),
      GammaCorrect(static_cast<visual>(120.0 / 255.0)),
      GammaCorrect(static_cast<visual>(121.0 / 255.0)),
      GammaCorrect(static_cast<visual>(122.0 / 255.0)),
      GammaCorrect(static_cast<visual>(123.0 / 255.0)),
      GammaCorrect(static_cast<visual>(124.0 / 255.0)),
      GammaCorrect(static_cast<visual>(125.0 / 255.0)),
      GammaCorrect(static_cast<visual>(126.0 / 255.0)),
      GammaCorrect(static_cast<visual>(127.0 / 255.0)),
      GammaCorrect(static_cast<visual>(128.0 / 255.0)),
      GammaCorrect(static_cast<visual>(129.0 / 255.0)),
      GammaCorrect(static_cast<visual>(130.0 / 255.0)),
      GammaCorrect(static_cast<visual>(131.0 / 255.0)),
      GammaCorrect(static_cast<visual>(132.0 / 255.0)),
      GammaCorrect(static_cast<visual>(133.0 / 255.0)),
      GammaCorrect(static_cast<visual>(134.0 / 255.0)),
      GammaCorrect(static_cast<visual>(135.0 / 255.0)),
      GammaCorrect(static_cast<visual>(136.0 / 255.0)),
      GammaCorrect(static_cast<visual>(137.0 / 255.0)),
      GammaCorrect(static_cast<visual>(138.0 / 255.0)),
      GammaCorrect(static_cast<visual>(139.0 / 255.0)),
      GammaCorrect(static_cast<visual>(140.0 / 255.0)),
      GammaCorrect(static_cast<visual>(141.0 / 255.0)),
      GammaCorrect(static_cast<visual>(142.0 / 255.0)),
      GammaCorrect(static_cast<visual>(143.0 / 255.0)),
      GammaCorrect(static_cast<visual>(144.0 / 255.0)),
      GammaCorrect(static_cast<visual>(145.0 / 255.0)),
      GammaCorrect(static_cast<visual>(146.0 / 255.0)),
      GammaCorrect(static_cast<visual>(147.0 / 255.0)),
      GammaCorrect(static_cast<visual>(148.0 / 255.0)),
      GammaCorrect(static_cast<visual>(149.0 / 255.0)),
      GammaCorrect(static_cast<visual>(150.0 / 255.0)),
      GammaCorrect(static_cast<visual>(151.0 / 255.0)),
      GammaCorrect(static_cast<visual>(152.0 / 255.0)),
      GammaCorrect(static_cast<visual>(153.0 / 255.0)),
      GammaCorrect(static_cast<visual>(154.0 / 255.0)),
      GammaCorrect(static_cast<visual>(155.0 / 255.0)),
      GammaCorrect(static_cast<visual>(156.0 / 255.0)),
      GammaCorrect(static_cast<visual>(157.0 / 255.0)),
      GammaCorrect(static_cast<visual>(158.0 / 255.0)),
      GammaCorrect(static_cast<visual>(159.0 / 255.0)),
      GammaCorrect(static_cast<visual>(160.0 / 255.0)),
      GammaCorrect(static_cast<visual>(161.0 / 255.0)),
      GammaCorrect(static_cast<visual>(162.0 / 255.0)),
      GammaCorrect(static_cast<visual>(163.0 / 255.0)),
      GammaCorrect(static_cast<visual>(164.0 / 255.0)),
      GammaCorrect(static_cast<visual>(165.0 / 255.0)),
      GammaCorrect(static_cast<visual>(166.0 / 255.0)),
      GammaCorrect(static_cast<visual>(167.0 / 255.0)),
      GammaCorrect(static_cast<visual>(168.0 / 255.0)),
      GammaCorrect(static_cast<visual>(169.0 / 255.0)),
      GammaCorrect(static_cast<visual>(170.0 / 255.0)),
      GammaCorrect(static_cast<visual>(171.0 / 255.0)),
      GammaCorrect(static_cast<visual>(172.0 / 255.0)),
      GammaCorrect(static_cast<visual>(173.0 / 255.0)),
      GammaCorrect(static_cast<visual>(174.0 / 255.0)),
      GammaCorrect(static_cast<visual>(175.0 / 255.0)),
      GammaCorrect(static_cast<visual>(176.0 / 255.0)),
      GammaCorrect(static_cast<visual>(177.0 / 255.0)),
      GammaCorrect(static_cast<visual>(178.0 / 255.0)),
      GammaCorrect(static_cast<visual>(179.0 / 255.0)),
      GammaCorrect(static_cast<visual>(180.0 / 255.0)),
      GammaCorrect(static_cast<visual>(181.0 / 255.0)),
      GammaCorrect(static_cast<visual>(182.0 / 255.0)),
      GammaCorrect(static_cast<visual>(183.0 / 255.0)),
      GammaCorrect(static_cast<visual>(184.0 / 255.0)),
      GammaCorrect(static_cast<visual>(185.0 / 255.0)),
      GammaCorrect(static_cast<visual>(186.0 / 255.0)),
      GammaCorrect(static_cast<visual>(187.0 / 255.0)),
      GammaCorrect(static_cast<visual>(188.0 / 255.0)),
      GammaCorrect(static_cast<visual>(189.0 / 255.0)),
      GammaCorrect(static_cast<visual>(190.0 / 255.0)),
      GammaCorrect(static_cast<visual>(191.0 / 255.0)),
      GammaCorrect(static_cast<visual>(192.0 / 255.0)),
      GammaCorrect(static_cast<visual>(193.0 / 255.0)),
      GammaCorrect(static_cast<visual>(194.0 / 255.0)),
      GammaCorrect(static_cast<visual>(195.0 / 255.0)),
      GammaCorrect(static_cast<visual>(196.0 / 255.0)),
      GammaCorrect(static_cast<visual>(197.0 / 255.0)),
      GammaCorrect(static_cast<visual>(198.0 / 255.0)),
      GammaCorrect(static_cast<visual>(199.0 / 255.0)),
      GammaCorrect(static_cast<visual>(200.0 / 255.0)),
      GammaCorrect(static_cast<visual>(201.0 / 255.0)),
      GammaCorrect(static_cast<visual>(202.0 / 255.0)),
      GammaCorrect(static_cast<visual>(203.0 / 255.0)),
      GammaCorrect(static_cast<visual>(204.0 / 255.0)),
      GammaCorrect(static_cast<visual>(205.0 / 255.0)),
      GammaCorrect(static_cast<visual>(206.0 / 255.0)),
      GammaCorrect(static_cast<visual>(207.0 / 255.0)),
      GammaCorrect(static_cast<visual>(208.0 / 255.0)),
      GammaCorrect(static_cast<visual>(209.0 / 255.0)),
      GammaCorrect(static_cast<visual>(210.0 / 255.0)),
      GammaCorrect(static_cast<visual>(211.0 / 255.0)),
      GammaCorrect(static_cast<visual>(212.0 / 255.0)),
      GammaCorrect(static_cast<visual>(213.0 / 255.0)),
      GammaCorrect(static_cast<visual>(214.0 / 255.0)),
      GammaCorrect(static_cast<visual>(215.0 / 255.0)),
      GammaCorrect(static_cast<visual>(216.0 / 255.0)),
      GammaCorrect(static_cast<visual>(217.0 / 255.0)),
      GammaCorrect(static_cast<visual>(218.0 / 255.0)),
      GammaCorrect(static_cast<visual>(219.0 / 255.0)),
      GammaCorrect(static_cast<visual>(220.0 / 255.0)),
      GammaCorrect(static_cast<visual>(221.0 / 255.0)),
      GammaCorrect(static_cast<visual>(222.0 / 255.0)),
      GammaCorrect(static_cast<visual>(223.0 / 255.0)),
      GammaCorrect(static_cast<visual>(224.0 / 255.0)),
      GammaCorrect(static_cast<visual>(225.0 / 255.0)),
      GammaCorrect(static_cast<visual>(226.0 / 255.0)),
      GammaCorrect(static_cast<visual>(227.0 / 255.0)),
      GammaCorrect(static_cast<visual>(228.0 / 255.0)),
      GammaCorrect(static_cast<visual>(229.0 / 255.0)),
      GammaCorrect(static_cast<visual>(230.0 / 255.0)),
      GammaCorrect(static_cast<visual>(231.0 / 255.0)),
      GammaCorrect(static_cast<visual>(232.0 / 255.0)),
      GammaCorrect(static_cast<visual>(233.0 / 255.0)),
      GammaCorrect(static_cast<visual>(234.0 / 255.0)),
      GammaCorrect(static_cast<visual>(235.0 / 255.0)),
      GammaCorrect(static_cast<visual>(236.0 / 255.0)),
      GammaCorrect(static_cast<visual>(237.0 / 255.0)),
      GammaCorrect(static_cast<visual>(238.0 / 255.0)),
      GammaCorrect(static_cast<visual>(239.0 / 255.0)),
      GammaCorrect(static_cast<visual>(240.0 / 255.0)),
      GammaCorrect(static_cast<visual>(241.0 / 255.0)),
      GammaCorrect(static_cast<visual>(242.0 / 255.0)),
      GammaCorrect(static_cast<visual>(243.0 / 255.0)),
      GammaCorrect(static_cast<visual>(244.0 / 255.0)),
      GammaCorrect(static_cast<visual>(245.0 / 255.0)),
      GammaCorrect(static_cast<visual>(246.0 / 255.0)),
      GammaCorrect(static_cast<visual>(247.0 / 255.0)),
      GammaCorrect(static_cast<visual>(248.0 / 255.0)),
      GammaCorrect(static_cast<visual>(249.0 / 255.0)),
      GammaCorrect(static_cast<visual>(250.0 / 255.0)),
      GammaCorrect(static_cast<visual>(251.0 / 255.0)),
      GammaCorrect(static_cast<visual>(252.0 / 255.0)),
      GammaCorrect(static_cast<visual>(253.0 / 255.0)),
      GammaCorrect(static_cast<visual>(254.0 / 255.0)),
      GammaCorrect(static_cast<visual>(255.0 / 255.0))};

  static const std::array<visual, 256> linear_lut = {
      static_cast<visual>(0.0 / 255.0),   static_cast<visual>(1.0 / 255.0),
      static_cast<visual>(2.0 / 255.0),   static_cast<visual>(3.0 / 255.0),
      static_cast<visual>(4.0 / 255.0),   static_cast<visual>(5.0 / 255.0),
      static_cast<visual>(6.0 / 255.0),   static_cast<visual>(7.0 / 255.0),
      static_cast<visual>(8.0 / 255.0),   static_cast<visual>(9.0 / 255.0),
      static_cast<visual>(10.0 / 255.0),  static_cast<visual>(11.0 / 255.0),
      static_cast<visual>(12.0 / 255.0),  static_cast<visual>(13.0 / 255.0),
      static_cast<visual>(14.0 / 255.0),  static_cast<visual>(15.0 / 255.0),
      static_cast<visual>(16.0 / 255.0),  static_cast<visual>(17.0 / 255.0),
      static_cast<visual>(18.0 / 255.0),  static_cast<visual>(19.0 / 255.0),
      static_cast<visual>(20.0 / 255.0),  static_cast<visual>(21.0 / 255.0),
      static_cast<visual>(22.0 / 255.0),  static_cast<visual>(23.0 / 255.0),
      static_cast<visual>(24.0 / 255.0),  static_cast<visual>(25.0 / 255.0),
      static_cast<visual>(26.0 / 255.0),  static_cast<visual>(27.0 / 255.0),
      static_cast<visual>(28.0 / 255.0),  static_cast<visual>(29.0 / 255.0),
      static_cast<visual>(30.0 / 255.0),  static_cast<visual>(31.0 / 255.0),
      static_cast<visual>(32.0 / 255.0),  static_cast<visual>(33.0 / 255.0),
      static_cast<visual>(34.0 / 255.0),  static_cast<visual>(35.0 / 255.0),
      static_cast<visual>(36.0 / 255.0),  static_cast<visual>(37.0 / 255.0),
      static_cast<visual>(38.0 / 255.0),  static_cast<visual>(39.0 / 255.0),
      static_cast<visual>(40.0 / 255.0),  static_cast<visual>(41.0 / 255.0),
      static_cast<visual>(42.0 / 255.0),  static_cast<visual>(43.0 / 255.0),
      static_cast<visual>(44.0 / 255.0),  static_cast<visual>(45.0 / 255.0),
      static_cast<visual>(46.0 / 255.0),  static_cast<visual>(47.0 / 255.0),
      static_cast<visual>(48.0 / 255.0),  static_cast<visual>(49.0 / 255.0),
      static_cast<visual>(50.0 / 255.0),  static_cast<visual>(51.0 / 255.0),
      static_cast<visual>(52.0 / 255.0),  static_cast<visual>(53.0 / 255.0),
      static_cast<visual>(54.0 / 255.0),  static_cast<visual>(55.0 / 255.0),
      static_cast<visual>(56.0 / 255.0),  static_cast<visual>(57.0 / 255.0),
      static_cast<visual>(58.0 / 255.0),  static_cast<visual>(59.0 / 255.0),
      static_cast<visual>(60.0 / 255.0),  static_cast<visual>(61.0 / 255.0),
      static_cast<visual>(62.0 / 255.0),  static_cast<visual>(63.0 / 255.0),
      static_cast<visual>(64.0 / 255.0),  static_cast<visual>(65.0 / 255.0),
      static_cast<visual>(66.0 / 255.0),  static_cast<visual>(67.0 / 255.0),
      static_cast<visual>(68.0 / 255.0),  static_cast<visual>(69.0 / 255.0),
      static_cast<visual>(70.0 / 255.0),  static_cast<visual>(71.0 / 255.0),
      static_cast<visual>(72.0 / 255.0),  static_cast<visual>(73.0 / 255.0),
      static_cast<visual>(74.0 / 255.0),  static_cast<visual>(75.0 / 255.0),
      static_cast<visual>(76.0 / 255.0),  static_cast<visual>(77.0 / 255.0),
      static_cast<visual>(78.0 / 255.0),  static_cast<visual>(79.0 / 255.0),
      static_cast<visual>(80.0 / 255.0),  static_cast<visual>(81.0 / 255.0),
      static_cast<visual>(82.0 / 255.0),  static_cast<visual>(83.0 / 255.0),
      static_cast<visual>(84.0 / 255.0),  static_cast<visual>(85.0 / 255.0),
      static_cast<visual>(86.0 / 255.0),  static_cast<visual>(87.0 / 255.0),
      static_cast<visual>(88.0 / 255.0),  static_cast<visual>(89.0 / 255.0),
      static_cast<visual>(90.0 / 255.0),  static_cast<visual>(91.0 / 255.0),
      static_cast<visual>(92.0 / 255.0),  static_cast<visual>(93.0 / 255.0),
      static_cast<visual>(94.0 / 255.0),  static_cast<visual>(95.0 / 255.0),
      static_cast<visual>(96.0 / 255.0),  static_cast<visual>(97.0 / 255.0),
      static_cast<visual>(98.0 / 255.0),  static_cast<visual>(99.0 / 255.0),
      static_cast<visual>(100.0 / 255.0), static_cast<visual>(101.0 / 255.0),
      static_cast<visual>(102.0 / 255.0), static_cast<visual>(103.0 / 255.0),
      static_cast<visual>(104.0 / 255.0), static_cast<visual>(105.0 / 255.0),
      static_cast<visual>(106.0 / 255.0), static_cast<visual>(107.0 / 255.0),
      static_cast<visual>(108.0 / 255.0), static_cast<visual>(109.0 / 255.0),
      static_cast<visual>(110.0 / 255.0), static_cast<visual>(111.0 / 255.0),
      static_cast<visual>(112.0 / 255.0), static_cast<visual>(113.0 / 255.0),
      static_cast<visual>(114.0 / 255.0), static_cast<visual>(115.0 / 255.0),
      static_cast<visual>(116.0 / 255.0), static_cast<visual>(117.0 / 255.0),
      static_cast<visual>(118.0 / 255.0), static_cast<visual>(119.0 / 255.0),
      static_cast<visual>(120.0 / 255.0), static_cast<visual>(121.0 / 255.0),
      static_cast<visual>(122.0 / 255.0), static_cast<visual>(123.0 / 255.0),
      static_cast<visual>(124.0 / 255.0), static_cast<visual>(125.0 / 255.0),
      static_cast<visual>(126.0 / 255.0), static_cast<visual>(127.0 / 255.0),
      static_cast<visual>(128.0 / 255.0), static_cast<visual>(129.0 / 255.0),
      static_cast<visual>(130.0 / 255.0), static_cast<visual>(131.0 / 255.0),
      static_cast<visual>(132.0 / 255.0), static_cast<visual>(133.0 / 255.0),
      static_cast<visual>(134.0 / 255.0), static_cast<visual>(135.0 / 255.0),
      static_cast<visual>(136.0 / 255.0), static_cast<visual>(137.0 / 255.0),
      static_cast<visual>(138.0 / 255.0), static_cast<visual>(139.0 / 255.0),
      static_cast<visual>(140.0 / 255.0), static_cast<visual>(141.0 / 255.0),
      static_cast<visual>(142.0 / 255.0), static_cast<visual>(143.0 / 255.0),
      static_cast<visual>(144.0 / 255.0), static_cast<visual>(145.0 / 255.0),
      static_cast<visual>(146.0 / 255.0), static_cast<visual>(147.0 / 255.0),
      static_cast<visual>(148.0 / 255.0), static_cast<visual>(149.0 / 255.0),
      static_cast<visual>(150.0 / 255.0), static_cast<visual>(151.0 / 255.0),
      static_cast<visual>(152.0 / 255.0), static_cast<visual>(153.0 / 255.0),
      static_cast<visual>(154.0 / 255.0), static_cast<visual>(155.0 / 255.0),
      static_cast<visual>(156.0 / 255.0), static_cast<visual>(157.0 / 255.0),
      static_cast<visual>(158.0 / 255.0), static_cast<visual>(159.0 / 255.0),
      static_cast<visual>(160.0 / 255.0), static_cast<visual>(161.0 / 255.0),
      static_cast<visual>(162.0 / 255.0), static_cast<visual>(163.0 / 255.0),
      static_cast<visual>(164.0 / 255.0), static_cast<visual>(165.0 / 255.0),
      static_cast<visual>(166.0 / 255.0), static_cast<visual>(167.0 / 255.0),
      static_cast<visual>(168.0 / 255.0), static_cast<visual>(169.0 / 255.0),
      static_cast<visual>(170.0 / 255.0), static_cast<visual>(171.0 / 255.0),
      static_cast<visual>(172.0 / 255.0), static_cast<visual>(173.0 / 255.0),
      static_cast<visual>(174.0 / 255.0), static_cast<visual>(175.0 / 255.0),
      static_cast<visual>(176.0 / 255.0), static_cast<visual>(177.0 / 255.0),
      static_cast<visual>(178.0 / 255.0), static_cast<visual>(179.0 / 255.0),
      static_cast<visual>(180.0 / 255.0), static_cast<visual>(181.0 / 255.0),
      static_cast<visual>(182.0 / 255.0), static_cast<visual>(183.0 / 255.0),
      static_cast<visual>(184.0 / 255.0), static_cast<visual>(185.0 / 255.0),
      static_cast<visual>(186.0 / 255.0), static_cast<visual>(187.0 / 255.0),
      static_cast<visual>(188.0 / 255.0), static_cast<visual>(189.0 / 255.0),
      static_cast<visual>(190.0 / 255.0), static_cast<visual>(191.0 / 255.0),
      static_cast<visual>(192.0 / 255.0), static_cast<visual>(193.0 / 255.0),
      static_cast<visual>(194.0 / 255.0), static_cast<visual>(195.0 / 255.0),
      static_cast<visual>(196.0 / 255.0), static_cast<visual>(197.0 / 255.0),
      static_cast<visual>(198.0 / 255.0), static_cast<visual>(199.0 / 255.0),
      static_cast<visual>(200.0 / 255.0), static_cast<visual>(201.0 / 255.0),
      static_cast<visual>(202.0 / 255.0), static_cast<visual>(203.0 / 255.0),
      static_cast<visual>(204.0 / 255.0), static_cast<visual>(205.0 / 255.0),
      static_cast<visual>(206.0 / 255.0), static_cast<visual>(207.0 / 255.0),
      static_cast<visual>(208.0 / 255.0), static_cast<visual>(209.0 / 255.0),
      static_cast<visual>(210.0 / 255.0), static_cast<visual>(211.0 / 255.0),
      static_cast<visual>(212.0 / 255.0), static_cast<visual>(213.0 / 255.0),
      static_cast<visual>(214.0 / 255.0), static_cast<visual>(215.0 / 255.0),
      static_cast<visual>(216.0 / 255.0), static_cast<visual>(217.0 / 255.0),
      static_cast<visual>(218.0 / 255.0), static_cast<visual>(219.0 / 255.0),
      static_cast<visual>(220.0 / 255.0), static_cast<visual>(221.0 / 255.0),
      static_cast<visual>(222.0 / 255.0), static_cast<visual>(223.0 / 255.0),
      static_cast<visual>(224.0 / 255.0), static_cast<visual>(225.0 / 255.0),
      static_cast<visual>(226.0 / 255.0), static_cast<visual>(227.0 / 255.0),
      static_cast<visual>(228.0 / 255.0), static_cast<visual>(229.0 / 255.0),
      static_cast<visual>(230.0 / 255.0), static_cast<visual>(231.0 / 255.0),
      static_cast<visual>(232.0 / 255.0), static_cast<visual>(233.0 / 255.0),
      static_cast<visual>(234.0 / 255.0), static_cast<visual>(235.0 / 255.0),
      static_cast<visual>(236.0 / 255.0), static_cast<visual>(237.0 / 255.0),
      static_cast<visual>(238.0 / 255.0), static_cast<visual>(239.0 / 255.0),
      static_cast<visual>(240.0 / 255.0), static_cast<visual>(241.0 / 255.0),
      static_cast<visual>(242.0 / 255.0), static_cast<visual>(243.0 / 255.0),
      static_cast<visual>(244.0 / 255.0), static_cast<visual>(245.0 / 255.0),
      static_cast<visual>(246.0 / 255.0), static_cast<visual>(247.0 / 255.0),
      static_cast<visual>(248.0 / 255.0), static_cast<visual>(249.0 / 255.0),
      static_cast<visual>(250.0 / 255.0), static_cast<visual>(251.0 / 255.0),
      static_cast<visual>(252.0 / 255.0), static_cast<visual>(253.0 / 255.0),
      static_cast<visual>(254.0 / 255.0), static_cast<visual>(255.0 / 255.0)};

  return gamma_correct ? gamma_correct_lut : linear_lut;
}

uint32_t ToKey(stbi_uc r, stbi_uc g, stbi_uc b) {
  uint32_t r32 = r;
  uint32_t g32 = g;
  uint32_t b32 = b;
  return r32 | (g32 << 8u) | (b32 << 16u);
}

stbi_uc* ReadFileWithStbi(const std::filesystem::path& filename, int* nx,
                          int* ny, int* num_channels, int desired_channels) {
  FILE* file = std::fopen(filename.native().c_str(), "r");
  if (!file) {
    std::cerr << "ERROR: Image loading failed with error: "
              << std::strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  stbi_uc* values =
      stbi_load_from_file(file, nx, ny, num_channels, desired_channels);
  if (!values) {
    std::cerr << "ERROR: Image loading failed with error: "
              << stbi_failure_reason() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::fclose(file);

  return values;
}

}  // namespace

std::shared_ptr<iris::textures::Image2D<visual>>
ImageManager::LoadFloatImageFromSDR(const std::filesystem::path& filename,
                                    bool gamma_correct) {
  auto& result = gamma_correct
                     ? gamma_corrected_float_images_[filename.native()]
                     : float_images_[filename.native()];
  if (result) {
    return result;
  }

  int nx, ny;
  stbi_uc* values =
      ReadFileWithStbi(filename.native().c_str(), &nx, &ny, nullptr,
                       /*desired_channels=*/1);

  const std::array<visual, 256>& float_lut = FloatLUT(gamma_correct);

  std::vector<visual> float_values;
  float_values.reserve(nx * ny);
  for (int y = 0; y < ny; y++) {
    for (int x = 0; x < nx; x++) {
      float_values.push_back(float_lut[values[(ny - y - 1) * nx + x]]);
    }
  }

  stbi_image_free(values);

  result = std::make_shared<iris::textures::Image2D<visual>>(
      std::move(float_values),
      std::make_pair(static_cast<size_t>(ny), static_cast<size_t>(nx)));

  return result;
}

std::shared_ptr<iris::textures::Image2D<ReferenceCounted<Reflector>>>
ImageManager::LoadReflectorImageFromSDR(const std::filesystem::path& filename,
                                        bool gamma_correct) {
  auto& result = gamma_correct
                     ? gamma_corrected_reflector_images_[filename.native()]
                     : reflector_images_[filename.native()];
  if (result) {
    return result;
  }

  int nx, ny, num_channels;
  stbi_uc* values =
      ReadFileWithStbi(filename.native().c_str(), &nx, &ny, &num_channels,
                       /*desired_channels=*/3);

  const std::array<visual, 256>& float_lut = FloatLUT(gamma_correct);

  std::vector<ReferenceCounted<Reflector>> reflector_values;
  reflector_values.reserve(nx * ny);

  if (num_channels == 1) {
    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {
        reflector_values.push_back(texture_manager_.AllocateUniformReflector(
            float_lut[values[(ny - y - 1) * nx + x]]));
      }
    }
  } else {
    std::unordered_map<uint32_t, ReferenceCounted<Reflector>>& reflector_map =
        gamma_correct ? gamma_corrected_reflectors_ : reflectors_;
    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {
        stbi_uc r = values[3 * ((ny - y - 1) * nx + x) + 0];
        stbi_uc g = values[3 * ((ny - y - 1) * nx + x) + 1];
        stbi_uc b = values[3 * ((ny - y - 1) * nx + x) + 2];
        uint32_t key = ToKey(r, g, b);

        ReferenceCounted<Reflector>& reflector = reflector_map[key];
        if (!reflector) {
          Color color({float_lut[r], float_lut[g], float_lut[g]}, Color::RGB);
          reflector = spectrum_manager_.AllocateReflector(color);
        }

        reflector_values.push_back(reflector);
      }
    }
  }

  stbi_image_free(values);

  result =
      std::make_shared<iris::textures::Image2D<ReferenceCounted<Reflector>>>(
          std::move(reflector_values),
          std::make_pair(static_cast<size_t>(ny), static_cast<size_t>(nx)));

  return result;
}

}  // namespace iris::pbrt_frontend