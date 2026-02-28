#include "engine/Shapes.h"

struct ShapeVertex {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 uv;
};

std::vector<glm::vec3> cubeOnlyVertices = {
    // ===== Front (+Z) =====
    {-0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, 0.5f},

    // ===== Back (-Z) =====
    {0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f},

    // ===== Left (-X) =====
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f},
    {-0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, -0.5f},

    // ===== Right (+X) =====
    {0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, 0.5f},

    // ===== Top (+Y) =====
    {-0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, -0.5f},
    {-0.5f, 0.5f, -0.5f},

    // ===== Bottom (-Y) =====
    {-0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, 0.5f},
    {-0.5f, -0.5f, 0.5f},
};

std::vector<ShapeVertex> cubeVertices = {
    // ===== Front (+Z) =====
    {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 0}},
    {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 0}},
    {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {1, 1}},
    {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 1}},

    // ===== Back (-Z) =====
    {{0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0}},
    {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 0}},
    {{-0.5f, 0.5f, -0.5f}, {0, 0, -1}, {1, 1}},
    {{0.5f, 0.5f, -0.5f}, {0, 0, -1}, {0, 1}},

    // ===== Left (-X) =====
    {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 0}},
    {{-0.5f, -0.5f, 0.5f}, {-1, 0, 0}, {1, 0}},
    {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}, {0, 1}},

    // ===== Right (+X) =====
    {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {0, 0}},
    {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 0}},
    {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {1, 1}},
    {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {0, 1}},

    // ===== Top (+Y) =====
    {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 0}},
    {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {1, 0}},
    {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1}},

    // ===== Bottom (-Y) =====
    {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 0}},
    {{0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0}},
    {{0.5f, -0.5f, 0.5f}, {0, -1, 0}, {1, 1}},
    {{-0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0, 1}},
};

std::vector<uint16_t> cubeIndices = {
    0,  1,  2,  2,  3,  0,  // Front
    4,  5,  6,  6,  7,  4,  // Back
    8,  9,  10, 10, 11, 8,  // Left
    12, 13, 14, 14, 15, 12, // Right
    16, 17, 18, 18, 19, 16, // Top
    20, 21, 22, 22, 23, 20  // Bottom
};

void generateSphere(float radius, uint32_t rings, uint32_t sectors,
                    std::vector<ShapeVertex> &vertices,
                    std::vector<uint16_t> &indices) {
  float const R = 1.0f / (float)(rings - 1);
  float const S = 1.0f / (float)(sectors - 1);

  for (uint32_t r = 0; r < rings; ++r) {
    for (uint32_t s = 0; s < sectors; ++s) {
      float y = sin(-M_PI_2 + M_PI * r * R);
      float x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
      float z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

      ShapeVertex v;
      v.pos = glm::vec3(x, y, z) * radius;
      v.norm = glm::normalize(glm::vec3(x, y, z));
      v.uv = glm::vec2(s * S, r * R);

      vertices.push_back(v);
    }
  }

  for (uint32_t r = 0; r < rings - 1; ++r) {
    for (uint32_t s = 0; s < sectors - 1; ++s) {
      indices.push_back(r * sectors + s);
      indices.push_back(r * sectors + (s + 1));
      indices.push_back((r + 1) * sectors + (s + 1));

      indices.push_back(r * sectors + s);
      indices.push_back((r + 1) * sectors + (s + 1));
      indices.push_back((r + 1) * sectors + s);
    }
  }
}

Shape Shapes::getCube(MAI::Renderer *ren, bool onlyPos) {
  Shape shape;

  if (!onlyPos) {
    shape.vertBuffer = ren->createBuffer({
        .usage = MAI::StorageBuffer,
        .storage = MAI::StorageType_Device,
        .size = sizeof(ShapeVertex) * cubeVertices.size(),
        .data = cubeVertices.data(),
    });

  } else {
    shape.vertBuffer = ren->createBuffer({
        .usage = MAI::StorageBuffer,
        .storage = MAI::StorageType_Device,
        .size = sizeof(glm::vec3) * cubeOnlyVertices.size(),
        .data = cubeOnlyVertices.data(),
    });
  }

  shape.indexBuffer = ren->createBuffer({
      .usage = MAI::IndexBuffer,
      .storage = MAI::StorageType_Device,
      .size = sizeof(uint16_t) * cubeIndices.size(),
      .data = cubeIndices.data(),
  });

  shape.indicesSize = (uint32_t)cubeIndices.size();

  return shape;
}

