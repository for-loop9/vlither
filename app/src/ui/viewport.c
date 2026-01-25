#include "viewport.h"

#include "../user.h"

void ui_viewport_init(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;

  usr->viewport_widget.scene = malloc(ctx->fif * sizeof(VkDescriptorSet));
  for (int i = 0; i < ctx->fif; i++)
    usr->viewport_widget.scene[i] = igImplVulkan_AddTexture(
        usr->r->linear_sampler, usr->r->images[i].color_view_alpha,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void ui_viewport(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  ImGuiStyle* style = igGetStyle();
  
  ImDrawList_AddImage(
      igGetBackgroundDrawList(igGetMainViewport()),
      (ImTextureRef){
          NULL, (ImTextureID)usr->viewport_widget.scene[ctx->current_frame]},
      (ImVec2){}, (ImVec2){env->wnd->size[0], env->wnd->size[1]},
      (ImVec2){0, 0}, (ImVec2){1, 1}, igGetColorU32_Vec4((ImVec4){1, 1, 1, 1}));
}

void ui_viewport_resize(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  renderer_resize(usr->r, ctx, ctx->size);
  for (int i = 0; i < ctx->fif; i++)
    igImplVulkan_RemoveTexture(usr->viewport_widget.scene[i]);
  for (int i = 0; i < ctx->fif; i++)
    usr->viewport_widget.scene[i] = igImplVulkan_AddTexture(
        usr->r->linear_sampler, usr->r->images[i].color_view_alpha,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void ui_viewport_destroy(tenv* env) {
  tuser_data* usr = env->usr;
  free(usr->viewport_widget.scene);
}