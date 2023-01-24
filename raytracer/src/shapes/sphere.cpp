#include "sphere.h"

Sphere::Sphere(float radius) : m_radius(radius) {}

Sphere::Sphere(const PropertyList &propList) {
  m_radius = propList.getFloat("radius", 1.f);
  m_center = propList.getPoint("center",Point3f(0,0,0));
}

void Sphere::intersect(const Ray &ray, Hit &hit) const {
  // throw RTException("Sphere::intersect not implemented yet.");

  float b = 2. * ray.direction.dot(ray.origin - m_center);
  float c = (ray.origin - m_center).squaredNorm() - m_radius * m_radius;
  float discr = b * b - 4. * c;
  if (discr >= 0) {
    discr = std::sqrt(discr);
    float t = 0.5 * (-b - discr);
    if (t < Epsilon)
      t = 0.5 * (-b + discr);
    if (t < Epsilon)
      return;

    hit.t = t;
    Point3f point = (ray.at(t) - m_center);
    hit.normal = point.normalized();

    // Texture coordinates
    float phi = std::atan2(point.y(), point.x());
    if (phi < 0.f)
      phi += 2.f * M_PI;
    float theta = std::acos(clamp(point.z() / m_radius, -1.f, 1.f));
    hit.uv = Point2f(phi / (2.f * M_PI), theta / M_PI);
  }
}

REGISTER_CLASS(Sphere, "sphere")
