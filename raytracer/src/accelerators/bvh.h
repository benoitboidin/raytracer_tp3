#pragma once

#include "accelerators/bbox.h"
#include "ray.h"
#include <Eigen/Geometry>
#include <vector>

class Mesh;

class BVH {

  struct Node {
    BoundingBox3f box;
    union {
      int first_child_id; // for inner nodes
      int first_face_id;  // for leaves
    };
    unsigned short nb_faces;
    short is_leaf = false;
  };

  typedef std::vector<Node> NodeList;

public:
  void build(const Mesh *pMesh, int targetCellSize, int maxDepth);
  void intersect(const Ray &ray, Hit &hit) const;

protected:
  void intersectNode(int nodeId, const Ray &ray, Hit &hit) const;

  int split(int start, int end, int dim, float split_value);

  void buildNode(int nodeId, int start, int end, int level, int targetCellSize,
                 int maxDepth);

  const Mesh *m_pMesh;
  NodeList m_nodes;
  std::vector<int> m_faces;
  std::vector<Point3f> m_centroids;
};
