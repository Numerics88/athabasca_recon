#include "bonelab/Tuple.hpp"
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class TupleTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (TupleTests, Zeros)
  {
  Tuple<3,float> b(Tuple<3,float>::zeros());
  ASSERT_EQ(b[0], 0);
  ASSERT_EQ(b[1], 0);
  ASSERT_EQ(b[2], 0);
  }

TEST_F (TupleTests, Ones)
  {
  Tuple<3,float> c(Tuple<3,float>(1.0,1.0,1.0));
  ASSERT_EQ(c[0], 1.0);
  ASSERT_EQ(c[1], 1.0);
  ASSERT_EQ(c[2], 1.0);
  }

TEST_F (TupleTests, CopyConstruction)
  {
  Tuple<3,float> d(2.0, 4.0, 6.0);
  Tuple<3,float> e(d);
  ASSERT_EQ(e[0], 2.0);
  ASSERT_EQ(e[1], 4.0);
  ASSERT_EQ(e[2], 6.0);
  }

TEST_F (TupleTests, ConstructionFromPointer)
  {
  float d[3] = {2.0, 4.0, 6.0};
  Tuple<3,float> e(d);
  ASSERT_EQ(e[0], 2.0);
  ASSERT_EQ(e[1], 4.0);
  ASSERT_EQ(e[2], 6.0);
  }

TEST_F (TupleTests, Pointer)
  {
  Tuple<3,float> d(2.0, 4.0, 6.0);
  ASSERT_EQ(&(d[2]), d.ptr()+2);
  }
 
TEST_F (TupleTests, Assignment)
  {
  float d[3] = {2.0, 4.0, 6.0};
  Tuple<3,float> f;
  f = d;
  ASSERT_EQ(f[0], 2.0);
  ASSERT_EQ(f[1], 4.0);
  ASSERT_EQ(f[2], 6.0);
  }

TEST_F (TupleTests, Equality)
  {
  Tuple<3,float> a(1.0,3.0,5.0);
  Tuple<3,float> b(1.0,3.0,5.0);
  ASSERT_EQ(a,a);
  ASSERT_EQ(a,b);
  }

TEST_F (TupleTests, Inequality)
  {
  Tuple<3,float> a(1.0,3.0,5.0);
  Tuple<3,float> b(1.0,3.0,5.1);
  ASSERT_NE(a,b);
  }

TEST_F (TupleTests, Negation)
  {
  Tuple<3,float> a(1.0,3.0,5.0);
  Tuple<3,float> b = -a;
  ASSERT_EQ(b[0], -1.0);
  ASSERT_EQ(b[1], -3.0);
  ASSERT_EQ(b[2], -5.0);
  }

TEST_F (TupleTests, StreamOperators)
  {
  Tuple<3,int> a;
  Tuple<3,int> b(1,4,6);
  std::stringstream buffer;
  buffer << "(1,4,6)";
  ASSERT_FALSE(buffer.fail());
  buffer >> a;
  ASSERT_EQ(a, b);
  ASSERT_TRUE(buffer.eof());
  ASSERT_FALSE(buffer.fail());
  }

TEST_F (TupleTests, StreamOperators2)
  {
  Tuple<3,int> a;
  Tuple<3,int> b(1,4,6);
  std::stringstream buffer;
  buffer.clear();
  buffer << b;
  buffer >> a;
  ASSERT_EQ(a, b);
  ASSERT_TRUE(buffer.eof());
  ASSERT_FALSE(buffer.fail());
  }

TEST_F (TupleTests, StreamOperatorsTupleTuple)
  {
  std::stringstream buffer;
  buffer << "((2,4),(3,5))";
  Tuple<2,Tuple<2,int> > a;
  buffer >> a;
  ASSERT_EQ(a[0][0], 2);
  ASSERT_EQ(a[0][1], 4);
  ASSERT_EQ(a[1][0], 3);
  ASSERT_EQ(a[1][1], 5);
  ASSERT_TRUE(buffer.eof());
  ASSERT_FALSE(buffer.fail());
  }

