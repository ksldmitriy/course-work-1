#pragma once
#include "debug_renderer.hpp"
#include "instance_renderer.hpp"
#include "map_borders.hpp"
#include "nn.hpp"
#include "road_collider.hpp"
#include "cars_physics.hpp"
#include "cars_ai.hpp"

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

  struct CarResult{
	shared_ptr<NeuralNetwork> nn;
	float result;
  };

  int time_from_start;
  vector<CarResult> car_results;
  float last_best_result = 0;
  
  CarsAI cars_ai;

  CarsPhysics cars_physics;
  
  vector<shared_ptr<NeuralNetwork>> neural_networks;
  vector<float> raycast_results;
  vector<float> cars_distance;
  vector<float> cars_rotation;
  vector<CarControls> cars_controls;
  vector<Transforn2D> cars_transforms;
  vector<glm::fvec2> cars_velocity;

  void DeleteCar(int index);
  float EstimateCarResults(int index);
  
  void CreateRaysLayout();
  void GetRaycatsResultsRange(int index, vector<float>::iterator &first,
                              vector<float>::iterator &last);

  void CreateNeuralNerworks();

  void UpdateRaycast();
  void UpdateControls();
  void UpdateMovement();
  void DeleteLosers();

  void CreateRaycastLines();
  void RenderCars();
  void RenderRays();
  void RenderKDBorders();

  shared_ptr<NeuralNetwork> GetChampion();
  
public:
  Simulation(SimulationCreateInfo &create_info);
  Simulation(Simulation &) = delete;
  Simulation operator=(Simulation &) = delete;

  void RestartSimulation(int cars_count, float mutation);
  void Update();
  void Render(bool draw_rays, bool draw_borders_kd_tree);
  void SetRenderers(SimulationRenderers& renderers);
  float GetBestResult();
};
