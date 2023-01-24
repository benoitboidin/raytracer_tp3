/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include "bsdf.h"
#include "texture.h"

/**
 * \brief Diffuse / Lambertian BRDF model
 */
class Diffuse : public BSDF {
public:
  Diffuse(const PropertyList &propList) {
    m_albedo = propList.getColor("albedo", Color3f(0.5f));

    // Load texture if any, otherwise use albedo
    m_texture = new Texture(m_albedo, propList);

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

    /* The BRDF is simply the albedo / pi */
    return m_texture->lookUp(uv) * INV_PI;
  }

  /// Return a human-readable summary
  std::string toString() const {
    return tfm::format("Diffuse[\n"
                       "  albedo = %s\n"
                       "]",
                       m_albedo.toString());
  }

  EClassType getClassType() const { return EBSDF; }

private:
  Color3f m_albedo;
};

REGISTER_CLASS(Diffuse, "diffuse");
