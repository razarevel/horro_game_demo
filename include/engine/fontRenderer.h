#pragma once
#include "engine/imageLoader.h"
#include "engine/mai_config.h"
#include "engine/mai_vk.h"
#include "engine/scene/game_settings.h"

#include <glm/glm.hpp>

struct FontVertex {
  glm::vec2 pos;
  glm::vec2 vu;
  glm::vec3 color;
};

struct Glyph {
  int codepoint;
  int x, y;
  int width, height;
  int x_offset, y_offset;
  int x_advance;
  int page_id;
};

struct Kerning {
  int first;
  int second;
  int amount;
};

struct DynamicText {
  MAI::Buffer *buffer;
  size_t vertices_size;
};

struct FontDrawInfo {
  uint32_t id;
  glm::vec3 pos;
  MAI::Buffer *buffer;
  size_t vertices_size;
};

struct FontRenderer {
  FontRenderer(MAI::Renderer *ren, VkFormat format, GameSettings &gameSettings);
  ~FontRenderer();

  void setText(const char *text, glm::vec2 pos,
               glm::vec3 color = glm::vec3(1.0f));

  void setDynamicText(MAI::CommandBuffer *buff, const char *text,
                      const char *id, glm::vec2 pos, glm::vec3 color);

  void draw(MAI::CommandBuffer *buff, glm::mat4 pv);

  void clearGarbge();

private:
  MAI::Renderer *ren = nullptr;
  GameSettings settings;

  MAI::Pipeline *pipeline;
  MAI::Texture *texture;
  MAI::Buffer *buffer;
  VkFormat format;

  std::vector<FontDrawInfo> fonts;

  std::unordered_map<uint32_t, Glyph> font_glyphs;
  std::unordered_map<uint32_t, Kerning> font_kernings;

  bool stopInserting = false;

  int atlastWidth;
  int atlastHeight;

  void loadFonts();
  void loadResources();
  FontDrawInfo populateVertices(const char *text, glm::vec2 pos,
                                glm::vec3 color);
};
