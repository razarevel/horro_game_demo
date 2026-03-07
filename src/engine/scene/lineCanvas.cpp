#include "engine/scene/lineCanvas.h"

void LineCanvas3D::line(const vec3 &p1, const vec3 &p2, const vec4 &c) {
  lines_.push_back({.pos = vec4(p1, 1.0f), .color = c});
  lines_.push_back({.pos = vec4(p2, 1.0f), .color = c});
}

void LineCanvas3D::plane(const glm::vec3 &o, const glm::vec3 &v1,
                         const glm::vec3 &v2, int n1, int n2, float s1,
                         float s2, const glm::vec4 &color,
                         const glm::vec4 &outlineColor) {
  line(o - s1 / 2.0f * v1 - s2 / 2.0f * v2, o - s1 / 2.0f * v1 + s2 / 2.0f * v2,
       outlineColor);
  line(o + s1 / 2.0f * v1 - s2 / 2.0f * v2, o + s1 / 2.0f * v1 + s2 / 2.0f * v2,
       outlineColor);

  line(o - s1 / 2.0f * v1 + s2 / 2.0f * v2, o + s1 / 2.0f * v1 + s2 / 2.0f * v2,
       outlineColor);
  line(o - s1 / 2.0f * v1 - s2 / 2.0f * v2, o + s1 / 2.0f * v1 - s2 / 2.0f * v2,
       outlineColor);

  for (int i = 1; i < n1; i++) {
    float t = ((float)i - (float)n1 / 2.0f) * s1 / (float)n1;
    const vec3 o1 = o + t * v1;
    line(o1 - s2 / 2.0f * v2, o1 + s2 / 2.0f * v2, color);
  }

  for (int i = 1; i < n2; i++) {
    const float t = ((float)i - (float)n2 / 2.0f) * s2 / (float)n2;
    const vec3 o2 = o + t * v2;
    line(o2 - s1 / 2.0f * v1, o2 + s1 / 2.0f * v1, color);
  }
}

void LineCanvas3D::box(const mat4 &m, const vec3 &size, const vec4 &c) {
  vec3 pts[8] = {
      vec3(+size.x, +size.y, +size.z), vec3(+size.x, +size.y, -size.z),
      vec3(+size.x, -size.y, +size.z), vec3(+size.x, -size.y, -size.z),
      vec3(-size.x, +size.y, +size.z), vec3(-size.x, +size.y, -size.z),
      vec3(-size.x, -size.y, +size.z), vec3(-size.x, -size.y, -size.z),
  };

  for (auto &p : pts)
    p = vec3(m * vec4(p, 1.f));

  line(pts[0], pts[1], c);
  line(pts[2], pts[3], c);
  line(pts[4], pts[5], c);
  line(pts[6], pts[7], c);

  line(pts[0], pts[2], c);
  line(pts[1], pts[3], c);
  line(pts[4], pts[6], c);
  line(pts[5], pts[7], c);

  line(pts[0], pts[4], c);
  line(pts[1], pts[5], c);
  line(pts[2], pts[6], c);
  line(pts[3], pts[7], c);
}

void LineCanvas3D::box(const mat4 &m, const BoundingBox &box, const vec4 &c) {
  this->box(m * glm::translate(mat4(1.f), .5f * (box.min_ + box.max_)),
            0.5f * vec3(box.max_ - box.min_), c);
}

