#include "road_collider.hpp"

RoadCollider::RoadCollider(vector<Triangle> road) { this->road = road; }

bool RoadCollider::IsInsideRoad(glm::fvec2 pos) {
  for (auto &t : road) {
    if (IsColliding(t, pos)) {
      return true;
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
