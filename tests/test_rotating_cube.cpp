#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

#include "introalgo/geometry/rotating_cube.hpp"
#include "test_utils.hpp"

using introalgo::Cube;
using introalgo::kPi;
using introalgo::Mat3;
using introalgo::Vec3;

namespace {

double det(const Mat3& m) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
           m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

double norm(const Vec3& v) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void expectNear(const Vec3& a, const Vec3& b, double eps = 1e-9) {
    for (int i = 0; i < 3; ++i) EXPECT_NEAR(a[std::size_t(i)], b[std::size_t(i)], eps);
}

void expectMatNear(const Mat3& a, const Mat3& b, double eps = 1e-9) {
    for (int i = 0; i < 3; ++i) expectNear(a[std::size_t(i)], b[std::size_t(i)], eps);
}

}  // namespace

TEST(Rotation, QuarterTurnsAboutEachAxis) {
    // Right-handed convention: x -> y about z, y -> z about x, z -> x about y.
    expectNear(introalgo::rotateVec(introalgo::axisRotation(2, kPi / 2), {1, 0, 0}), {0, 1, 0});
    expectNear(introalgo::rotateVec(introalgo::axisRotation(0, kPi / 2), {0, 1, 0}), {0, 0, 1});
    expectNear(introalgo::rotateVec(introalgo::axisRotation(1, kPi / 2), {0, 0, 1}), {1, 0, 0});
    EXPECT_THROW(introalgo::axisRotation(3, 0), std::invalid_argument);
}

TEST(Rotation, MatricesAreProperRotations) {
    testutil::Rng rng(101);
    for (int iter = 0; iter < 200; ++iter) {
        Cube c(2);
        c.getRotateMat(testutil::randReal(rng, -10, 10), testutil::randReal(rng, -10, 10),
                       testutil::randReal(rng, -10, 10));
        const Mat3& r = c.rotationMatrix();
        EXPECT_NEAR(det(r), 1.0, 1e-9);
        // R R^T = I
        Mat3 rt{};
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                rt[std::size_t(i)][std::size_t(j)] = r[std::size_t(j)][std::size_t(i)];
        expectMatNear(introalgo::multiply(r, rt), introalgo::axisRotation(0, 0.0));
    }
}

TEST(Rotation, DegreesAndRadiansAgree) {
    // pi was declared `static const int pi = M_PI;` (= 3) in the original, so the degree version
    // rotated by 3/180 turns per degree.  Angles of 180 degrees or more were also folded modulo
    // 180.
    Cube a(2), b(2);
    for (double deg : {30.0, 90.0, 179.0, 180.0, 270.0, -45.0, 725.0}) {
        a.getRotateMatd(deg, 0.5 * deg, -deg);
        b.getRotateMat(deg * kPi / 180, 0.5 * deg * kPi / 180, -deg * kPi / 180);
        expectMatNear(a.rotationMatrix(), b.rotationMatrix());
    }
    a.getRotateMatd(270, 0, 0);
    b.getRotateMatd(-90, 0, 0);
    expectMatNear(a.rotationMatrix(), b.rotationMatrix());
    a.getRotateMatd(180, 0, 0);
    EXPECT_NEAR(a.rotationMatrix()[1][1], -1.0, 1e-12);  // a half turn is not the identity
}

TEST(Rotation, ApplicationOrderIsZThenYThenX) {
    Cube c(1);
    c.getRotateMat(0.3, -0.8, 1.1);
    const Vec3 p{1.0, 2.0, 3.0};
    Vec3 expected = introalgo::rotateVec(introalgo::axisRotation(2, 1.1), p);
    expected = introalgo::rotateVec(introalgo::axisRotation(1, -0.8), expected);
    expected = introalgo::rotateVec(introalgo::axisRotation(0, 0.3), expected);
    expectNear(introalgo::rotateVec(c.rotationMatrix(), p), expected);
}

TEST(Cube, GridLayoutMatchesTheOriginal) {
    Cube c(4);
    EXPECT_EQ(c.side(), 4);
    expectNear(c.point(0, 0), {-2, -2, 2});
    expectNear(c.point(1, 3), {1, -1, 2});
    EXPECT_THROW(Cube(0), std::invalid_argument);
}

TEST(Cube, RotationPreservesDistancesFromTheCentreAndIsCumulative) {
    testutil::Rng rng(102);
    Cube c(5);
    std::vector<double> radius;
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j) radius.push_back(norm(c.point(i, j)));
    c.getRotateMat(0.4, 1.3, -2.2);
    c.rotate();
    c.rotate();
    std::size_t k = 0;
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j) EXPECT_NEAR(norm(c.point(i, j)), radius[k++], 1e-9);

    // two quarter turns about z = one half turn
    Cube d(3), e(3);
    d.getRotateMat(0, 0, kPi / 2);
    d.rotate();
    d.rotate();
    e.getRotateMat(0, 0, kPi);
    e.rotate();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) expectNear(d.point(i, j), e.point(i, j));
}

TEST(Cube, ShowDrawsOneStarPerGridPointForAxisAlignedViews) {
    auto stars = [](const Cube& cube) {
        std::ostringstream out;
        cube.show(out);
        const std::string text = out.str();
        return std::count(text.begin(), text.end(), '*');
    };
    Cube c(5);
    EXPECT_EQ(stars(c), 25);

    c.getRotateMatd(0, 0, 90);  // a quarter turn about z maps the grid onto itself
    c.rotate();
    EXPECT_EQ(stars(c), 25);

    Cube tilted(9);
    tilted.getRotateMatd(60, 35, 20);
    tilted.rotate();
    EXPECT_GT(stars(tilted),
              0);  // drawing must stay inside the canvas (the original wrote out of bounds)
}
