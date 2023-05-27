#pragma once
#include "exception.hpp"
#include "logs.hpp"
#include "render_structs.hpp"
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <map>

using namespace std;
namespace fs = filesystem;

class MapLoader {
private:
  struct VertexCache {
    vector<int> triangles;
    vector<int> lines;
  };

  vector<string> raw_lines;

  vector<glm::fvec2> vertices;
  vector<glm::ivec3> indices;

  vector<Triangle> triangles;
  vector<Line> lines;

  vector<Line> outer_lines;

  int HashVec2Indices(glm::ivec2 v);

  void CalculateOutline();
  void CalculateTriangles();

  void ScaleMap(float scale);
  
  void ReadLinesFromFile(fs::path path);
  void ParseLines();
  void ParseVertexLines(string &line);
  void ParseFaceLines(string &line);

public:
  MapLoader() = default;

  void LoadMap(fs::path path, float scale);
  vector<Triangle> GetMesh();
  vector<Line> GetOutline();
};