Shape Shapes::getSphere(MAI::Renderer *ren) {
  std::vector<ShapeVertex> vertices;
  std::vector<uint16_t> indices;
  generateSphere(1.0, 128, 128, vertices, indices);

  Shape shape;

  shape.vertBuffer = ren->createBuffer({
      .usage = MAI::StorageBuffer,
      .storage = MAI::StorageType_Device,
      .size = sizeof(ShapeVertex) * vertices.size(),
      .data = vertices.data(),
  });

  shape.indexBuffer = ren->createBuffer({
      .usage = MAI::IndexBuffer,
      .storage = MAI::StorageType_Device,
      .size = sizeof(ShapeVertex) * indices.size(),
      .data = indices.data(),
  });

  shape.indicesSize = (uint32_t)indices.size();

  return shape;
}

Shape Shapes::getCylinder(MAI::Renderer *ren, float radius, float height,
                          int segments) {
  float half = height * 0.5f;

  std::vector<glm::vec3> vertices;
  std::vector<uint16_t> indices;

  // ===== SIDE VERTICES =====
  for (int i = 0; i <= segments; i++) {
    float a = (i / (float)segments) * glm::two_pi<float>();
    float x = radius * cos(a);
    float z = radius * sin(a);

    vertices.push_back(glm::vec3(x, -half, z)); // bottom
    vertices.push_back(glm::vec3(x, half, z));  // top
  }

  // ===== SIDE INDICES =====
  for (int i = 0; i < segments; i++) {
    uint32_t k = i * 2;

    indices.push_back(k);
    indices.push_back(k + 1);
    indices.push_back(k + 2);

    indices.push_back(k + 1);
    indices.push_back(k + 3);
    indices.push_back(k + 2);
  }

  // ===== TOP CENTER =====
  uint32_t topCenterIndex = vertices.size();
  vertices.push_back(glm::vec3(0, half, 0));

  // ===== BOTTOM CENTER =====
  uint32_t bottomCenterIndex = vertices.size();
  vertices.push_back(glm::vec3(0, -half, 0));

  // ===== CAPS =====
  for (int i = 0; i < segments; i++) {
    uint32_t k = i * 2;

    // Top
    indices.push_back(topCenterIndex);
    indices.push_back(k + 3);
    indices.push_back(k + 1);

    // Bottom
    indices.push_back(bottomCenterIndex);
    indices.push_back(k);
    indices.push_back(k + 2);
  }

  Shape sh;
  sh.vertBuffer = ren->createBuffer({
      .usage = MAI::Storage_Buffer,
      .storage = MAI::StorageType_Device,
      .size = vertices.size() * sizeof(glm::vec3),
      .data = vertices.data(),
  });
  sh.indexBuffer = ren->createBuffer({
      .usage = MAI::IndexBuffer,
      .storage = MAI::StorageType_Device,
      .size = indices.size() * sizeof(uint16_t),
      .data = indices.data(),
  });
  sh.indicesSize = indices.size();

  return sh;
}

Shape Shapes::getCone(MAI::Renderer *ren, float radius, float height,
                      int segments) {
  float half = height * 0.5f;
  std::vector<glm::vec3> vertices;
  std::vector<uint16_t> indices;

  // ===== TIP =====
  uint32_t tipIndex = 0;
  vertices.push_back(glm::vec3(0, half, 0));

  // ===== BASE RING =====
  for (int i = 0; i <= segments; i++) {
    float a = (i / (float)segments) * glm::two_pi<float>();
    vertices.push_back(glm::vec3(radius * cos(a), -half, radius * sin(a)));
  }

  // ===== SIDE INDICES =====
  for (int i = 1; i <= segments; i++) {
    indices.push_back(tipIndex);
    indices.push_back(i);
    indices.push_back(i + 1);
  }

  // ===== BASE CENTER =====
  uint32_t baseCenterIndex = vertices.size();
  vertices.push_back(glm::vec3(0, -half, 0));

  // ===== BASE INDICES =====
  for (int i = 1; i <= segments; i++) {
    indices.push_back(baseCenterIndex);
    indices.push_back(i + 1);
    indices.push_back(i);
  }

  Shape sh;
  sh.vertBuffer = ren->createBuffer({
      .usage = MAI::Storage_Buffer,
      .storage = MAI::StorageType_Device,
      .size = vertices.size() * sizeof(glm::vec3),
      .data = vertices.data(),
  });
  sh.indexBuffer = ren->createBuffer({
      .usage = MAI::IndexBuffer,
      .storage = MAI::StorageType_Device,
      .size = indices.size() * sizeof(uint16_t),
      .data = indices.data(),
  });
  sh.indicesSize = indices.size();

  return sh;
}

void Shapes::deleteShape(Shape &shape) {
  delete shape.vertBuffer;
  delete shape.indexBuffer;
}
