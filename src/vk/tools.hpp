#pragma once
#include <string>
#include <vector>

using namespace std;

namespace tools {
vector<const char *> string_vector_to_c_array(vector<string> &array);

template <typename T> T align_up(T number, T alignment) {
  return ((number + alignment - 1) / alignment) * alignment;
}

template <typename T> T align_down(T number, T alignment) {
  if (number % alignment == 0) {
    return number;
  }

  return align_up(number, alignment) - alignment;
}

} // namespace tools
