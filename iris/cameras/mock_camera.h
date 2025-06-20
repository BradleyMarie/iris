#ifndef _IRIS_CAMERA_MOCK_CAMERA_
#define _IRIS_CAMERA_MOCK_CAMERA_

#include <array>
#include <optional>

#include "googlemock/include/gmock/gmock.h"
#include "iris/camera.h"
#include "iris/float.h"
#include "iris/ray_differential.h"

namespace iris {
namespace cameras {

class MockCamera final : public Camera {
 public:
  MOCK_METHOD(RayDifferential, Compute,
              ((const std::array<geometric_t, 2>&),
               (const std::array<geometric_t, 2>&),
               (const std::optional<std::array<geometric_t, 2>>&)),
              (const override));
  MOCK_METHOD(bool, HasLens, (), (const override));
};

}  // namespace cameras
}  // namespace iris

#endif  // _IRIS_CAMERA_MOCK_CAMERA_
