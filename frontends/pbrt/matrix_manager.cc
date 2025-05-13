#include "frontends/pbrt/matrix_manager.h"

#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numbers>
#include <unordered_map>

namespace iris {
namespace pbrt_frontend {

MatrixManager::MatrixManager() {
  transformation_stack_.emplace(Matrix::Identity(), Matrix::Identity());
  active_transform_stack_.emplace(ActiveTransformation::ALL);
}

MatrixManager::~MatrixManager() {
  if (!stack_manipulation_reasons_.empty()) {
    const char* message =
        (stack_manipulation_reasons_.top() == PushPopReason::ATTRIBUTE)
            ? "AttributeEnd"
            : "TransformEnd";
    std::cerr << "ERROR: Missing " << message << " directive" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void MatrixManager::Identity() { Set(Matrix::Identity()); }

void MatrixManager::Translate(geometric x, geometric y, geometric z) {
  Transform(Matrix::Translation(x, y, z).value());
}

void MatrixManager::Scale(geometric x, geometric y, geometric z) {
  if (x == 0.0 || y == 0.0 || z == 0.0) {
    std::cerr << "ERROR: Parameters to Scale must be non-zero" << std::endl;
    exit(EXIT_FAILURE);
  }

  Transform(Matrix::Scalar(x, y, z).value());
}

void MatrixManager::Rotate(geometric theta, geometric x, geometric y,
                           geometric z) {
  if (x == 0.0 && y == 0.0 && z == 0.0) {
    std::cerr
        << "ERROR: One of the x, y, or z parameters to Rotate must be non-zero"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  Transform(
      Matrix::Rotation(
          theta * static_cast<geometric_t>(std::numbers::pi / 180.0), x, y, z)
          .value());
}

void MatrixManager::LookAt(geometric eye_x, geometric eye_y, geometric eye_z,
                           geometric look_x, geometric look_y, geometric look_z,
                           geometric up_x, geometric up_y, geometric up_z) {
  if (eye_x == look_x && eye_y == look_y && eye_z == look_z) {
    std::cerr << "ERROR: eye and look_at parameters to LookAt must not be equal"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if (up_x == 0.0 && up_y == 0.0 && up_z == 0.0) {
    std::cerr << "ERROR: One of up_x, up_y, or up_z parameters to LookAt must "
                 "be non-zero"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  auto look_at = Matrix::LookAt(eye_x, eye_y, eye_z, look_x, look_y, look_z,
                                up_x, up_y, up_z);
  if (!look_at.has_value()) {
    std::cerr << "ERROR: Up parameter to LookAt must not be parallel with look "
                 "direction"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // This transformation is inverted in PBRT
  Transform(look_at.value().Inverse());
}

void MatrixManager::CoordinateSystem(const std::string& name) {
  coord_systems_.erase(name);
  coord_systems_.emplace(name, std::make_pair(transformation_stack_.top().start,
                                              transformation_stack_.top().end));
}

void MatrixManager::CoordSysTransform(const std::string& name) {
  auto iter = coord_systems_.find(name);
  if (iter == coord_systems_.end()) {
    std::cerr << "ERROR: Unknown coordinate system name specified for "
                 "directive CoordSysTransform: \""
              << name << "\"" << std::endl;
    exit(EXIT_FAILURE);
  }

  transformation_stack_.pop();
  transformation_stack_.emplace(iter->second.first, iter->second.second);
}

void MatrixManager::Transform(geometric m00, geometric m01, geometric m02,
                              geometric m03, geometric m10, geometric m11,
                              geometric m12, geometric m13, geometric m20,
                              geometric m21, geometric m22, geometric m23,
                              geometric m30, geometric m31, geometric m32,
                              geometric m33) {
  std::array<std::array<geometric, 4>, 4> values2d = {
      std::array<geometric, 4>{m00, m10, m20, m30},
      std::array<geometric, 4>{m01, m11, m21, m31},
      std::array<geometric, 4>{m02, m12, m22, m32},
      std::array<geometric, 4>{m03, m13, m23, m33}};

  auto matrix = Matrix::Create(values2d);
  if (!matrix.has_value()) {
    std::cerr << "ERROR: Uninvertible matrix specified for Transform directive"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  Set(matrix.value());
}

void MatrixManager::ConcatTransform(geometric m00, geometric m01, geometric m02,
                                    geometric m03, geometric m10, geometric m11,
                                    geometric m12, geometric m13, geometric m20,
                                    geometric m21, geometric m22, geometric m23,
                                    geometric m30, geometric m31, geometric m32,
                                    geometric m33) {
  std::array<std::array<geometric, 4>, 4> values2d = {
      std::array<geometric, 4>{m00, m10, m20, m30},
      std::array<geometric, 4>{m01, m11, m21, m31},
      std::array<geometric, 4>{m02, m12, m22, m32},
      std::array<geometric, 4>{m03, m13, m23, m33}};

  auto matrix = Matrix::Create(values2d);
  if (!matrix.has_value()) {
    std::cerr
        << "ERROR: Uninvertible matrix specified for ConcatTransform directive"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  Transform(matrix.value());
}

void MatrixManager::ActiveTransform(ActiveTransformation active_transform) {
  active_transform_stack_.top() = active_transform;
}

void MatrixManager::AttributeBegin() { Push(PushPopReason::ATTRIBUTE); }

void MatrixManager::AttributeEnd() { Pop(PushPopReason::ATTRIBUTE); }

void MatrixManager::TransformBegin() { Push(PushPopReason::TRANSFORM); }

void MatrixManager::TransformEnd() { Pop(PushPopReason::TRANSFORM); }

void MatrixManager::Transform(const Matrix& m) {
  Matrix start = (active_transform_stack_.top() != ActiveTransformation::END)
                     ? transformation_stack_.top().start.Multiply(m)
                     : transformation_stack_.top().start;

  Matrix end = (active_transform_stack_.top() != ActiveTransformation::START)
                   ? transformation_stack_.top().end.Multiply(m)
                   : transformation_stack_.top().end;

  transformation_stack_.pop();
  transformation_stack_.emplace(start, end);
}

void MatrixManager::Set(const Matrix& m) {
  Matrix start = (active_transform_stack_.top() != ActiveTransformation::END)
                     ? m
                     : transformation_stack_.top().start;

  Matrix end = (active_transform_stack_.top() != ActiveTransformation::START)
                   ? m
                   : transformation_stack_.top().end;

  transformation_stack_.pop();
  transformation_stack_.emplace(start, end);
}

const MatrixManager::Transformation& MatrixManager::Get() {
  return transformation_stack_.top();
}

void MatrixManager::Push(PushPopReason reason) {
  transformation_stack_.emplace(transformation_stack_.top());
  stack_manipulation_reasons_.push(reason);
}

void MatrixManager::Pop(PushPopReason reason) {
  if (stack_manipulation_reasons_.empty()) {
    const char* message = (reason == PushPopReason::ATTRIBUTE)
                              ? "AttributeBegin"
                              : "TransformBegin";
    std::cerr << "ERROR: Missing " << message << " directive" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (stack_manipulation_reasons_.top() != reason) {
    const char* message =
        (stack_manipulation_reasons_.top() == PushPopReason::ATTRIBUTE)
            ? "AttributeBegin and AttributeEnd"
            : "TransformBegin and TransformEnd";
    std::cerr << "ERROR: Mismatched " << message << " directives" << std::endl;
    exit(EXIT_FAILURE);
  }

  transformation_stack_.pop();
  stack_manipulation_reasons_.pop();
}

}  // namespace pbrt_frontend
}  // namespace iris
