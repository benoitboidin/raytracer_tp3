#pragma once

#include "common.h"
#include "vector.h"

class Shape;

class Ray {
public:
  Ray(const Point3f &o, const Vector3f &d, bool shadow = false)
      : origin(o), direction(d), recursionLevel(0), shadowRay(shadow) {}
  Ray() : recursionLevel(0), shadowRay(false) {}

  Point3f origin;
  Vector3f direction;

  Point3f at(float t) const { return origin + t * direction; }

  int recursionLevel; ///< recursion level (used as a stoping critera)
  bool shadowRay;     ///< tag for shadow rays
};

class Hit {
public:
  /// Unoccluded distance along the ray
  float t;
  /// Shading normal
  Normal3f normal;
  /// Texture coordinates, if any
  Point2f uv;
  /// pointer to the associated shape
  const Shape *shape;

  Hit() : t(std::numeric_limits<float>::max()), uv(0, 0), shape(nullptr) {}

  bool foundIntersection() const {
    return t < std::numeric_limits<float>::max();
  }
};
