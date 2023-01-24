#include "bsdf.h"
#include "integrator.h"
#include "scene.h"

class FlatIntegrator : public Integrator {
public:
  FlatIntegrator(const PropertyList &props) { /* No parameters this time */
  }

  Color3f Li(const Scene *scene, const Ray &ray) const {
    /* Find the surface that is visible in the requested direction */
    Hit hit;
    scene->intersect(ray, hit);
    if (!hit.foundIntersection())
      return scene->backgroundColor();

    /* Return the object albedo */
    Color3f albedo = hit.shape->bsdf()->albedo(hit.uv);
    return albedo;
  }

  std::string toString() const { return "FlatIntegrator[]"; }
};

REGISTER_CLASS(FlatIntegrator, "flat")
