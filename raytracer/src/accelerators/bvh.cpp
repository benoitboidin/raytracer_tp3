#include "accelerators/bvh.h"
#include "shapes/mesh.h"

#include <algorithm>
#include <iostream>

void BVH::build(const Mesh *pMesh, int targetCellSize, int maxDepth) {
  m_pMesh = pMesh;
  m_nodes.resize(1);
  if (m_pMesh->nbFaces() <= targetCellSize) {
    m_nodes[0].box = pMesh->getBoundingBox();
    m_nodes[0].first_face_id = 0;
    m_nodes[0].is_leaf = true;
    m_nodes[0].nb_faces = m_pMesh->nbFaces();
    m_faces.resize(m_pMesh->nbFaces());
    for (int i = 0; i < m_pMesh->nbFaces(); ++i) {
      m_faces[i] = i;
    }
  } else {
    m_nodes.reserve(std::min<int>(
        2 << maxDepth, std::log(m_pMesh->nbFaces() / targetCellSize)));
    
    // compute centroids and initialize the face list
    m_centroids.resize(m_pMesh->nbFaces());
    m_faces.resize(m_pMesh->nbFaces());
    for (int i = 0; i < m_pMesh->nbFaces(); ++i) {
      m_centroids[i] = (m_pMesh->vertexOfFace(i, 0).position +
                        m_pMesh->vertexOfFace(i, 1).position +
                        m_pMesh->vertexOfFace(i, 2).position) /
                       3.f;
      m_faces[i] = i;
    }
    buildNode(0, 0, m_pMesh->nbFaces(), 0, targetCellSize, maxDepth);
  }
}

void BVH::intersect(const Ray &ray, Hit &hit) const {
  // compute the intersection with the root node
  float tMin, tMax;
  Normal3f n;
  m_nodes[0].box.rayIntersect(ray, tMin, tMax);

  // TODO
  // vérifier si on a bien une intersection (en fonction de tMin, tMax, et
  // hit.t()), et si oui appeler intersecNode...
  throw RTException("BVH::intersect not implemented yet.");
}

void BVH::intersectNode(int nodeId, const Ray &ray, Hit &hit) const {
  // TODO, deux cas: soit mNodes[nodeId] est une feuille (il faut alors
  // intersecter les triangles du noeud), soit c'est un noeud interne (il faut
  // visiter les fils (ou pas))
  throw RTException("BVH::intersectNode not implemented yet.");
}

/** Sorts the faces with respect to their centroid along the dimension \a dim
 * and spliting value \a split_value. \returns the middle index
 */
int BVH::split(int start, int end, int dim, float split_value) {
  int l(start), r(end - 1);
  while (l < r) {
    // find the first on the left
    while (l < end && m_centroids[l](dim) < split_value)
      ++l;
    while (r >= start && m_centroids[r](dim) >= split_value)
      --r;
    if (l > r)
      break;
    std::swap(m_centroids[l], m_centroids[r]);
    std::swap(m_faces[l], m_faces[r]);
    ++l;
    --r;
  }
  return m_centroids[l][dim] < split_value ? l + 1 : l;
}

void BVH::buildNode(int nodeId, int start, int end, int level,
                    int targetCellSize, int maxDepth) {
  Node &node = m_nodes[nodeId];

  // étape 1 : calculer la boite englobante des faces indexées de m_faces[start]
  // à m_faces[end] (Utiliser la fonction extend de Eigen::AlignedBox3f et la
  // fonction mpMesh->vertexOfFace(int) pour obtenir les coordonnées des sommets
  // des faces)

  // étape 2 : déterminer si il s'agit d'une feuille (appliquer les critères
  // d'arrêts)

  // Si c'est une feuille, finaliser le noeud et quitter la fonction

  // Si c'est un noeud interne :

  // étape 3 : calculer l'index de la dimension (x=0, y=1, ou z=2) et la valeur
  // du plan de coupe (on découpe au milieu de la boite selon la plus grande
  // dimension)

  // étape 4 : appeler la fonction split pour trier (partiellement) les faces et
  // vérifier si le split a été utile

  // étape 5 : allouer les fils, et les construire en appelant buildNode...

  // TODO
  throw RTException("BVH::buildNode not implemented yet.");
}
