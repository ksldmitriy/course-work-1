#include "road_collider.hpp"

RoadCollider::RoadCollider(vector<Triangle> &road) { BuildKDTree(road); }

bool RoadCollider::IsInsideRoad(glm::fvec2 pos) {
  stack<int> nodes_to_check;
  nodes_to_check.push(0);

  while (!nodes_to_check.empty()) {
    int node_i = nodes_to_check.top();
    nodes_to_check.pop();

    KDTreeNode &node = kd_tree[node_i];

    if (node.is_final) {
      for (auto &t : node.triangles) {
        if (IsColliding(t, pos)) {
          return true;
        }
      }

    } else {
      nodes_to_check.push(node.childs);
      nodes_to_check.push(node.childs + 1);
    }
  }

  return false;
}

bool RoadCollider::IsColliding(Triangle &t, glm::fvec2 pos) {
  double denominator = ((t.v[1].y - t.v[2].y) * (t.v[0].x - t.v[2].x) +
                        (t.v[2].x - t.v[1].x) * (t.v[0].y - t.v[2].y));
  double a = ((t.v[1].y - t.v[2].y) * (pos.x - t.v[2].x) +
              (t.v[2].x - t.v[1].x) * (pos.y - t.v[2].y)) /
             denominator;
  double b = ((t.v[2].y - t.v[0].y) * (pos.x - t.v[2].x) +
              (t.v[0].x - t.v[2].x) * (pos.y - t.v[2].y)) /
             denominator;

  double c = 1 - a - b;

  return a >= 0 && b >= 0 && c >= 0;
};

void RoadCollider::BuildKDTree(vector<Triangle> &triangles) {
  KDTreeNode initial_node;
  initial_node.triangles = triangles;
  initial_node.is_final = true;
  initial_node.unique_triangles = triangles.size();
  initial_node.duplicates = 0;
  initial_node.rect = CalculateTrianglesRect(triangles);

  kd_tree.push_back(initial_node);

  TrySplitKDNode(0);
}

void RoadCollider::TrySplitKDNode(int node_i) {
  KDTreeNode &node = kd_tree[node_i];
  if (node.triangles.size() < 3) {
    return;
  }

  KDTreeNode childs[2];
  SplitKDNode(node, childs);

  int min_unique = min(childs[0].unique_triangles, childs[1].unique_triangles);

  if (min_unique < 3) {
    return;
  }

  AddChildNodes(node_i, childs[0], childs[1]);
  TrySplitKDNode(kd_tree[node_i].childs + 1);

  TrySplitKDNode(kd_tree[node_i].childs);
}

void RoadCollider::SplitKDNode(KDTreeNode &node, KDTreeNode childs[2]) {
  KDTreeNode x_split[2];
  KDTreeNode y_split[2];

  SplitKDNodeByAxis(node, x_split, 0);
  SplitKDNodeByAxis(node, y_split, 1);

  double x_unique = x_split[0].unique_triangles + x_split[1].unique_triangles;
  double y_unique = y_split[0].unique_triangles + y_split[1].unique_triangles;

  if (x_unique > y_unique) {
    childs[0] = move(x_split[0]);
    childs[1] = move(x_split[1]);
  } else {
    childs[0] = move(y_split[0]);
    childs[1] = move(y_split[1]);
  }
}

void RoadCollider::SplitKDNodeByAxis(KDTreeNode &node, KDTreeNode childs[2],
                                     int axis) {
  float middle_line = 0;
  for (auto &l : node.triangles) {
    glm::fvec2 center = (l.v[0] + l.v[1] + l.v[2]) / 3.f;
    middle_line += center[axis];
  }
  middle_line /= node.triangles.size();

  KDTreeNode empty_child_node;
  empty_child_node.is_final = true;
  empty_child_node.rect = node.rect;
  empty_child_node.duplicates = node.duplicates;
  empty_child_node.unique_triangles = 0;
  childs[0] = empty_child_node;
  childs[1] = empty_child_node;

  if (axis) {
    childs[1].rect.bottom_right[axis] = middle_line;
    childs[0].rect.top_left[axis] = middle_line;
  } else {
    childs[0].rect.bottom_right[axis] = middle_line;
    childs[1].rect.top_left[axis] = middle_line;
  }

  for (auto &t : node.triangles) {
    int left_side_points = 0;
    for (auto &p : t.v) {
      left_side_points += (int)(p[axis] < middle_line);
    }

    if (left_side_points == 0) {
      childs[1].triangles.push_back(t);
      childs[1].unique_triangles++;

    } else if (left_side_points == 3) {
      childs[0].triangles.push_back(t);
      childs[0].unique_triangles++;

    } else {

      childs[0].triangles.push_back(t);
      childs[1].triangles.push_back(t);

      childs[0].duplicates++;
      childs[0].duplicates++;
    }
  }
}

void RoadCollider::AddChildNodes(int parent, KDTreeNode &c1, KDTreeNode &c2) {
  KDTreeNode &parent_node = kd_tree[parent];
  int childs_pos = kd_tree.size();
  parent_node.childs = childs_pos;
  parent_node.triangles.clear();
  parent_node.is_final = false;

  kd_tree.push_back(move(c1));
  kd_tree.push_back(move(c2));
}

Rect RoadCollider::CalculateTrianglesRect(vector<Triangle> &triangles) {
  Rect rect;
  rect.bottom_right = triangles[0].v[0];
  rect.top_left = triangles[0].v[0];

  for (auto &l : triangles) {
    for (auto p : l.v) {
      rect.bottom_right = {max(rect.bottom_right.x, p.x),
                           min(rect.bottom_right.y, p.y)};
      rect.top_left = {min(rect.top_left.x, p.x), max(rect.top_left.y, p.y)};
    }
  }

  return rect;
}
