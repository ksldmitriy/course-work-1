#include "tools.hpp"

namespace tools {

vector<const char *> string_vector_to_c_array(vector<string> &array) {
  vector<const char *> result(array.size());

  for (int i = 0; i < array.size(); i++) {
    result[i] = array[i].c_str();
  }

  return result;
}

} // namespace tools
