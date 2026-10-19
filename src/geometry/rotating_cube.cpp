#include "introalgo/geometry/rotating_cube.hpp"

#include <cmath>
#include <ostream>
#include <stdexcept>
#include <string>

namespace introalgo {

Mat3 axisRotation(int idx, double ang) {
    if (idx < 0 || idx > 2) throw std::invalid_argument("axisRotation: axis must be 0, 1 or 2");
    // The 2x2 rotation acts on the two axes that follow idx cyclically (y,z for x; z,x for y; x,y
    // for z).
    const double rotateMat[2][2] = {{std::cos(ang), -std::sin(ang)},
                                    {std::sin(ang), std::cos(ang)}};
    Mat3 axisMat{};
    axisMat[idx][idx] = 1;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) axisMat[(idx + i + 1) % 3][(idx + j + 1) % 3] = rotateMat[i][j];
    return axisMat;
}

Mat3 multiply(const Mat3& a, const Mat3& b) {
    Mat3 ans{};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int l = 0; l < 3; l++) ans[i][j] += a[i][l] * b[l][j];
    return ans;
}

Vec3 rotateVec(const Mat3& m, const Vec3& v) {
    Vec3 out{};
    for (int i = 0; i < 3; i++)
        for (int l = 0; l < 3; l++) out[i] += m[i][l] * v[l];
    return out;
}

Cube::Cube(int s) : size(s), rotMat(axisRotation(0, 0.0)) {
    if (s < 1) throw std::invalid_argument("Cube: size must be positive");
    cb.resize(static_cast<std::size_t>(size) * static_cast<std::size_t>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            cb[static_cast<std::size_t>(i * size + j)] = {j - size / 2.0, i - size / 2.0,
                                                          size / 2.0};
}

void Cube::getRotateMat(double alpha, double beta, double theta) {
    rotMat = multiply(multiply(axisRotation(0, std::fmod(alpha, 2 * kPi)),
                               axisRotation(1, std::fmod(beta, 2 * kPi))),
                      axisRotation(2, std::fmod(theta, 2 * kPi)));
}

void Cube::getRotateMatd(double alphaD, double betaD, double thetaD) {
    const double alpha = std::fmod(alphaD, 360) * kPi / 180.0;
    const double beta = std::fmod(betaD, 360) * kPi / 180.0;
    const double theta = std::fmod(thetaD, 360) * kPi / 180.0;
    getRotateMat(alpha, beta, theta);
}

void Cube::rotate() {
    for (Vec3& p : cb) p = rotateVec(rotMat, p);
}

void Cube::show(std::ostream& os) const {
    // The rotated face stays within a disc of radius size * sqrt(3) / 2 around the origin.
    const int dim = static_cast<int>(std::ceil(size * std::sqrt(3.0))) + 1;
    std::vector<std::string> canvas(static_cast<std::size_t>(dim),
                                    std::string(static_cast<std::size_t>(dim), ' '));
    const double centre = (dim - 1) / 2.0;
    for (const Vec3& p : cb) {
        const long col = std::lround(p[0] + centre);
        const long row = std::lround(centre - p[1]);  // y points up on the screen
        if (col >= 0 && col < dim && row >= 0 && row < dim)
            canvas[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = '*';
    }
    for (const std::string& line : canvas) {
        for (char c : line) os << c << ' ';
        os << std::endl;
    }
}

}  // namespace introalgo
