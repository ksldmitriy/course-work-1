#pragma once
#include "render_structs.hpp"
#include <vector>

class RoadCollider{
private:
  vector<Triangle> road;

  bool IsColliding(Triangle& t, glm::fvec2 pos);
  
public:
  RoadCollider(vector<Triangle> road);
  RoadCollider(RoadCollider&) = delete;
  RoadCollider operator=(RoadCollider&) = delete;

  bool IsInsideRoad(glm::fvec2 pos);
};
