#include "bonelab/Array.hpp"
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class ArrayTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (ArrayTests, SimpleAssignment)
  {
  Array<2,double> A(Tuple<2,size_t>(3,4));
  A(2,3) = 4.0;
  A(1,2) = 3.0;
  ASSERT_EQ(A(2,3), 4.0);
  ASSERT_EQ(A(1,2), 3.0);
  }

TEST_F (ArrayTests, FlatIndexing)
  {
  Array<2,double> A(Tuple<2,size_t>(3,4));
  A(2,3) = 4.0;
  A(1,2) = 3.0;
  ASSERT_EQ(A[11], 4.0);
  ASSERT_EQ(A[6], 3.0);
  }

TEST_F (ArrayTests, PointerConstructor)
  {
  double a[] = {1.0,2.0,3.0,4.0,5.0,6.0};
  Array<2,double> A(a, Tuple<2,size_t>(2,3));
  ASSERT_EQ(A(0,2), 3.0);
  ASSERT_EQ(A(1,2), 6.0);
  }

TEST_F (ArrayTests, ReferenceConstructor)
  {
  double a[] = {1.0,2.0,3.0,4.0,5.0,6.0};
  Array<2,double> A(a, Tuple<2,size_t>(2,3));
  Array<2,double> B(A);
  ASSERT_EQ(B.size(), 6);
  ASSERT_EQ(B.dims(), (Tuple<2,size_t>(2,3)));
  ASSERT_EQ(A.ptr(), B.ptr());
  ASSERT_EQ(B(0,2), 3.0);
  ASSERT_EQ(B(1,2), 6.0);
  }

TEST_F (ArrayTests, Pointer)
  {
  Array<2,double> A(Tuple<2,size_t>(3,4));
  ASSERT_EQ(&(A(1,2)), A.ptr() + 6);
  }

TEST_F (ArrayTests, End)
  {
  Array<2,double> A(Tuple<2,size_t>(3,4));
  ASSERT_EQ(A.ptr() + 12, A.end());
  }

TEST_F (ArrayTests, Zero)
  {
  Array<2,double> A(Tuple<2,size_t>(3,4));
  A(2,3) = 4.0;
  A(1,2) = 3.0;
  A.zero();
  ASSERT_EQ(A(2,3), 0.0);
  ASSERT_EQ(A(1,2), 0.0);
  }

TEST_F (ArrayTests, Copy)
  {
  double a[] = {1.0,2.0,3.0,4.0,5.0,6.0};
  Array<2,double> A(a, Tuple<2,size_t>(2,3));
  Array<2,double> B(Tuple<2,size_t>(2,3));
  B.copy(A);
  ASSERT_EQ(B.size(), 6);
  ASSERT_NE(A.ptr(), B.ptr());
  ASSERT_EQ(B(0,2), A(0,2));
  ASSERT_EQ(B(1,2), A(1,2));
  }

TEST_F (ArrayTests, CopyPointer)
  {
  double a[] = {1.0,2.0,3.0,4.0,5.0,6.0};
  Array<2,double> A(Tuple<2,size_t>(2,3));
  A.copy(a);
  ASSERT_NE(A.ptr(), a);
  ASSERT_EQ(A(0,2),3.0);
  ASSERT_EQ(A(1,2),6.0);
  }

TEST_F (ArrayTests, SimpleAssignment3)
  {
  Array<3,double> A(Tuple<3,size_t>(2,3,4));
  A(1,2,3) = 4.0;
  A(0,1,2) = 3.0;
  ASSERT_EQ(A(1,2,3), 4.0);
  ASSERT_EQ(A(0,1,2), 3.0);
  }

TEST_F (ArrayTests, FlatIndexing3)
  {
  Array<3,double> A(Tuple<3,size_t>(2,3,4));
  A(1,2,3) = 4.0;
  A(0,1,2) = 3.0;
  ASSERT_EQ(A[23], 4.0);
  ASSERT_EQ(A[6], 3.0);
  }

