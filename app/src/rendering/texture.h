#ifndef TEXTURE_H
#define TEXTURE_H

#include <thermite.h>

typedef struct texture {
  ivec2 size;
  VkImage image;
  VkImageView view;
  VmaAllocation memory;
} texture;

texture* create_mipmap_texture(tcontext* ctx, const char* filename);
texture* create_minimap_texture(tcontext* ctx, int width);
void destroy_texture(tcontext* ctx, texture* tex);

#endif