TEST_F (TupleTests, AddAScalar)
  {
  Tuple<1,int> a(2);
  Tuple<2,int> b(2,3);
  Tuple<3,int> c(2,3,4);
  Tuple<4,int> d(2,3,4,5);
  int s = 3;
  ASSERT_EQ(a+s, (Tuple<1,int>(5)));
  ASSERT_EQ(b+s, (Tuple<2,int>(5,6)));
  ASSERT_EQ(c+s, (Tuple<3,int>(5,6,7)));
  ASSERT_EQ(d+s, (Tuple<4,int>(5,6,7,8)));
  }

TEST_F (TupleTests, Addition)
  {
  Tuple<1,int> a(2);
  Tuple<1,int> a2(10);
  Tuple<2,int> b(2,3);
  Tuple<2,int> b2(20,10);
  Tuple<3,int> c(2,3,4);
  Tuple<3,int> c2(30,20,10);
  Tuple<4,int> d(2,3,4,5);
  Tuple<4,int> d2(40,30,20,10);
  ASSERT_EQ(a+a2, (Tuple<1,int>(12)));
  ASSERT_EQ(b+b2, (Tuple<2,int>(22,13)));
  ASSERT_EQ(c+c2, (Tuple<3,int>(32,23,14)));
  ASSERT_EQ(d+d2, (Tuple<4,int>(42,33,24,15)));
  }

TEST_F (TupleTests, SubtractAScalar)
  {
  Tuple<1,int> a(2);
  Tuple<2,int> b(2,3);
  Tuple<3,int> c(2,3,4);
  Tuple<4,int> d(2,3,4,5);
  int s = 3;
  ASSERT_EQ(a-s, (Tuple<1,int>(-1)));
  ASSERT_EQ(b-s, (Tuple<2,int>(-1,0)));
  ASSERT_EQ(c-s, (Tuple<3,int>(-1,0,1)));
  ASSERT_EQ(d-s, (Tuple<4,int>(-1,0,1,2)));
  }

TEST_F (TupleTests, Subtraction)
  {
  Tuple<1,int> a(2);
  Tuple<1,int> a2(10);
  Tuple<2,int> b(2,3);
  Tuple<2,int> b2(20,10);
  Tuple<3,int> c(2,3,4);
  Tuple<3,int> c2(30,20,10);
  Tuple<4,int> d(2,3,4,5);
  Tuple<4,int> d2(40,30,20,10);
  ASSERT_EQ(a-a2, (Tuple<1,int>(-8)));
  ASSERT_EQ(b-b2, (Tuple<2,int>(-18,-7)));
  ASSERT_EQ(c-c2, (Tuple<3,int>(-28,-17,-6)));
  ASSERT_EQ(d-d2, (Tuple<4,int>(-38,-27,-16,-5)));
  }

TEST_F (TupleTests, MultiplicationByScalar)
  {
  Tuple<1,int> a(2);
  Tuple<2,int> b(2,3);
  Tuple<3,int> c(2,3,4);
  Tuple<4,int> d(2,3,4,5);
  int s = 3;
  ASSERT_EQ(s*a, (Tuple<1,int>(6)));
  ASSERT_EQ(s*b, (Tuple<2,int>(6,9)));
  ASSERT_EQ(s*c, (Tuple<3,int>(6,9,12)));
  ASSERT_EQ(s*d, (Tuple<4,int>(6,9,12,15)));
  }

TEST_F (TupleTests, Multiplication)
  {
  Tuple<1,int> a(2);
  Tuple<1,int> a2(10);
  Tuple<2,int> b(2,3);
  Tuple<2,int> b2(20,10);
  Tuple<3,int> c(2,3,4);
  Tuple<3,int> c2(30,20,10);
  Tuple<4,int> d(2,3,4,5);
  Tuple<4,int> d2(40,30,20,10);
  ASSERT_EQ(a*a2, (Tuple<1,int>(20)));
  ASSERT_EQ(b*b2, (Tuple<2,int>(40,30)));
  ASSERT_EQ(c*c2, (Tuple<3,int>(60,60,40)));
  ASSERT_EQ(d*d2, (Tuple<4,int>(80,90,80,50)));
  }

