#pragma once
#include "debug_renderer.hpp"
#include "instance_renderer.hpp"
#include "map_borders.hpp"
#include "nn.hpp"
#include "road_collider.hpp"
#include "cars_physics.hpp"

using namespace std;

struct SimulationRenderers {
  InstanceRenderer *cars_renderer;
  DebugRenderer *debug_renderer;
};

struct SimulationCreateInfo {
  SimulationRenderers renderers;
  MapBorders *map_borders;
  RoadCollider *road_collider;
  float const_delta_time;
};

class Simulation {
private:
  float const_delta_time;

  MapBorders *map_borders;
  RoadCollider *road_collider;

  InstanceRenderer *cars_renderer;
  DebugRenderer *debug_renderer;

  vector<glm::fvec2> rays_layout;

  vector<Line> raycast_lines;

  CarsPhysics cars_physics;
  
  vector<unique_ptr<NeuralNetwork>> neural_networks;
  vector<float> raycast_results;
  vector<Transforn2D> cars_transforms;
  vector<CarControls> cars_controls;
  vector<glm::fvec2> cars_velocity;

  void DeleteCar(int index);

  void CreateRaysLayout();
  void GetRaycatsResultsRange(int index, vector<float>::iterator &first,
                              vector<float>::iterator &last);

  void CreateNeuralNerworks();

  void UpdateRaycast();
  void UpdateMovement();
  void DeleteLosers();

  void CreateRaycastLines();
  void RenderCars();
  void RenderRays();
  void RenderKDBorders();

public:
  Simulation(SimulationCreateInfo &create_info);
  Simulation(Simulation &) = delete;
  Simulation operator=(Simulation &) = delete;

  void Update();
  void Render(bool draw_rays, bool draw_borders_kd_tree);
  void SetRenderers(SimulationRenderers& renderers);
};
