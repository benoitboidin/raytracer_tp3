/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#if defined(_MSC_VER)
/* Disable some warnings on MSVC++ */
#pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512)
#define WIN32_LEAN_AND_MEAN /* Don't ever include MFC on Windows */
#define NOMINMAX            /* Don't override min/max */
#endif

/* Include the basics needed by any Nori file */
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <algorithm>
#include <filesystem/resolver.h>
#include <iostream>
#include <stdint.h>
#include <tinyformat.h>
#include <vector>

/* "Ray epsilon": relative error threshold for ray intersection computations */
#define Epsilon 1e-4f

static const float OneMinusEpsilon = 0x1.fffffep-1;

/* A few useful constants */
#undef M_PI

#define M_PI 3.14159265358979323846f
#define INV_PI 0.31830988618379067154f
#define INV_TWOPI 0.15915494309189533577f
#define INV_FOURPI 0.07957747154594766788f
#define SQRT_TWO 1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f

/* Forward declarations */
namespace filesystem {
class path;
class resolver;
} // namespace filesystem

template <typename Scalar, int Dimension> struct TVector;
template <typename Scalar, int Dimension> struct TPoint;
template <typename Point> struct TBoundingBox;

typedef TVector<float, 1> Vector1f;
typedef TVector<float, 2> Vector2f;
typedef TVector<float, 3> Vector3f;
typedef TVector<float, 4> Vector4f;
typedef TVector<double, 1> Vector1d;
typedef TVector<double, 2> Vector2d;
typedef TVector<double, 3> Vector3d;
typedef TVector<double, 4> Vector4d;
typedef TVector<int, 1> Vector1i;
typedef TVector<int, 2> Vector2i;
typedef TVector<int, 3> Vector3i;
typedef TVector<int, 4> Vector4i;
typedef TPoint<float, 1> Point1f;
typedef TPoint<float, 2> Point2f;
typedef TPoint<float, 3> Point3f;
typedef TPoint<float, 4> Point4f;
typedef TPoint<double, 1> Point1d;
typedef TPoint<double, 2> Point2d;
typedef TPoint<double, 3> Point3d;
typedef TPoint<double, 4> Point4d;
typedef TPoint<int, 1> Point1i;
typedef TPoint<int, 2> Point2i;
typedef TPoint<int, 3> Point3i;
typedef TPoint<int, 4> Point4i;
typedef TBoundingBox<Point2i> BoundingBox2i;
typedef TBoundingBox<Point3f> BoundingBox3f;

/// Simple exception class, which stores a human-readable error description
class RTException : public std::runtime_error {
public:
  /// Variadic template constructor to support printf-style arguments
  template <typename... Args>
  RTException(const char *fmt, const Args &...args)
      : std::runtime_error(tfm::format(fmt, args...)) {}
};

/// Import cout, cerr, endl for debugging purposes
using std::cerr;
using std::cout;
using std::endl;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> MatrixXf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;

/// Return the number of cores (real and virtual)
extern int getCoreCount();

/// Indent a string by the specified number of spaces
extern std::string indent(const std::string &string, int amount = 2);

/// Convert a string to lower case
extern std::string toLower(const std::string &value);

/// Convert a string into an boolean value
extern bool toBool(const std::string &str);

/// Convert a string into a signed integer value
extern int toInt(const std::string &str);

/// Convert a string into an unsigned integer value
extern unsigned int toUInt(const std::string &str);

/// Convert a string into a floating point value
extern float toFloat(const std::string &str);

/// Convert a string into a 3D vector
extern Eigen::Vector3f toVector3f(const std::string &str);

/// Convert a string into a quaternion
extern Eigen::Quaternionf toQuaternionf(const std::string &str);

/// Tokenize a string into a list by splitting at 'delim'
extern std::vector<std::string> tokenize(const std::string &s,
                                         const std::string &delim = ", ",
                                         bool includeEmpty = false);

/// Check if a string ends with another string
extern bool endsWith(const std::string &value, const std::string &ending);

/// Convert a time value in milliseconds into a human-readable string
extern std::string timeString(double time, bool precise = false);

/// Convert a memory amount in bytes into a human-readable string
extern std::string memString(size_t size, bool precise = false);

//// Convert radians to degrees
inline float radToDeg(float value) { return value * (180.0f / M_PI); }

/// Convert degrees to radians
inline float degToRad(float value) { return value * (M_PI / 180.0f); }

#if !defined(_GNU_SOURCE)
/// Emulate sincosf using sinf() and cosf()
inline void sincosf(float theta, float *_sin, float *_cos) {
  *_sin = sinf(theta);
  *_cos = cosf(theta);
}
#endif

/// Simple floating point clamping function
inline float clamp(float value, float min, float max) {
  if (value < min)
    return min;
  else if (value > max)
    return max;
  else
    return value;
}

/// Simple integer clamping function
inline int clamp(int value, int min, int max) {
  if (value < min)
    return min;
  else if (value > max)
    return max;
  else
    return value;
}

/// Linearly interpolate between two values
inline float lerp(float t, float v1, float v2) {
  return ((float)1 - t) * v1 + t * v2;
}

/// Always-positive modulo operation
inline int mod(int a, int b) {
  int r = a % b;
  return (r < 0) ? r + b : r;
}

inline std::string toString(const Eigen::Vector3f &v) {
  std::string result;
  for (size_t i = 0; i < 3; ++i) {
    result += std::to_string(v.coeff(i));
    if (i + 1 < 3)
      result += ", ";
  }
  return "[" + result + "]";
}

inline Eigen::Vector3f reflect(const Eigen::Vector3f &v,
                               const Eigen::Vector3f &n) {
  return -v + 2.f * v.dot(n) * n;
}

/**
 * Computed the refracted direction @param refracted of an @param incident
 * direction according to the @param normal and the material refraction
 * coefficients @param etaI et @param etaT
 * @return false in case of total internal reflection for transmission
 */
inline bool refract(const Eigen::Vector3f &normal,
                    const Eigen::Vector3f &incident, float etaI, float etaT,
                    Eigen::Vector3f &refracted) {
  float eta = etaI / etaT;
  float cosThetaI = -normal.dot(incident);
  float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
  float sin2ThetaT = eta * eta * sin2ThetaI;
  // Handle total internal reflection for transmission
  if (sin2ThetaT > 1.f)
    return false;
  float cosThetaT = std::sqrt(1.f - sin2ThetaT);
  refracted = eta * incident + (eta * cosThetaI - cosThetaT) * normal;
  return true;
}

/**
 * \brief Calculates the unpolarized fresnel reflection coefficient for a
 * dielectric material. Handles incidence from either side (i.e.,
 * \code cosThetaI<0 is allowed).
 *
 * \param cosThetaI
 *      Cosine of the angle between the normal and the incident ray
 * \param extIOR
 *      Refractive index of the side that contains the surface normal
 * \param intIOR
 *      Refractive index of the interior
 */
extern float fresnel(float cosThetaI, float extIOR, float intIOR);

/**
 * \brief Return the global file resolver instance
 *
 * This class is used to locate resource files (e.g. mesh or
 * texture files) referenced by a scene being loaded
 */
extern filesystem::resolver *getFileResolver();
