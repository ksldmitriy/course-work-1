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

void NeuralNetwork::Run() {
  AddBiases(layers_values[0], input_biases);
  ApplyActivaionFunction(layers_values.front());

  for (int i = 0; i < layers.size(); i++) {
    copy(layers[i].biases.begin(), layers[i].biases.end(),
         layers_values[i].begin());

    ComputeLayer(layers_values[i], layers_values[i + 1], layers[i]);

	ApplyActivaionFunction(layers_values[i]);
  }
}

void NeuralNetwork::ApplyActivaionFunction(vector<float> &v) {
  for (auto &i : v) {
    i = ActivaionFunction(i);
  }
}

void NeuralNetwork::ComputeLayer(vector<float> &prev_l, vector<float> &cur_l,
                                 Layer &layer) {
  for (int i = 0; i < cur_l.size(); i++) {
    for (int j = 0; j < prev_l.size(); j++) {
      cur_l[i] += prev_l[j] * layer.weights[i][j];
    }
  }
}

void NeuralNetwork::AddBiases(vector<float> &val, vector<float> &biases) {
  for (int i = 0; i < val.size(); i++) {
    val[i] += biases[i];
  }
}

float NeuralNetwork::ActivaionFunction(float x) { return x / (1 + abs(x)); }

vector<float> &NeuralNetwork::GetOutputLayer() { return layers_values.back(); }

vector<float> &NeuralNetwork::GetInputLayer() { return layers_values.front(); }

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
