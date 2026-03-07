#include "engine/fontRenderer.h"
#include "engine/stbi_image.h"
#include <cstdio>
#include <glm/ext.hpp>
#include <iostream>

FontRenderer::FontRenderer(MAI::Renderer *render, VkFormat format,
                           GameSettings &gameSettings)
    : ren(render), format(format), settings(gameSettings) {
  loadFonts();
  loadResources();
}

void FontRenderer::loadFonts() {
  FILE *file = fopen(RESOURCES_PATH "latin.fnt", "r");
  if (!file) {
    std::cerr << "Faile to read font file" << std::endl;
    assert(false);
  }

  fseek(file, 0L, SEEK_END);
  const size_t bytesinfile = ftell(file);
  fseek(file, 0L, SEEK_SET);

  char *buffers = (char *)alloca(bytesinfile + 1);
  const size_t bytesread = fread(buffers, 1, bytesinfile, file);
  fclose(file);

  buffers[bytesread] = 0;

  std::string code(buffers);
  std::string line;
  uint32_t kerning = 0;
  for (char c : code) {
    line += c;
    if (c == '\n') {
      if (line.find("chars count") != std::string::npos) {
        int count;
        std::sscanf(line.c_str(), "chars count=%d", &count);
        font_glyphs.reserve(count);
      } else if (line.find("kernings count") != std::string::npos) {
        int count;
        std::sscanf(line.c_str(), "kernings count=%d", &count);
        font_kernings.reserve(count);
      } else if (line.find("char id=") != std::string::npos) {
        int id, x, y, width, height, xoffset, yoffset, xadvance, page;
        std::sscanf(line.c_str(),
                    "char id=%d x=%d y=%d width=%d height=%d "
                    "xoffset=%d yoffset=%d xadvance=%d page=%d ",
                    &id, &x, &y, &width, &height, &xoffset, &yoffset, &xadvance,
                    &page);
        font_glyphs.insert(
            {(uint32_t)id,
             Glyph{id, x, y, width, height, xoffset, yoffset, xadvance, page}});
      } else if (line.find("kerning first") != std::string::npos) {
        int first, second, amount;
        std::sscanf(line.c_str(), "kerning first=%d  second=%d amount=%d",
                    &first, &second, &amount);
        font_kernings.insert({kerning, Kerning{first, second, amount}});
        kerning++;
      }
      line = "";
    }
  }
}

void FontRenderer::loadResources() {
  const stbi_uc *pixels = stbi_load(RESOURCES_PATH "latin_0.png", &atlastWidth,
                                    &atlastHeight, nullptr, 4);
  assert(pixels);
  texture = ren->createImage({
      .type = MAI::TextureType_2D,
      .format = MAI::Format_RGBA_S8,
      .dimensions = {(uint32_t)atlastWidth, (uint32_t)atlastHeight},
      .data = pixels,
      .usage = MAI::Sampled_Bit,
  });

  stbi_image_free((void *)pixels);

  MAI::Shader *vert = ren->createShader(SHADERS_PATH "spvs/font.vspv");
  MAI::Shader *frag = ren->createShader(SHADERS_PATH "spvs/font.fspv");
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
      .sampleCount = settings.getMSAASample(),
  });
  delete vert;
  delete frag;
}

uint32_t id = 0;
FontDrawInfo FontRenderer::populateVertices(const char *text, glm::vec2 pos,
                                            glm::vec3 color) {
  std::vector<FontVertex> vertices;
  std::string str = text;
  float penX = settings.width * pos.x;
  float penY = settings.height * pos.y;
  for (auto &c : str) {
    auto it = font_glyphs.find(c);
    assert(it != font_glyphs.end());
    Glyph &g = it->second;

    float xpos = (penX + g.x_offset);
    float ypos = (penY + g.y_offset);
    float u0 = g.x / (float)atlastWidth;
    float v0 = g.y / (float)atlastHeight;
    float u1 = (g.x + g.width) / (float)atlastWidth;
    float v1 = (g.y + g.height) / (float)atlastHeight;

    // printf("pos: %.2f, %.2f\n", xpos, ypos);
    // printf("uv0: %.2f, %.2f\n", u0, v0);
    // printf("uv1: %.2f, %.2f\n", u1, v1);

    // triangle 0
    vertices.push_back({glm::vec2(xpos, ypos), glm::vec2(u0, v0), color});
    vertices.push_back(
        {glm::vec2(xpos + g.width, ypos), glm::vec2(u1, v0), color});
    vertices.push_back(
        {glm::vec2(xpos + g.width, ypos + g.height), glm::vec2(u1, v1), color});
    // triangle 1
    vertices.push_back({glm::vec2(xpos, ypos), glm::vec2(u0, v0), color});
    vertices.push_back(
        {glm::vec2(xpos + g.width, ypos + g.height), glm::vec2(u1, v1), color});
    vertices.push_back(
        {glm::vec2(xpos, ypos + g.height), glm::vec2(u0, v1), color});
    penX += g.x_advance;
  }

  ++id;

  FontDrawInfo fd{
      .id = id,
      .pos = glm::vec3(0.0f),
      .buffer = ren->createBuffer({
          .usage = MAI::Storage_Buffer,
          .storage = MAI::StorageType_Device,
          .size = sizeof(FontVertex) * vertices.size(),
          .data = vertices.data(),
      }),
      .vertices_size = vertices.size(),
  };
  return fd;
}
void FontRenderer::setText(const char *text, glm::vec2 pos, glm::vec3 color) {
  if (stopInserting)
    return;
  fonts.emplace_back(populateVertices(text, pos, color));
}

void FontRenderer::setDynamicText(MAI::CommandBuffer *buff, const char *text,
                                  const char *id, glm::vec2 pos,
                                  glm::vec3 color) {}

glm::vec3 initialPos = glm::vec3(-90.0f, -40.0f, 0.0f);
void FontRenderer::draw(MAI::CommandBuffer *buff, glm::mat4 pv) {

  struct PushConstant {
    glm::mat4 proj;
    uint32_t textureId;
    uint64_t vertices;
  } pc{
      .proj =
          glm::ortho(0.0f, float(settings.width), 0.0f, float(settings.height)),
      .textureId = texture->getIndex(),
  };

  buff->bindPipeline(pipeline);
  for (auto &f : fonts) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(initialPos));
    model = glm::scale(model, glm::vec3(0.1f));
    pc.proj = pv * model;
    pc.vertices = ren->gpuAddress(f.buffer);

    buff->cmdPushConstant(&pc);
    buff->cmdDraw(f.vertices_size);
  }

  stopInserting = true;
}

void FontRenderer::clearGarbge() {}

FontRenderer::~FontRenderer() {

  for (auto &f : fonts)
    delete f.buffer;

  delete pipeline;
  delete texture;
}
