/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include "bsdf.h"

/**
 * \brief Phong BRDF model (energy conservative)
 */
class Phong : public BSDF {
public:
  Phong(const PropertyList &propList) {
    /* Albedo of the diffuse base material (a.k.a "kd") */
    m_kd = propList.getColor("kd", Color3f(0.5f));
    /* Albedo of the specular base material (a.k.a "ks") */
    m_ks = propList.getColor("ks", Color3f(0.5f));
    /* Roughness of the material (a.k.a "exponent") */
    m_exponent = propList.getFloat("exponent", 0.1f);

    // Load texture if any, otherwise use diffuse albedo
    m_texture = new Texture(m_kd, propList);

    m_type = MaterialType::DiffuseAndGlossy;
  }

  Color3f albedo(const Point2f &uv) const { return m_texture->lookUp(uv); }

  /// Evaluate the BRDF model
  Color3f eval(const Vector3f &wi, const Vector3f &wo, const Normal3f &n,
               const Point2f &uv) const {
    /* This is a smooth BRDF -- return zero when queried for illumination on the
     * backside */
    if (wi.dot(n) <= 0 || wo.dot(n) <= 0)
      return Color3f(0.0f);

    float alpha = wi.dot(reflect(wo, n));

    Color3f color = m_texture->lookUp(uv); //* INV_PI;

    if (alpha > 0.0f) {
      // color +=
          // m_ks * (m_exponent + 2.f) * std::pow(alpha, m_exponent) * INV_TWOPI;
      color += m_ks * std::pow(alpha, m_exponent);
    }

    return color;
  }

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("Phong[\n"
                       "  kd = %s\n"
                       "  ks = %s\n"
                       "  exponent = %f\n"
                       "]",
                       m_kd.toString(), m_ks.toString(), m_exponent);
  }

  EClassType getClassType() const { return EBSDF; }

private:
  Color3f m_kd, m_ks;
  float m_exponent;
};

REGISTER_CLASS(Phong, "phong");
