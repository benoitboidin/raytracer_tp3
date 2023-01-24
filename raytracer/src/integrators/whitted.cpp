#include "bsdf.h"
#include "integrator.h"
#include "scene.h"

class Whitted : public Integrator {
public:
  Whitted(const PropertyList &props) {
    m_maxRecursion = props.getInteger("maxRecursion", 4);
  }

  Color3f Li(const Scene *scene, const Ray &ray) const {

    Color3f radiance = Color3f::Zero();

    // stopping criteria:
    if (ray.recursionLevel >= m_maxRecursion) {
      return radiance;
    }

    /* Find the surface that is visible in the requested direction */
    Hit hit;
    scene->intersect(ray, hit);
    if (!hit.foundIntersection()) {
      return scene->backgroundColor(ray.direction);
    }

    Normal3f normal = hit.normal;
    Point3f pos = ray.at(hit.t);

    const BSDF *bsdf = hit.shape->bsdf();

    if (bsdf->type() == MaterialType::Reflection) {
      Vector3f reflectDir = reflect(-ray.direction, normal);
      Ray reflexion_ray(pos + normal * Epsilon, reflectDir);
      reflexion_ray.recursionLevel = ray.recursionLevel + 1;
      float cos_term = std::max(0.f, reflectDir.dot(normal));
      return cos_term * bsdf->albedo(hit.uv) * Li(scene, reflexion_ray);
    }
    if (bsdf->type() == MaterialType::ReflectionAndRefraction) {
      Vector3f reflectDir = reflect(-ray.direction, normal);
      Point3f reflectionRayOrig;
      if (reflectDir.dot(normal) > 0)
        reflectionRayOrig = pos + normal * Epsilon;
      else
        reflectionRayOrig = pos - normal * Epsilon;
      Ray reflexion_ray(reflectionRayOrig, reflectDir);
      reflexion_ray.recursionLevel = ray.recursionLevel + 1;
      Color3f reflectedColor = bsdf->albedo(hit.uv) * Li(scene, reflexion_ray);

      float etaI = bsdf->extIOR(), etaT = bsdf->intIOR();
      if (normal.dot(-ray.direction) < 0) {
        std::swap(etaI, etaT);
        normal = -normal;
      }
      Vector3f r;
      Color3f transmittedColor(0.f);
      if (refract(normal, ray.direction, etaI, etaT, r)) {
        Point3f transmissionRayOrig;
        if (r.dot(normal) > 0)
          transmissionRayOrig = pos + normal * Epsilon;
        else
          transmissionRayOrig = pos - normal * Epsilon;
        Ray refraction_ray(pos - normal * Epsilon, r);
        refraction_ray.recursionLevel = ray.recursionLevel + 1;
        transmittedColor = bsdf->albedo(hit.uv) * Li(scene, refraction_ray);
      }
      float f = fresnel(-ray.direction.dot(normal), etaI, etaT); 
      return f * reflectedColor + (1.f - f) * transmittedColor;
    }

    for (Light *light : scene->lightList()) {
      Vector3f lightDir;
      float dist;
      Color3f intensity = light->intensity(pos, lightDir, dist);
      Ray shadowRay(pos + normal * Epsilon, lightDir, true);
      Hit shadowHit;
      scene->intersect(shadowRay, shadowHit);
      if (shadowHit.t < dist) {
        continue;
      }
      float cos_term = std::max(0.f, lightDir.dot(normal));
      Color3f brdf = bsdf->eval(-ray.direction, lightDir, normal, hit.uv);
      radiance += intensity * cos_term * brdf;
    }
    return radiance;
  }

  std::string toString() const {
    return tfm::format("Whitted[\n"
                       "  max recursion = %f\n"
                       " ]",
                       m_maxRecursion);
  }

private:
  int m_maxRecursion;
};

REGISTER_CLASS(Whitted, "whitted")
