#pragma once
#include "render_structs.hpp"
#include <vector>
#include <stack>

class RoadCollider {
private:
  struct KDTreeNode {
    Rect rect;
    int childs;
    int duplicates;
    int unique_triangles;

    bool is_final;
    vector<Triangle> triangles;
  };

  vector<KDTreeNode> kd_tree;

  bool IsColliding(Triangle &t, glm::fvec2 pos);

  Rect CalculateTrianglesRect(vector<Triangle> &borders);

  void BuildKDTree(vector<Triangle>& borders);

  void TrySplitKDNode(int node);
  void AddChildNodes(int parent, KDTreeNode &c1, KDTreeNode &c2);

  void SplitKDNode(KDTreeNode &node, KDTreeNode childs[2]);
  void SplitKDNodeByAxis(KDTreeNode &node, KDTreeNode childs[2], int axis);

public:
  RoadCollider(vector<Triangle> &road);
  RoadCollider(RoadCollider &) = delete;
  RoadCollider operator=(RoadCollider &) = delete;

  bool IsInsideRoad(glm::fvec2 pos);
};
