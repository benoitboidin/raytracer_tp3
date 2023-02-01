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


  float t = 0; 
  ray.at(t);

  if (tMin < t && tMax > t) {
    intersectNode(0, ray, hit);
  }

  // throw RTException("BVH::intersect not implemented yet.");
}

void BVH::intersectNode(int nodeId, const Ray &ray, Hit &hit) const {
  // TODO, deux cas: soit mNodes[nodeId] est une feuille (il faut alors
  // intersecter les triangles du noeud), soit c'est un noeud interne (il faut
  // visiter les fils (ou pas))
  // throw RTException("BVH::intersectNode not implemented yet.");

  Node node = m_nodes[nodeId];
  int total = node.nb_faces; 

  if (node.is_leaf) {
    // Intersecter les triangles du noeud. 
    Hit tmp_hit = Hit(); 
    int start = node.first_face_id;
    for (int i = start; i < total; i++){
      m_pMesh->intersectFace(ray, tmp_hit, m_faces[i]);
      if (tmp_hit.t < hit.t && tmp_hit.t > 0){
        hit = tmp_hit;
      }
    }
    return;
  }else{
    // Visiter les fils.
    // Tester s'il y a une intersection entre les boites des deux fils. 
    
    // À vérifier...
    float tMin, tMax;
    Normal3f n;
    m_nodes[nodeId].box.rayIntersect(ray, tMin, tMax);
    float t = 0;
    ray.at(t);
    if (tMin < t && tMax > t) {
      intersectNode(nodeId*2+1, ray, hit);
      intersectNode(nodeId*2+2, ray, hit);
    }
  }
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

  for (int face=0; m_faces[face] < end; ++face) {
    Eigen::AlignedBox3f box;
    for (int i=0; i<3; ++i) {
      box.extend(m_pMesh->vertexOfFace(m_faces[face], i).position);
    } 
    // étape 2 : déterminer si il s'agit d'une feuille (appliquer les critères
    // d'arrêts)

    // Si c'est une feuille, finaliser le noeud et quitter la fonction
    if (level >= maxDepth){
      node.is_leaf = true;
      node.first_face_id = start;
      node.nb_faces = end - start;
      return;
    }
    // Si c'est un noeud interne :

    // étape 3 : calculer l'index de la dimension (x=0, y=1, ou z=2) et la valeur
    // du plan de coupe (on découpe au milieu de la boite selon la plus grande
    // dimension)
    Vector3f max_dim = box.max();
    Vector3f min_dim = box.min();
    int ax_cut = 0;
    float ax_val = (max_dim(0) + min_dim(0)) / 2;

    if (max_dim(1) - min_dim(1) > max_dim(0) - min_dim(0)) {
      ax_cut = 1;
      ax_val = (max_dim(1) + min_dim(1)) / 2;
    }
    if (max_dim(2) - min_dim(2) > max_dim(1) - min_dim(1)) {
      ax_cut = 2;
      ax_val = (max_dim(2) + min_dim(2)) / 2;
    }

    // étape 4 : appeler la fonction split pour trier (partiellement) les faces et
    // vérifier si le split a été utile
    int split_val = split(start, end, ax_cut, ax_val);
    
    if (split_val == start || split_val == end) {
      node.is_leaf = true;
      node.first_face_id = start;
      node.nb_faces = end - start;
      return;
    }
    
    // étape 5 : allouer les fils, et les construire en appelant buildNode...
    // À vérifier...
    int size = m_nodes.size();
    node.first_child_id = size;
    m_nodes.resize(m_nodes.size() + 2);

    buildNode(2*nodeId+1, start, split_val, level+1, targetCellSize, maxDepth);
    buildNode(2*nodeId+2, split_val, end, level+1, targetCellSize, maxDepth);
  
  }


  // TODO
  // throw RTException("BVH::buildNode not implemented yet.");
}
