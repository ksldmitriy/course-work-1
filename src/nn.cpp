#include "nn.hpp"

NeuralNetwork::NeuralNetwork(NeuralNetworkCreateInfo &create_info) {
  int layers_count = create_info.layers_sizes.size();

  layers.resize(layers_count - 1);
  layers_values.resize(layers_count);

  input_biases.resize(create_info.layers_sizes[0]);

  for (int i = 0; i < layers_count - 1; i++) {
    layers[i] = CreateRandomLayer(create_info.layers_sizes[i + 1],
                                  create_info.layers_sizes[i]);
    layers_values[i].resize(create_info.layers_sizes[i]);
  }

  layers_values[layers_count - 1].resize(create_info.layers_sizes.back());
}

void NeuralNetwork::Run(vector<float> &input, vector<float> &result) {}

NeuralNetwork::Layer NeuralNetwork::CreateRandomLayer(int size, int prev_size) {
  Layer layer;

  layer.weights.resize(size);

  for (int i = 0; i < size; i++) {
    layer.weights[i] = CreateRandomVector(prev_size, -1, 1);
  }

  layer.biases = CreateRandomVector(size, -1, 1);

  return layer;
}

vector<float> NeuralNetwork::CreateRandomVector(int size, float min,
                                                float max) {
  vector<float> vec(size);

  for (auto &i : vec) {
    i = RandomFloat(min, max);
  }

  return vec;
}

float NeuralNetwork::RandomFloat(float min, float max) {
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = max - min;
  float r = random * diff;

  return min + r;
}
