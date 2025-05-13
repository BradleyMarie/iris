#ifndef _FRONTENDS_PBRT_MATRIX_MANAGER_
#define _FRONTENDS_PBRT_MATRIX_MANAGER_

#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

#include "iris/matrix.h"

namespace iris {
namespace pbrt_frontend {

class MatrixManager {
 public:
  MatrixManager();
  ~MatrixManager();

  // Types
  enum class ActiveTransformation { START, END, ALL };

  struct Transformation {
    Matrix start;
    Matrix end;
  };

  // Accessor
  const Transformation& Get();

  // Matrix Manipulation
  void Identity();
  void Translate(geometric x, geometric y, geometric z);
  void Scale(geometric x, geometric y, geometric z);
  void Rotate(geometric theta, geometric x, geometric y, geometric z);
  void LookAt(geometric eye_x, geometric eye_y, geometric eye_z,
              geometric look_x, geometric look_y, geometric look_z,
              geometric up_x, geometric up_y, geometric up_z);
  void CoordinateSystem(const std::string& name);
  void CoordSysTransform(const std::string& name);
  void Transform(geometric m00, geometric m01, geometric m02, geometric m03,
                 geometric m10, geometric m11, geometric m12, geometric m13,
                 geometric m20, geometric m21, geometric m22, geometric m23,
                 geometric m30, geometric m31, geometric m32, geometric m33);
  void ConcatTransform(geometric m00, geometric m01, geometric m02,
                       geometric m03, geometric m10, geometric m11,
                       geometric m12, geometric m13, geometric m20,
                       geometric m21, geometric m22, geometric m23,
                       geometric m30, geometric m31, geometric m32,
                       geometric m33);

  // Stack Manipulation
  void ActiveTransform(ActiveTransformation active_transform);
  void AttributeBegin();
  void AttributeEnd();
  void TransformBegin();
  void TransformEnd();

 private:
  enum class PushPopReason { ATTRIBUTE, TRANSFORM };

  void Push(PushPopReason reason);
  void Pop(PushPopReason reason);

  void Transform(const Matrix& m);
  void Set(const Matrix& m);

  std::stack<Transformation> transformation_stack_;
  std::stack<ActiveTransformation> active_transform_stack_;
  std::stack<PushPopReason> stack_manipulation_reasons_;
  std::unordered_map<std::string, std::pair<Matrix, Matrix>> coord_systems_;
};

}  // namespace pbrt_frontend
}  // namespace iris

#endif  // _FRONTENDS_PBRT_MATRIX_MANAGER_
