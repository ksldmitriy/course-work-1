#include "window.hpp"
#include "logs.hpp"
#include "vk/exception.hpp"
#include "vk/instance.hpp"

bool Window::glfw_inited = false;
map<GLFWwindow *, Window *> Window::windows_db;

Window::Window() {
  if (!glfw_inited) {
    InitGLFW();
  }

  surface = VK_NULL_HANDLE;

  CreateWindow();

  glm::dvec2 cursor_pos;
  glfwGetCursorPos(handle, &cursor_pos.x, &cursor_pos.y);

  MouseState state;
  state.p_pos = cursor_pos;
  state.pos = PixelToRelative(cursor_pos);

  glm::ivec2 window_pos;
  glm::ivec2 window_size;
  glfwGetWindowFrameSize(handle, &window_pos.x, &window_pos.y, &window_size.x,
                         &window_size.y);
  size = window_size - window_pos;

  DEBUG("window created");
}

Window::~Window() {
  if (!handle) {
    return;
  }

  Destroy();
}

void Window::CreateWindow() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  handle = glfwCreateWindow(900, 900, "title", 0, 0);
  if (!handle) {
    throw CriticalException("cant create window");
  }

  glfwSetKeyCallback(handle, StaticKeyCallback);
  glfwSetWindowSizeCallback(handle, StaticResizeCallback);
  glfwSetCursorPosCallback(handle, StaticCursorPositionCallback);
  glfwSetMouseButtonCallback(handle, StaticMouseButtonCallback);

  windows_db[handle] = this;
}

glm::fvec2 Window::PixelToRelative(glm::ivec2 p_pos) {
  glm::fvec2 pos;
  pos.x = p_pos.x / (float)size.x;
  pos.y = p_pos.y / (float)size.y;

  return pos;
}

void Window::AttachInstance(vk::Instance &instance) {
  this->instance = &instance;
}

void Window::Destroy() {
  windows_db.erase(handle);

  glfwDestroyWindow(handle);

  if (surface) {
    DestroySurface();
  }

  handle = nullptr;

  DEBUG("window destroyed");
}

GLFWwindow *Window::GetHandle() { return handle; }

void Window::PollEvents() { glfwPollEvents(); }

bool Window::ShouldClose() { return glfwWindowShouldClose(handle); }

void Window::DestroySurface() {
  vkDestroySurfaceKHR(instance->GetHandle(), surface, nullptr);
  surface = nullptr;
}

VkSurfaceKHR Window::GetSurface() { return surface; }

void Window::CreateSurface() {
  if (surface) {
    WARN("creating new surface without deleting old one");
  }

  VkResult result =
      glfwCreateWindowSurface(instance->GetHandle(), handle, nullptr, &surface);
  if (result) {
    throw CriticalException("cant create window surface");
  }

  DEBUG("surface created");
}

void glfw_error_callback(int error, const char *description) {
  cout << "glfw error " << error << " :" << description << endl;
}

void Window::InitGLFW() {
  glfwSetErrorCallback(glfw_error_callback);

  int result = glfwInit();
  if (!result) {
    cout << "cant init glfw" << endl;
    throw -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
}

void Window::GetInstanceExtensions(const char **&extensions,
                                   uint32_t &extensions_count) {
  extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
}

void Window::AttachImguiIO(ImGuiIO *io) { imgui_io = io; }

MouseState Window::GetMouseState() { return mouse_state; }

void Window::GetEvents(vector<MouseButtonEvent> &key_events,
                       vector<MouseMoveEvent> &move_events) {
  key_events = move(this->mouse_button_events);
  move_events = move(this->mouse_move_events);
}

// callbacks

void Window::ResizeCallback(int width, int height) {
  INFO("window resized to {} {}", width, height);

  size = {width, height};

  mouse_state.pos = PixelToRelative(mouse_state.p_pos);
}

void Window::KeyCallback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(handle, GLFW_TRUE);
    return;
  }
}

void Window::CursorPositionCallback(double x_pos, double y_pos) {
  MouseMoveEvent event;
  event.p_pos = {x_pos, y_pos};
  event.pos = PixelToRelative(event.p_pos);

  event.p_delta = event.p_pos - mouse_state.p_pos;
  event.delta = event.pos - mouse_state.pos;

  mouse_move_events.push_back(event);
}

void Window::MouseButtonCallback(int button, int action, int mods) {
  if (imgui_io->WantCaptureMouse) {
    return;
  }

  MouseButtonEvent event;

  switch (button) {
  case 0:
    event.button = MouseButton::left;
    break;
  case 1:
    event.button = MouseButton::right;
    break;
  case 2:
    event.button = MouseButton::middle;
    break;
  default:
    return;
  };

  event.pressed = action == GLFW_PRESS;

  event.shift_mod = mods & GLFW_MOD_SHIFT;

  mouse_button_events.push_back(event);
}

// static callbacks

void Window::StaticResizeCallback(GLFWwindow *window, int width, int height) {
  if (!windows_db.contains(window)) {
    INFO("no window for resize callback");
    return;
  }

  windows_db[window]->ResizeCallback(width, height);
}

void Window::StaticKeyCallback(GLFWwindow *window, int key, int scancode,
                               int action, int mods) {
  if (!windows_db.contains(window)) {
    INFO("no window for key callback");
    return;
  }

  windows_db[window]->KeyCallback(key, scancode, action, mods);
}
void Window::StaticCursorPositionCallback(GLFWwindow *window, double x_pos,
                                          double y_pos) {
  if (!windows_db.contains(window)) {
    INFO("no window for key callback");
    return;
  }

  windows_db[window]->CursorPositionCallback(x_pos, y_pos);
}

void Window::StaticMouseButtonCallback(GLFWwindow *window, int button,
                                       int action, int mods) {
  if (!windows_db.contains(window)) {
    INFO("no window for resize callback");
    return;
  }

  windows_db[window]->MouseButtonCallback(button, action, mods);
}
