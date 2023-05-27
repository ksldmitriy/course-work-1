#include "map_borders.hpp"

MapBorders::MapBorders(vector<Line> &borders) { BuildKDTree(borders); }

void MapBorders::Raycast(Transforn2D transform, span<float> &raycats_results) {
  float rot_sin = sinf(transform.rot);
  float rot_cos = cosf(transform.rot);

  glm::fmat2x2 rot_mat = {{rot_cos, -rot_sin}, {rot_sin, rot_cos}};

  for (int r = 0; r < rays_layout.size(); r++) {
    glm::vec2 ray = rays_layout[r] * rot_mat;
    raycats_results[r] = Raycast(ray, transform.pos);
  }
}

Rect MapBorders::CalculateBordersRect(vector<Line> &borders) {
  Rect rect;
  rect.bottom_right = borders[0].end;
  rect.top_left = borders[0].end;

  for (auto &l : borders) {
    glm::fvec2 p = l.start;
    rect.bottom_right = {max(rect.bottom_right.x, p.x),
                         min(rect.bottom_right.y, p.y)};
    rect.top_left = {min(rect.top_left.x, p.x), max(rect.top_left.y, p.y)};

    p = l.end;
    rect.bottom_right = {max(rect.bottom_right.x, p.x),
                         min(rect.bottom_right.y, p.y)};
    rect.top_left = {min(rect.top_left.x, p.x), max(rect.top_left.y, p.y)};
  }

  return rect;
}

float MapBorders::Raycast(glm::fvec2 ray, glm::fvec2 pos) {
  float raycast = numeric_limits<float>::max();

  // for (auto b : borders) {
  //   raycast = min(Raycast(ray, pos, b), raycast);
  // }

  return raycast;
}

float MapBorders::Raycast(glm::fvec2 ray, glm::fvec2 pos, Line line) {
  glm::fvec2 v1 = pos - line.start;
  glm::fvec2 v2 = line.end - line.start;
  glm::fvec2 v3 = {-ray.y, ray.x};

  float dot = glm::dot(v2, v3);
  if (abs(dot) < 0.000001) {
    return numeric_limits<float>::max();
  }

  float t1 = glm::cross(glm::fvec3(v2, 0), glm::fvec3(v1, 0)).z / dot;
  float t2 = glm::dot(v1, v3) / dot;

  if (t1 >= 0.0 && (t2 >= 0.0 && t2 <= 1.0))
    return t1;

  return numeric_limits<float>::max();
}

void MapBorders::LoadRaysLayout(vector<glm::fvec2> rays_layout) {
  this->rays_layout = rays_layout;
}

void MapBorders::BuildKDTree(vector<Line> borders) {
  KDTreeNode initial_node;
  initial_node.lines = borders;
  initial_node.is_final = true;
  initial_node.unique_lines = borders.size();
  initial_node.duplicates = 0;
  initial_node.rect = CalculateBordersRect(borders);

  kd_tree.push_back(initial_node);

  TrySplitKDNode(0);
}

void MapBorders::TrySplitKDNode(int node_i) {
  KDTreeNode &node = kd_tree[node_i];
  if (node.lines.size() < 3) {
    cout << "-split" << endl;
    return;
  }

  KDTreeNode childs[2];
  SplitKDNode(node, childs);

  int min_unique = min(childs[0].unique_lines, childs[1].unique_lines);

  if (min_unique < 4) {
    return;
  }

  AddChildNodes(node_i, childs[0], childs[1]);

  TrySplitKDNode(kd_tree[node_i].childs);
  TrySplitKDNode(kd_tree[node_i].childs + 1);
}

vector<Rect> MapBorders::GetKDRects() {
  vector<Rect> rects;

  for (auto &n : kd_tree) {
    if (n.is_final) {
      rects.push_back(n.rect);
    }
  }

  return rects;
}

void MapBorders::SplitKDNode(KDTreeNode &node, KDTreeNode childs[2]) {
  KDTreeNode x_split[2];
  KDTreeNode y_split[2];

  SplitKDNodeByAxis(node, x_split, 0);
  SplitKDNodeByAxis(node, y_split, 1);

  double x_unique = x_split[0].unique_lines + x_split[1].unique_lines;
  double y_unique = y_split[0].unique_lines + y_split[1].unique_lines;

  if (x_unique > y_unique) {
    childs[0] = move(x_split[0]);
    childs[1] = move(x_split[1]);
  } else {
    childs[0] = move(y_split[0]);
    childs[1] = move(y_split[1]);
  }
}

void MapBorders::SplitKDNodeByAxis(KDTreeNode &node, KDTreeNode childs[2],
                                   int axis) {
  float middle_line = 0;
  for (auto &l : node.lines) {
    glm::fvec2 center = (l.end + l.start) / 2.0f;
    middle_line += center[axis];
  }
  middle_line /= node.lines.size();

  KDTreeNode empty_child_node;
  empty_child_node.is_final = true;
  empty_child_node.rect = node.rect;
  empty_child_node.duplicates = node.duplicates;
  empty_child_node.unique_lines = 0;
  childs[0] = empty_child_node;
  childs[1] = empty_child_node;

  if (axis) {
    childs[1].rect.bottom_right[axis] = middle_line;
    childs[0].rect.top_left[axis] = middle_line;
  } else {
    childs[0].rect.bottom_right[axis] = middle_line;
    childs[1].rect.top_left[axis] = middle_line;
  }

  for (auto &l : node.lines) {
    float min_c = min(l.start[axis], l.end[axis]);
    float max_c = max(l.start[axis], l.end[axis]);

    if (min_c <= middle_line && middle_line <= max_c) {
      childs[0].lines.push_back(l);
      childs[1].lines.push_back(l);

      childs[0].duplicates++;
      childs[0].duplicates++;
    } else if (min_c < middle_line) {
      childs[0].lines.push_back(l);
      childs[0].unique_lines++;
    } else {
      childs[1].lines.push_back(l);
      childs[1].unique_lines++;
    }
  }
}

void MapBorders::AddChildNodes(int parent, KDTreeNode &c1, KDTreeNode &c2) {
  KDTreeNode &parent_node = kd_tree[parent];
  int childs_pos = kd_tree.size();
  parent_node.childs = childs_pos;
  parent_node.lines.clear();
  parent_node.is_final = false;

  kd_tree.push_back(move(c1));
  kd_tree.push_back(move(c2));
}
