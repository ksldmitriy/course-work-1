#include "main.hpp"
#include "logs.hpp"

// implement stb image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
  setup_logs();

  try {
    Application application;
    application.Run();
  } catch (IException &e) {
    ERROR("application exit afer unhandled excpetion: {0}", (string)e);
    return -1;
  };

  INFO("program finished");
}
