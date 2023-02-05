# Maillage triangulaire. 

## 1. Calcul d'intersection. 

La première image obtenue permettait de distinguer les trianges composant le personnage. Après avoir calculé les coordonées de texture, l'image obtenue est beaucoup plus lisse :

<p align="center">
    <img src="tw503.png" float="auto">
</p>

Après avoir augmenté le nombre de faces, l'image obtenue est meilleure, mais le temps de rendu augmente considérablement : 

<p align="center">
    <img src="tw.png" float="auto">
</p>

## 2. BVH. 

Notre implémentation de la BVH ne fonctionne pas à cause d'une erreur de segmentation. Voilà ci-après les différentes fonctions qui nous étaient données à écrire.

**Intersection :**

```cpp
void BVH::intersect(const Ray &ray, Hit &hit) const {
  // TODO
  // vérifier si on a bien une intersection (en fonction de tMin, tMax, et
  // hit.t()), et si oui appeler intersecNode...

  float tMin, tMax;
  float t = 0; 

  m_nodes[0].box.rayIntersect(ray, tMin, tMax);

  ray.at(t);

  if (tMin < t && t > tMax ) {
    intersectNode(0, ray, hit);
  }
}
```

**Intersection d'un nœud :**

```cpp
void BVH::intersectNode(int nodeId, const Ray &ray, Hit &hit) const {
  // TODO, deux cas: soit mNodes[nodeId] est une feuille (il faut alors
  // intersecter les triangles du noeud), soit c'est un noeud interne (il faut
  // visiter les fils (ou pas))

  Node node = m_nodes[nodeId];
  int total = node.nb_faces; 

  if (node.is_leaf) {
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
```

**Construction d'un nœud :**

```cpp

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
    int size = m_nodes.size();
    node.first_child_id = size;
    m_nodes.resize(m_nodes.size() + 2);

    buildNode(2*nodeId+1, start, split_val, level+1, targetCellSize, maxDepth);
    buildNode(2*nodeId+2, split_val, end, level+1, targetCellSize, maxDepth);
  
  }
}
```