TEST_F (TupleTests, DivideByScalar)
  {
  Tuple<1,float> a(2);
  Tuple<2,float> b(2,3);
  Tuple<3,float> c(2,3,4);
  float s = 3;
  ASSERT_EQ(a/s, (Tuple<1,float>(float(2)/float(3))));
  ASSERT_EQ(b/s, (Tuple<2,float>(float(2)/float(3),float(3)/float(3))));
  ASSERT_EQ(c/s, (Tuple<3,float>(float(2)/float(3),float(3)/float(3),float(4)/float(3))));
  }

TEST_F (TupleTests, Division)
  {
  Tuple<1,float> a(2);
  Tuple<1,float> a2(10);
  Tuple<2,float> b(2,3);
  Tuple<2,float> b2(20,10);
  ASSERT_EQ(a/a2, (Tuple<1,float>(float(2)/float(10))));
  ASSERT_EQ(b/b2, (Tuple<2,float>(float(2)/float(20),float(3)/float(10))));
  }

TEST_F (TupleTests, Product)
  {
  Tuple<1,int> a(2);
  Tuple<2,int> b(2,3);
  Tuple<3,int> c(2,3,4);
  Tuple<4,int> d(2,3,4,5);
  ASSERT_EQ(product(a), 2);
  ASSERT_EQ(product(b), 6);
  ASSERT_EQ(product(c), 24);
  ASSERT_EQ(product(d), 120);
  }

TEST_F (TupleTests, LongProduct)
  {
  Tuple<2,int> b(1073741824,1073741824);
  ASSERT_EQ(long_product(b), 1152921504606846976);
  }

TEST_F (TupleTests, Sum)
  {
  Tuple<1,int> a(2);
  Tuple<2,int> b(2,3);
  Tuple<3,int> c(2,3,4);
  Tuple<4,int> d(2,3,4,5);
  ASSERT_EQ(sum(a), 2);
  ASSERT_EQ(sum(b), 5);
  ASSERT_EQ(sum(c), 9);
  ASSERT_EQ(sum(d), 14);
  }

TEST_F (TupleTests, Norm)
  {
  Tuple<1,float> a(2);
  Tuple<2,float> b(3,4);
  Tuple<3,float> c(2,3,4);
  Tuple<4,float> d(2,3,4,5);
  ASSERT_FLOAT_EQ(norm(a), 2);
  ASSERT_FLOAT_EQ(norm(b), 5);
  ASSERT_FLOAT_EQ(norm(c), sqrt(sum(c*c)));
  ASSERT_FLOAT_EQ(norm(d), sqrt(sum(d*d)));
  }

TEST_F (TupleTests, Dot)
  {
  Tuple<1,int> a(2);
  Tuple<1,int> a2(10);
  Tuple<2,int> b(2,3);
  Tuple<2,int> b2(20,10);
  Tuple<3,int> c(2,3,4);
  Tuple<3,int> c2(30,20,10);
  Tuple<4,int> d(2,3,4,5);
  Tuple<4,int> d2(40,30,20,10);
  ASSERT_EQ(dot(a,a2), 20);
  ASSERT_EQ(dot(a,a), 4);
  ASSERT_EQ(dot(b,b2), 70);
  ASSERT_EQ(dot(b,b), 13);
  ASSERT_EQ(dot(c,c2), 160);
  ASSERT_EQ(dot(c,c), 29);
  ASSERT_EQ(dot(d,d2), 300);
  ASSERT_EQ(dot(d,d), 54);
  }

TEST_F (TupleTests, Reverse)
  {
  Tuple<1,int> a(2);
  Tuple<2,int> b(2,3);
  Tuple<3,int> c(2,3,4);
  Tuple<4,int> d(2,3,4,5);
  ASSERT_EQ(reverse(a), (Tuple<1,int>(2)));
  ASSERT_EQ(reverse(b), (Tuple<2,int>(3,2)));
  ASSERT_EQ(reverse(c), (Tuple<3,int>(4,3,2)));
  ASSERT_EQ(reverse(d), (Tuple<4,int>(5,4,3,2)));
  }
