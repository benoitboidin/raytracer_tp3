/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include "object.h"
#include "texture.h"

enum class MaterialType : int {
  Reflection = 1,
  ReflectionAndRefraction = 2,
  DiffuseAndGlossy = 4,
};

/**
 * \brief Superclass of all bidirectional scattering distribution functions
 */
class BSDF : public Object {
public:
  virtual ~BSDF() {
    if (m_texture)
      delete m_texture;
  }

  /**
   * \brief \return the albedo of the BSDF (optionally at texture coordinates
   * \param uv)
   */
  virtual Color3f albedo(const Point2f &uv) const = 0;

  /**
   * \brief Evaluate the BSDF for a pair of directions
   *
   * \param wi
   *    Incident direction
   * \param wo
   *    Outgoing direction
   * \param n
   *    Shading normal
   * \param uv
   *    Texture coordinates
   *
   * \return
   *     The BSDF value, evaluated for each color channel
   */
  virtual Color3f eval(const Vector3f &wi, const Vector3f &wo,
                       const Normal3f &n, const Point2f &uv) const = 0;

  virtual float extIOR() const { return 0.f; }
  virtual float intIOR() const { return 0.f; }

  /**
   * \brief Return whether or not this BRDF has a diffuse component.
   */
  MaterialType type() const { return m_type; }

  /**
   * \brief Return the type of object (i.e., Mesh/BSDF/etc.)
   * provided by this instance
   * */
  EClassType getClassType() const { return EBSDF; }

protected:
  MaterialType m_type;
  Texture *m_texture{nullptr};
};
