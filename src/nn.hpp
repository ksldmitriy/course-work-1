#pragma once
#include <vector>
#include <cstdlib>

using namespace std;

struct NeuralNetworkCreateInfo {
  vector<int> layers_sizes;
  vector<void (*)(float &)> activation_funcs;
};

class NeuralNetwork {
private:
  struct Layer {
    vector<vector<float>> weights;
    vector<float> biases;
  };

  vector<Layer> layers;
  vector<float> input_biases;
  vector<vector<float>> layers_values;

  Layer CreateRandomLayer(int size, int prev_size);
  vector<float> CreateRandomVector(int size, float min, float max);
  float RandomFloat(float min, float max);

public:
  NeuralNetwork(NeuralNetworkCreateInfo &create_info);

  void Run(vector<float> &input, vector<float> &result);
};
