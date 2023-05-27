#pragma once
#include <cstdlib>
#include <span>
#include <vector>

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

  float ActivaionFunction(float x);
  void ApplyActivaionFunction(vector<float>& v);
  void AddBiases(vector<float> &val, vector<float> &biases);
  void ComputeLayer(vector<float> &prev_l, vector<float> &cur_l, Layer &layer);

public:
  NeuralNetwork(NeuralNetworkCreateInfo &create_info);

  void Run();
  vector<float> &GetOutputLayer();
  vector<float> &GetInputLayer();
};
