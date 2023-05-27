#include "map_loader.hpp"

void MapLoader::LoadMap(fs::path path, float scale) {
  ReadLinesFromFile(path);
  ParseLines();

  ScaleMap(scale);

  CalculateTriangles();
  CalculateOutline();
}

vector<Triangle> MapLoader::GetMesh() { return triangles; }

vector<Line> MapLoader::GetOutline() { return outer_lines; }

void MapLoader::ScaleMap(float scale) {
  for (auto &v : vertices) {
    v *= scale;
  }
}

void MapLoader::CalculateTriangles() {
  triangles.resize(indices.size());

  for (int i = 0; i < indices.size(); i++) {
    glm::ivec3 ind = indices[i];
    Triangle t;

    t.v[2] = vertices[ind.x - 1];
    t.v[1] = vertices[ind.y - 1];
    t.v[0] = vertices[ind.z - 1];

    triangles[i] = t;
  }
}

void MapLoader::CalculateOutline() {
  vector<glm::ivec2> line_indx;

  for (int i = 0; i < indices.size(); i++) {
    glm::ivec3 ind = indices[i];

    line_indx.push_back({ind.x, ind.y});
    line_indx.push_back({ind.z, ind.y});
    line_indx.push_back({ind.x, ind.z});
  }

  unordered_map<int, char> dups;
  for (int i = 0; i < line_indx.size(); i++) {
    int hash_code = HashVec2Indices(line_indx[i]);

    auto it = dups.find(hash_code);
    if (it != dups.end()) {
      dups.erase(it);
    } else {
      dups[hash_code] = 0;
    }
  }

  outer_lines.clear();

  for (int i = 0; i < line_indx.size(); i++) {
    int hash_code = HashVec2Indices(line_indx[i]);

    auto it = dups.find(hash_code);
    if (it != dups.end()) {
      outer_lines.push_back(
          {vertices[line_indx[i].x - 1], vertices[line_indx[i].y - 1]});
    }
  }
};

int MapLoader::HashVec2Indices(glm::ivec2 v) {
  int min_i = min(v.x, v.y);
  int max_i = max(v.x, v.y);
  int hash_code = min_i + max_i * (vertices.size() * 10);

  return hash_code;
}

void MapLoader::ParseLines() {
  for (auto l : raw_lines) {
    char type = l[0];

    if (type == 'v') {
      ParseVertexLines(l);
    } else if (type == 'f') {
      ParseFaceLines(l);
    }
  }
}

void MapLoader::ParseVertexLines(string &line) {
  stringstream ss(line);
  ss.seekg(2);

  glm::fvec2 v;
  float dump;

  ss >> v.x >> dump >> v.y;

  vertices.push_back(v);
}

void MapLoader::ParseFaceLines(string &line) {
  stringstream ss(line);
  ss.seekg(2);

  glm::ivec3 i;

  ss >> i.x >> i.y >> i.z;

  indices.push_back(i);
}

void MapLoader::ReadLinesFromFile(fs::path path) {
  ifstream file(path);
  if (!file) {
    throw CriticalException("cant open map file");
  }

  raw_lines.clear();
  string buf;

  while (getline(file, buf)) {
    raw_lines.push_back(buf);
  }

  file.close();
}
