#include "iris/reference_counted.h"

#include "googletest/include/gtest/gtest.h"

namespace iris {
namespace {

const int kValue = 1337;

class Sharable : public ReferenceCountable {
 public:
  Sharable(const bool* allow_deletion, bool* deleted)
      : allow_deletion_(allow_deletion), deleted_(deleted) {
    *deleted_ = false;
  }

  ~Sharable() {
    EXPECT_TRUE(*allow_deletion_);
    *deleted_ = true;
  }

  int GetValue() const { return kValue; }

 private:
  const bool* allow_deletion_;
  bool* deleted_;
};

std::unique_ptr<ReferenceCounted<Sharable>> MakeSharable(
    const bool* allow_deletion, bool* deleted) {
  return std::make_unique<ReferenceCounted<Sharable>>(
      MakeReferenceCounted<Sharable>(allow_deletion, deleted));
}

class Derived : public Sharable {
 public:
  Derived(const bool* allow_deletion, bool* deleted)
      : Sharable(allow_deletion, deleted) {}
};

std::unique_ptr<ReferenceCounted<Derived>> MakeDerived(
    const bool* allow_deletion, bool* deleted) {
  return std::make_unique<ReferenceCounted<Derived>>(
      MakeReferenceCounted<Derived>(allow_deletion, deleted));
}

TEST(ReferenceCountedTest, Empty) {
  ReferenceCounted<Sharable> ptr;
  EXPECT_EQ(nullptr, ptr.Get());
}

TEST(ReferenceCountedTest, Construct) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Sharable>> ptr =
      MakeSharable(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  allow_deletion = true;
  ptr.reset();
  EXPECT_TRUE(deleted);
}

TEST(ReferenceCountedTest, CopyConstruct) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Sharable>> ptr =
      MakeSharable(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  std::unique_ptr<ReferenceCounted<Sharable>> ptr2 =
      std::make_unique<ReferenceCounted<Sharable>>(*ptr);
  ptr.reset();
  EXPECT_FALSE(deleted);
  allow_deletion = true;
  ptr2.reset();
  EXPECT_TRUE(deleted);
}

TEST(ReferenceCountedTest, MoveConstruct) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Sharable>> ptr =
      MakeSharable(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  std::unique_ptr<ReferenceCounted<Sharable>> ptr2 =
      std::make_unique<ReferenceCounted<Sharable>>(std::move(*ptr));
  allow_deletion = true;
  ptr2.reset();
  EXPECT_TRUE(deleted);
  ptr.reset();
}

TEST(ReferenceCountedTest, DoesNotIncrementEmpty) {
  ReferenceCounted<Sharable> empty;
  ReferenceCounted<Sharable> empty_copy(empty);
  ReferenceCounted<Sharable> moved(std::move(empty));
  empty = empty_copy;
  empty_copy = std::move(moved);
  EXPECT_EQ(nullptr, empty.Get());
  EXPECT_EQ(nullptr, empty_copy.Get());
}

TEST(ReferenceCountedTest, CopyUpcast) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Derived>> ptr =
      MakeDerived(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  std::unique_ptr<ReferenceCounted<Sharable>> ptr2 =
      std::make_unique<ReferenceCounted<Sharable>>(*ptr);
  ptr.reset();
  EXPECT_FALSE(deleted);
  allow_deletion = true;
  ptr2.reset();
  EXPECT_TRUE(deleted);
}

TEST(ReferenceCountedTest, MoveUpcast) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Derived>> ptr =
      MakeDerived(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  std::unique_ptr<ReferenceCounted<Sharable>> ptr2 =
      std::make_unique<ReferenceCounted<Sharable>>(std::move(*ptr));
  allow_deletion = true;
  ptr2.reset();
  EXPECT_TRUE(deleted);
  ptr.reset();
}

TEST(ReferenceCountedTest, CopyAssign) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Sharable>> ptr =
      MakeSharable(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  std::unique_ptr<ReferenceCounted<Sharable>> ptr2 =
      std::make_unique<ReferenceCounted<Sharable>>();
  *ptr2 = *ptr;
  ptr.reset();
  EXPECT_FALSE(deleted);
  allow_deletion = true;
  ptr2.reset();
  EXPECT_TRUE(deleted);
}

TEST(ReferenceCountedTest, MoveAssign) {
  bool allow_deletion = false, deleted = false;
  std::unique_ptr<ReferenceCounted<Sharable>> ptr =
      MakeSharable(&allow_deletion, &deleted);
  EXPECT_FALSE(deleted);
  std::unique_ptr<ReferenceCounted<Sharable>> ptr2 =
      std::make_unique<ReferenceCounted<Sharable>>();
  *ptr2 = std::move(*ptr);
  allow_deletion = true;
  ptr2.reset();
  EXPECT_TRUE(deleted);
  ptr.reset();
}

TEST(ReferenceCountedTest, OperatorBool) {
  ReferenceCounted<Sharable> empty;
  EXPECT_FALSE(empty);

  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  EXPECT_TRUE(ptr);
}

TEST(ReferenceCountedTest, OperatorArrow) {
  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  EXPECT_TRUE(ptr);
  EXPECT_EQ(kValue, ptr->GetValue());
}

TEST(ReferenceCountedTest, OperatorDereference) {
  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  EXPECT_TRUE(ptr);
  EXPECT_EQ(kValue, (*ptr).GetValue());
}

TEST(ReferenceCountedTest, Swap) {
  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr0 =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  const Sharable* raw0 = ptr0.Get();
  ReferenceCounted<Sharable> ptr1 =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  const Sharable* raw1 = ptr1.Get();
  std::swap(ptr0, ptr1);
  EXPECT_EQ(raw0, ptr1.Get());
  EXPECT_EQ(raw1, ptr0.Get());
}

TEST(ReferenceCountedTest, Reset) {
  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  EXPECT_NE(nullptr, ptr.Get());
  ptr.Reset();
  EXPECT_EQ(nullptr, ptr.Get());
  EXPECT_TRUE(deleted);
}

TEST(ReferenceCountedTest, Compare) {
  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr0 =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  ReferenceCounted<Sharable> ptr1 = ptr0;
  EXPECT_EQ(ptr0, ptr1);
  EXPECT_LE(ptr0, ptr1);
  EXPECT_GE(ptr0, ptr1);
  EXPECT_FALSE(ptr0 < ptr1);
  EXPECT_FALSE(ptr0 > ptr1);

  ReferenceCounted<Sharable> ptr2 =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  EXPECT_NE(ptr0, ptr2);
  EXPECT_TRUE(ptr0 < ptr2 || ptr1 > ptr2);
  EXPECT_FALSE(ptr0 < ptr2 && ptr1 > ptr2);
  EXPECT_TRUE(ptr0 <= ptr2 || ptr1 >= ptr2);
  EXPECT_FALSE(ptr0 <= ptr2 && ptr1 >= ptr2);
}

TEST(ReferenceCountedTest, Hash) {
  std::hash<ReferenceCounted<Sharable>> hasher;

  bool allow_deletion = true, deleted = false;
  ReferenceCounted<Sharable> ptr0 =
      MakeReferenceCounted<Sharable>(&allow_deletion, &deleted);
  ReferenceCounted<Sharable> empty;
  EXPECT_NE(hasher(empty), hasher(ptr0));

  ReferenceCounted<Sharable> ptr1 = ptr0;
  EXPECT_EQ(hasher(ptr0), hasher(ptr1));
}

}  // namespace
}  // namespace iris
