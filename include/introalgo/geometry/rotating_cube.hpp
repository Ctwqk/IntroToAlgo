#pragma once

#include <array>
#include <iosfwd>
#include <vector>

namespace introalgo {

using Vec3 = std::array<double, 3>;
using Mat3 = std::array<std::array<double, 3>, 3>;

constexpr double kPi = 3.14159265358979323846;

/// Rotation about the coordinate axis `axis` (0 = x, 1 = y, 2 = z) by `angle` radians, counter-clockwise
/// when looking down the axis towards the origin (the right-handed convention).
Mat3 axisRotation(int axis, double angle);
Mat3 multiply(const Mat3& a, const Mat3& b);
Vec3 rotateVec(const Mat3& m, const Vec3& v);

/// A size x size grid of points on one face of a cube centred at the origin (z = size / 2) that
/// can be rotated about the three axes and drawn as an ASCII projection -- the spinning-cube
/// experiment of the original projects/spinning.cpp.
///
/// getRotateMat(alpha, beta, theta) builds R = Rx(alpha) Ry(beta) Rz(theta); rotate() maps every
/// point p to R p, so the z rotation is applied first.
///
/// Changes relative to the original: pi was an `int` (3) in the degree conversion, degrees were
/// reduced modulo 180 instead of 360, points were multiplied as row vectors (which rotates by the
/// inverse angle), and show() wrote through an uninitialised canvas and outside it after a rotation.
class Cube {
public:
    explicit Cube(int s);

    void getRotateMat(double alpha, double beta, double theta);          // radians
    void getRotateMatd(double alphaD, double betaD, double thetaD);      // degrees
    const Mat3& rotationMatrix() const { return rotMat; }

    /// Rotates every point by the current rotation matrix (cumulative over repeated calls).
    void rotate();

    int side() const { return size; }
    const Vec3& point(int i, int j) const { return cb[static_cast<std::size_t>(i * size + j)]; }

    /// Draws the points projected on the xy plane, one '*' per occupied cell.
    void show(std::ostream& os) const;

private:
    int size;
    std::vector<Vec3> cb;
    Mat3 rotMat;
};

}  // namespace introalgo
