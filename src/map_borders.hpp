#pragma once
#include "glm/gtx/transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "render_structs.hpp"
#include <limits>
#include <span>
#include <iostream>

using namespace std;

class MapBorders {
private:
  vector<Line> borders;
  vector<glm::fvec2> rays_layout;

  float Raycast(glm::fvec2 ray, glm::fvec2 pos);
  float Raycast(glm::fvec2 ray, glm::fvec2 pos, Line line);

public:
  MapBorders(vector<Line> &borders);
  MapBorders(MapBorders &) = delete;
  MapBorders operator=(MapBorders &) = delete;

  void Raycast(Transforn2D transform, span<float> &raycats_results);
  void LoadRaysLayout(vector<glm::fvec2> rays_layout);
};