void LineCanvas3D::frustum(const mat4 &camView, const mat4 &camProj,
                           const vec4 &color) {
  const vec3 corners[] = {vec3(-1, -1, -1), vec3(+1, -1, -1), vec3(+1, +1, -1),
                          vec3(-1, +1, -1), vec3(-1, -1, +1), vec3(+1, -1, +1),
                          vec3(+1, +1, +1), vec3(-1, +1, +1)};

  vec3 pp[8];

  for (int i = 0; i < 8; i++) {
    vec4 q =
        glm::inverse(camView) * glm::inverse(camProj) * vec4(corners[i], 1.0f);
    pp[i] = vec3(q.x / q.w, q.y / q.w, q.z / q.w);
  }
  line(pp[0], pp[4], color);
  line(pp[1], pp[5], color);
  line(pp[2], pp[6], color);
  line(pp[3], pp[7], color);
  // near
  line(pp[0], pp[1], color);
  line(pp[1], pp[2], color);
  line(pp[2], pp[3], color);
  line(pp[3], pp[0], color);
  // x
  line(pp[0], pp[2], color);
  line(pp[1], pp[3], color);
  // far
  line(pp[4], pp[5], color);
  line(pp[5], pp[6], color);
  line(pp[6], pp[7], color);
  line(pp[7], pp[4], color);
  // x
  line(pp[4], pp[6], color);
  line(pp[5], pp[7], color);

  const vec4 gridColor = color * 0.7f;
  const int gridLines = 100;

  // bottom
  {
    vec3 p1 = pp[0];
    vec3 p2 = pp[1];
    const vec3 s1 = (pp[4] - pp[0]) / float(gridLines);
    const vec3 s2 = (pp[5] - pp[1]) / float(gridLines);
    for (int i = 0; i != gridLines; i++, p1 += s1, p2 += s2)
      line(p1, p2, gridColor);
  }
  // top
  {
    vec3 p1 = pp[2];
    vec3 p2 = pp[3];
    const vec3 s1 = (pp[6] - pp[2]) / float(gridLines);
    const vec3 s2 = (pp[7] - pp[3]) / float(gridLines);
    for (int i = 0; i != gridLines; i++, p1 += s1, p2 += s2)
      line(p1, p2, gridColor);
  }
  // left
  {
    vec3 p1 = pp[0];
    vec3 p2 = pp[3];
    const vec3 s1 = (pp[4] - pp[0]) / float(gridLines);
    const vec3 s2 = (pp[7] - pp[3]) / float(gridLines);
    for (int i = 0; i != gridLines; i++, p1 += s1, p2 += s2)
      line(p1, p2, gridColor);
  }
  // right
  {
    vec3 p1 = pp[1];
    vec3 p2 = pp[2];
    const vec3 s1 = (pp[5] - pp[1]) / float(gridLines);
    const vec3 s2 = (pp[6] - pp[2]) / float(gridLines);
    for (int i = 0; i != gridLines; i++, p1 += s1, p2 += s2)
      line(p1, p2, gridColor);
  }
}

void LineCanvas3D::render(MAI::Renderer *ren, VkFormat format,
                          MAI::CommandBuffer *buff) {
  if (lines_.empty())
    return;

  if (pipeline == nullptr) {
    MAI::Shader *vert = ren->createShader(SHADERS_PATH "spvs/canva.vspv");
    MAI::Shader *frag = ren->createShader(SHADERS_PATH "spvs/canva.fspv");
    pipeline = ren->createPipeline({
        .vert = vert,
        .frag = frag,
        .color =
            {
                .blendEnable = true,
                .srcColorBlend = MAI::Src_Alpha,
                .dstColorBlend = MAI::Minus_Src_Alpha,
            },
        .depthFormat = format,
        .cullMode = MAI::Back,
        .topology = MAI::Line_List,
    });
    delete vert;
    delete frag;
  }

  const uint32_t requiredSize = lines_.size() * sizeof(LineData);

  if (!linesBuffer_) {
    linesBuffer_ = ren->createBuffer({
        .usage = MAI::Storage_Buffer,
        .storage = MAI::StorageType_Device,
        .size = requiredSize,
        .data = lines_.data(),
    });
  }

  struct PushConstant {
    glm::mat4 mvp;
    uint64_t addr;
  } pc = {
      .mvp = mvp_,
      .addr = ren->gpuAddress(linesBuffer_),
  };

  buff->bindPipeline(pipeline);
  buff->cmdPushConstant(&pc);
  buff->cmdDraw(lines_.size());
}

void LineCanvas3D::freeResoucs() {
  if (pipeline)
    delete pipeline;
  if (linesBuffer_)
    delete linesBuffer_;
}
