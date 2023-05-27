#pragma once
#include "glm/gtx/transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "render_structs.hpp"
#include <iostream>
#include <limits>
#include <span>

using namespace std;

class MapBorders {
private:
  struct KDTreeNode {
    Rect rect;
    int childs;
	int duplicates;
	int unique_lines;

    bool is_final;
    vector<Line> lines;
  };

  vector<KDTreeNode> kd_tree;

  vector<glm::fvec2> rays_layout;

  float Raycast(glm::fvec2 ray, glm::fvec2 pos);
  __attribute__((always_inline)) inline float
  Raycast(glm::fvec2 ray, glm::fvec2 pos, Line line);

  Rect CalculateBordersRect(vector<Line> &borders);

  void BuildKDTree(vector<Line> borders);

  void TrySplitKDNode(int node);
  void AddChildNodes(int parent, KDTreeNode &c1, KDTreeNode &c2);

  void SplitKDNode(KDTreeNode &node, KDTreeNode childs[2]);
  void SplitKDNodeByAxis(KDTreeNode &node, KDTreeNode childs[2], int axis);
  
public:
  MapBorders(vector<Line> &borders);
  MapBorders(MapBorders &) = delete;
  MapBorders operator=(MapBorders &) = delete;

  vector<Rect> GetKDRects();
  void Raycast(Transforn2D transform, span<float> &raycats_results);
  void LoadRaysLayout(vector<glm::fvec2> rays_layout);
};
