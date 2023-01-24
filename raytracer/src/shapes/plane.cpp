#include "plane.h"

Plane::Plane(const PropertyList &propList) {
  m_position = propList.getPoint("position", Point3f(0, 0, 0));
  m_normal = propList.getVector("normal", Point3f(0, 0, 1));
  m_normal.normalize();
}

void Plane::intersect(const Ray &ray, Hit &hit) const {
  // throw RTException("Plane::intersect not implemented yet.");

  float Vd = m_normal.dot(ray.direction);

  if (Vd == 0.f) // ray parallel to the plane
    return;

  float D = m_position.dot(m_normal);
  float V0 = m_normal.dot(ray.origin);
  float t = (D - V0) / Vd;

  if (t < 0.f)
    return;

  hit.t = t;
  hit.normal = m_normal;

  // Tex. coords
  Vector3f u = Vector3f( m_normal.y(), m_normal.z(), -m_normal.x() );
  Vector3f v = u.cross(m_normal);
  Point3f p = ray.at(t);
  hit.uv = Vector2f(p.dot(u), p.dot(v));
}

REGISTER_CLASS(Plane, "plane")
