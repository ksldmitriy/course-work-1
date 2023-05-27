#pragma once
#include "window.hpp"
#include <chrono>

#include "simulation.hpp"
#include "map_borders.hpp"
#include "map_loader.hpp"
#include "vulkan_application.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imconfig.h>
#include <imgui.h>
#include <imgui_internal.h>

using namespace std;

class Application : protected VulkanApplication {
private:
  time_point program_start;
  time_point prev_frame;
  long double time_from_start;
  long double delta_time;
  vector<float> frame_time_history;
  vector<float> fps_history;
  float fps;

  VkRenderPass imgui_render_pass;

  Camera camera;
  bool move_state;

  unique_ptr<MapBorders> map_borders;
  unique_ptr<RoadCollider> road_collider;
  vector<Triangle> map_mesh;
  vector<Line> outer_lines;

  unique_ptr<Simulation> simulation;

  bool draw_rays, draw_borders_kd_tree;
  
  void Prepare();
  void LoadMap();

  void ChangeSufaceCallback();
  
  void RenderLoop();

  void CreateSimulation();
  
  void Update();
  void UpdateTime();
  void UpdateCars();
  
  void LoadSprites();

  void ProcessEvents();
  void ProcessMouseMoveEvent(MouseMoveEvent event);
  void ProcessMouseButtonEvent(MouseButtonEvent event);

  void RenderUI();
  void DrawTestMenu();
  void DrawPerformanceMenu();

public:
  Application() = default;
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;
  ~Application();

  void Run();
};
