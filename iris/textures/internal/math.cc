#include "iris/textures/internal/math.h"

#include <algorithm>
#include <cstdint>

#include "iris/float.h"
#include "iris/point.h"
#include "iris/vector.h"

namespace iris {
namespace textures {
namespace internal {
namespace {

constexpr uint8_t perlin_noise_data[512] = {
    151u, 160u, 137u, 91u,  90u,  15u,  131u, 13u,  201u, 95u,  96u,  53u,
    194u, 233u, 7u,   225u, 140u, 36u,  103u, 30u,  69u,  142u, 8u,   99u,
    37u,  240u, 21u,  10u,  23u,  190u, 6u,   148u, 247u, 120u, 234u, 75u,
    0u,   26u,  197u, 62u,  94u,  252u, 219u, 203u, 117u, 35u,  11u,  32u,
    57u,  177u, 33u,  88u,  237u, 149u, 56u,  87u,  174u, 20u,  125u, 136u,
    171u, 168u, 68u,  175u, 74u,  165u, 71u,  134u, 139u, 48u,  27u,  166u,
    77u,  146u, 158u, 231u, 83u,  111u, 229u, 122u, 60u,  211u, 133u, 230u,
    220u, 105u, 92u,  41u,  55u,  46u,  245u, 40u,  244u, 102u, 143u, 54u,
    65u,  25u,  63u,  161u, 1u,   216u, 80u,  73u,  209u, 76u,  132u, 187u,
    208u, 89u,  18u,  169u, 200u, 196u, 135u, 130u, 116u, 188u, 159u, 86u,
    164u, 100u, 109u, 198u, 173u, 186u, 3u,   64u,  52u,  217u, 226u, 250u,
    124u, 123u, 5u,   202u, 38u,  147u, 118u, 126u, 255u, 82u,  85u,  212u,
    207u, 206u, 59u,  227u, 47u,  16u,  58u,  17u,  182u, 189u, 28u,  42u,
    223u, 183u, 170u, 213u, 119u, 248u, 152u, 2u,   44u,  154u, 163u, 70u,
    221u, 153u, 101u, 155u, 167u, 43u,  172u, 9u,   129u, 22u,  39u,  253u,
    19u,  98u,  108u, 110u, 79u,  113u, 224u, 232u, 178u, 185u, 112u, 104u,
    218u, 246u, 97u,  228u, 251u, 34u,  242u, 193u, 238u, 210u, 144u, 12u,
    191u, 179u, 162u, 241u, 81u,  51u,  145u, 235u, 249u, 14u,  239u, 107u,
    49u,  192u, 214u, 31u,  181u, 199u, 106u, 157u, 184u, 84u,  204u, 176u,
    115u, 121u, 50u,  45u,  127u, 4u,   150u, 254u, 138u, 236u, 205u, 93u,
    222u, 114u, 67u,  29u,  24u,  72u,  243u, 141u, 128u, 195u, 78u,  66u,
    215u, 61u,  156u, 180u, 151u, 160u, 137u, 91u,  90u,  15u,  131u, 13u,
    201u, 95u,  96u,  53u,  194u, 233u, 7u,   225u, 140u, 36u,  103u, 30u,
    69u,  142u, 8u,   99u,  37u,  240u, 21u,  10u,  23u,  190u, 6u,   148u,
    247u, 120u, 234u, 75u,  0u,   26u,  197u, 62u,  94u,  252u, 219u, 203u,
    117u, 35u,  11u,  32u,  57u,  177u, 33u,  88u,  237u, 149u, 56u,  87u,
    174u, 20u,  125u, 136u, 171u, 168u, 68u,  175u, 74u,  165u, 71u,  134u,
    139u, 48u,  27u,  166u, 77u,  146u, 158u, 231u, 83u,  111u, 229u, 122u,
    60u,  211u, 133u, 230u, 220u, 105u, 92u,  41u,  55u,  46u,  245u, 40u,
    244u, 102u, 143u, 54u,  65u,  25u,  63u,  161u, 1u,   216u, 80u,  73u,
    209u, 76u,  132u, 187u, 208u, 89u,  18u,  169u, 200u, 196u, 135u, 130u,
    116u, 188u, 159u, 86u,  164u, 100u, 109u, 198u, 173u, 186u, 3u,   64u,
    52u,  217u, 226u, 250u, 124u, 123u, 5u,   202u, 38u,  147u, 118u, 126u,
    255u, 82u,  85u,  212u, 207u, 206u, 59u,  227u, 47u,  16u,  58u,  17u,
    182u, 189u, 28u,  42u,  223u, 183u, 170u, 213u, 119u, 248u, 152u, 2u,
    44u,  154u, 163u, 70u,  221u, 153u, 101u, 155u, 167u, 43u,  172u, 9u,
    129u, 22u,  39u,  253u, 19u,  98u,  108u, 110u, 79u,  113u, 224u, 232u,
    178u, 185u, 112u, 104u, 218u, 246u, 97u,  228u, 251u, 34u,  242u, 193u,
    238u, 210u, 144u, 12u,  191u, 179u, 162u, 241u, 81u,  51u,  145u, 235u,
    249u, 14u,  239u, 107u, 49u,  192u, 214u, 31u,  181u, 199u, 106u, 157u,
    184u, 84u,  204u, 176u, 115u, 121u, 50u,  45u,  127u, 4u,   150u, 254u,
    138u, 236u, 205u, 93u,  222u, 114u, 67u,  29u,  24u,  72u,  243u, 141u,
    128u, 195u, 78u,  66u,  215u, 61u,  156u, 180u};

visual_t PerlinNoiseGradient(uint8_t x, uint8_t y, uint8_t z, visual_t dx,
                             visual_t dy, visual_t dz) {
  uint8_t value = perlin_noise_data[x];
  value = perlin_noise_data[value + y];
  value = perlin_noise_data[value + z];
  value &= 15u;

  visual_t u = (value < 8u || value == 12u || value == 13u) ? dx : dy;
  u = (value & 1u) ? -u : u;

  visual_t v = (value < 4u || value == 12u || value == 13u) ? dy : dz;
  v = (value & 2u) ? -v : v;

  return u + v;
}

visual_t PerlinNoiseWeight(visual_t value) {
  visual_t value_2 = value * value;
  visual_t value_3 = value_2 * value;
  visual_t value_4 = value_2 * value_2;
  visual_t value_5 = value_2 * value_3;

  return static_cast<visual_t>(6.0) * value_5 -
         static_cast<visual_t>(15.0) * value_4 +
         static_cast<visual_t>(10.0) * value_3;
}

visual_t SmoothStep(visual_t min, visual_t max, visual_t value) {
  visual_t v =
      std::clamp((value - min) / (max - min), static_cast<visual_t>(0.0),
                 static_cast<visual_t>(1.0));
  return v * v * (static_cast<visual_t>(-1.0) * v + static_cast<visual_t>(3.0));
}

visual_t Log2(geometric_t value) {
  static constexpr visual_t inverse_log2 = 1.442695040888963387004650940071;
  return static_cast<visual_t>(std::log(value)) * inverse_log2;
}

}  // namespace

visual_t PerlinNoise(const Point& hit_point) {
  visual_t x = static_cast<visual_t>(
      std::fmod(hit_point.x, static_cast<geometric_t>(256.0)) +
      static_cast<geometric_t>(256.0));
  visual_t y = static_cast<visual_t>(
      std::fmod(hit_point.y, static_cast<geometric_t>(256.0)) +
      static_cast<geometric_t>(256.0));
  visual_t z = static_cast<visual_t>(
      std::fmod(hit_point.z, static_cast<geometric_t>(256.0)) +
      static_cast<geometric_t>(256.0));

  visual_t dx = static_cast<visual_t>(std::modf(x, &x));
  visual_t dy = static_cast<visual_t>(std::modf(y, &y));
  visual_t dz = static_cast<visual_t>(std::modf(z, &z));

  uint8_t ix = static_cast<uint8_t>(x);
  uint8_t iy = static_cast<uint8_t>(y);
  uint8_t iz = static_cast<uint8_t>(z);

  visual_t g000 = PerlinNoiseGradient(
      ix + 0u, iy + 0u, iz + 0u, dx - static_cast<visual_t>(0.0),
      dy - static_cast<visual_t>(0.0), dz - static_cast<visual_t>(0.0));
  visual_t g100 = PerlinNoiseGradient(
      ix + 1u, iy + 0u, iz + 0u, dx - static_cast<visual_t>(1.0),
      dy - static_cast<visual_t>(0.0), dz - static_cast<visual_t>(0.0));
  visual_t g010 = PerlinNoiseGradient(
      ix + 0u, iy + 1u, iz + 0u, dx - static_cast<visual_t>(0.0),
      dy - static_cast<visual_t>(1.0), dz - static_cast<visual_t>(0.0));
  visual_t g110 = PerlinNoiseGradient(
      ix + 1u, iy + 1u, iz + 0u, dx - static_cast<visual_t>(1.0),
      dy - static_cast<visual_t>(1.0), dz - static_cast<visual_t>(0.0));
  visual_t g001 = PerlinNoiseGradient(
      ix + 0u, iy + 0u, iz + 1u, dx - static_cast<visual_t>(0.0),
      dy - static_cast<visual_t>(0.0), dz - static_cast<visual_t>(1.0));
  visual_t g101 = PerlinNoiseGradient(
      ix + 1u, iy + 0u, iz + 1u, dx - static_cast<visual_t>(1.0),
      dy - static_cast<visual_t>(0.0), dz - static_cast<visual_t>(1.0));
  visual_t g011 = PerlinNoiseGradient(
      ix + 0u, iy + 1u, iz + 1u, dx - static_cast<visual_t>(0.0),
      dy - static_cast<visual_t>(1.0), dz - static_cast<visual_t>(1.0));
  visual_t g111 = PerlinNoiseGradient(
      ix + 1u, iy + 1u, iz + 1u, dx - static_cast<visual_t>(1.0),
      dy - static_cast<visual_t>(1.0), dz - static_cast<visual_t>(1.0));

  visual_t wx = PerlinNoiseWeight(dx);
  visual_t wy = PerlinNoiseWeight(dy);
  visual_t wz = PerlinNoiseWeight(dz);

  visual_t x00 = std::lerp(g000, g100, wx);
  visual_t x10 = std::lerp(g010, g110, wx);
  visual_t x01 = std::lerp(g001, g101, wx);
  visual_t x11 = std::lerp(g011, g111, wx);
  visual_t y0 = std::lerp(x00, x10, wy);
  visual_t y1 = std::lerp(x01, x11, wy);

  return std::lerp(y0, y1, wz);
}

visual_t FractionalBrownianMotion(const Point& hit_point,
                                  const Vector& dhit_point_dx,
                                  const Vector& dhit_point_dy, visual_t omega,
                                  uint8_t max_octaves) {
  geometric_t length_squared =
      std::max(DotProduct(dhit_point_dx, dhit_point_dx),
               DotProduct(dhit_point_dy, dhit_point_dy));

  visual_t n = std::clamp(static_cast<visual_t>(-1.0) -
                              static_cast<visual_t>(0.5) * Log2(length_squared),
                          static_cast<visual_t>(0.0),
                          static_cast<visual_t>(max_octaves));
  visual_t n_fractional = std::modf(n, &n);
  uint8_t n_integer = static_cast<uint8_t>(n);

  visual_t sum = static_cast<visual_t>(0.0);
  visual_t o = static_cast<visual_t>(1.0);
  geometric_t lambda = static_cast<geometric_t>(1.0);
  for (uint8_t i = 0; i < n_integer; i++) {
    sum += o * std::abs(PerlinNoise(ScalePoint(hit_point, lambda)));
    lambda *= static_cast<visual_t>(1.99);
    o *= omega;
  }

  sum += o * std::lerp(static_cast<visual_t>(0.2),
                       std::abs(PerlinNoise(ScalePoint(hit_point, lambda))),
                       SmoothStep(static_cast<visual_t>(0.3),
                                  static_cast<visual_t>(0.7), n_fractional));

  return sum;
}

visual_t Turbulence(const Point& hit_point, const Vector& dhit_point_dx,
                    const Vector& dhit_point_dy, visual_t omega,
                    uint8_t max_octaves) {
  geometric_t length_squared =
      std::max(DotProduct(dhit_point_dx, dhit_point_dx),
               DotProduct(dhit_point_dy, dhit_point_dy));

  visual_t n = std::clamp(static_cast<visual_t>(-1.0) -
                              static_cast<visual_t>(0.5) * Log2(length_squared),
                          static_cast<visual_t>(0.0),
                          static_cast<visual_t>(max_octaves));
  visual_t n_fractional = std::modf(n, &n);
  uint8_t n_integer = static_cast<uint8_t>(n);

  visual_t sum = static_cast<visual_t>(0.0);
  visual_t o = static_cast<visual_t>(1.0);
  geometric_t lambda = static_cast<geometric_t>(1.0);
  for (uint8_t i = 0; i < n_integer; i++) {
    sum += o * std::abs(PerlinNoise(ScalePoint(hit_point, lambda)));
    lambda *= static_cast<visual_t>(1.99);
    o *= omega;
  }

  sum += o * std::lerp(static_cast<visual_t>(0.2),
                       std::abs(PerlinNoise(ScalePoint(hit_point, lambda))),
                       SmoothStep(static_cast<visual_t>(0.3),
                                  static_cast<visual_t>(0.7), n_fractional));
  for (uint8_t i = n_integer; i < max_octaves; i++) {
    sum += o * static_cast<visual_t>(0.2);
    o *= omega;
  }

  return sum;
}

}  // namespace internal
}  // namespace textures
}  // namespace iris
