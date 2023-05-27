#include "map_borders.hpp"

MapBorders::MapBorders(vector<Line> &borders) { this->borders = borders; }

void MapBorders::Raycast(Transforn2D transform,
                         span<float> &raycats_results) {
  float rot_sin = sin(transform.rot);
  float rot_cos = cos(transform.rot);

  glm::fmat2x2 rot_mat = {{rot_cos, -rot_sin}, {rot_sin, rot_cos}};

  for (int r = 0; r < rays_layout.size(); r++) {
    glm::vec2 ray = rays_layout[r] * rot_mat;
    raycats_results[r] = Raycast(ray, transform.pos);
  }
}

float MapBorders::Raycast(glm::fvec2 ray, glm::fvec2 pos) {
  float raycast = numeric_limits<float>::max();

  for (auto b : borders) {
    raycast = min(Raycast(ray, pos, b), raycast);
  }

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
