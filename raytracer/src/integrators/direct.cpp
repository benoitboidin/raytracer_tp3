#include "bsdf.h"
#include "integrator.h"
#include "scene.h"

class Direct : public Integrator {
public:
  Direct(const PropertyList &props) {}

  Color3f Li(const Scene *scene, const Ray &ray) const {
    /* Find the surface that is visible in the requested direction */
    Hit hit;
    scene->intersect(ray, hit);
    if (!hit.foundIntersection())
      return scene->backgroundColor();

    Color3f radiance = Color3f::Zero();
    const BSDF *bsdf = hit.shape->bsdf();
    Point3f pos = ray.at(hit.t);

    for (Light* light : scene->lightList()) {
      float dist;
      Vector3f lightDir;
      Color3f intensity = light->intensity(pos, lightDir, dist);
      Ray shadowRay(pos + hit.normal * Epsilon, lightDir, true);
      Hit shadowHit;
      scene->intersect(shadowRay, shadowHit);
      if (!shadowHit.foundIntersection() || shadowHit.t > dist) {
        float cos_term = std::max(0.f, lightDir.dot(hit.normal));
        Color3f brdf = bsdf->eval(-ray.direction, lightDir, hit.normal, hit.uv);
        radiance += intensity * cos_term * brdf;
      }
    }

    return radiance;
  }

  std::string toString() const { return "Direct[]"; }
};

REGISTER_CLASS(Direct, "direct")
