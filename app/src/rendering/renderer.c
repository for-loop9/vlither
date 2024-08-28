#include "renderer.h"
#include <graphics/ig_buffer.h>
#include <stdlib.h>
#include "../game/game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"

void imgui_init(game* g, ig_context* context, ig_window* window, renderer* renderer) {
	ImGui_ImplVulkan_InitInfo vk_imgui_init_info = {
		.Instance = context->instance,
		.PhysicalDevice = context->gpu,
		.Device = context->device,
		.QueueFamily = context->queue_family,
		.Queue = context->queue,
		.DescriptorPool = context->descriptor_pool,
		.RenderPass = context->default_frame.render_pass,
		.MinImageCount = context->fif,
		.ImageCount = context->fif,
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
		.Subpass = 0
	};
	
	igCreateContext(NULL);
	ImGuiIO* io = igGetIO();
	io->IniFilename = NULL;
	io->LogFilename = NULL;
	ImGuiStyle* style = igGetStyle();
	style->FrameRounding = 15;
	style->PopupRounding = 10;
	style->GrabRounding = 15;
	igStyleColorsClassic(NULL);
	igImplGlfw_InitForVulkan(window->native_handle, 1);
	igImplVulkan_Init(&vk_imgui_init_info);

	renderer->fonts[RENDERER_FONT_BIG] = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "app/res/fonts/Poppins-Regular.ttf", 28, NULL, NULL);
	renderer->fonts[RENDERER_FONT_MED] = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "app/res/fonts/Poppins-Regular.ttf", 24, NULL, NULL);
	renderer->fonts[RENDERER_FONT_MED_BOLD] = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "app/res/fonts/Poppins-Bold.ttf", 24, NULL, NULL);
	renderer->fonts[RENDERER_FONT_SMALL] = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "app/res/fonts/Poppins-Regular.ttf", 20, NULL, NULL);
	renderer->fonts[RENDERER_FONT_SMALL_BOLD] = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "app/res/fonts/Poppins-Bold.ttf", 20, NULL, NULL);

	igImplVulkan_CreateFontsTexture();
}

void imgui_render(_ig_frame* frame) {
	igRender();
	ImDrawData* draw_data = igGetDrawData();
	igImplVulkan_RenderDrawData(draw_data, frame->cmd_buffer, VK_NULL_HANDLE);
}

void imgui_destroy() {
	igImplVulkan_Shutdown();
    igImplGlfw_Shutdown();
    igDestroyContext(NULL);
}

renderer* renderer_create(game* g, ig_context* context, ig_window* window, const ig_texture* sprite_sheet, unsigned int max_circles, unsigned int max_foods, unsigned int max_sprites, ig_texture* font_sheet, ig_texture* bg_tex, unsigned int max_chars) {
	renderer* r = malloc(sizeof(renderer));
	r->context = context;

	float quad_data[] = {
		0, 0,
		0, 1,
		1, 0,
		1, 1,
	};

	r->quad_buffer = ig_context_buffer_create(context, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, quad_data, sizeof(quad_data));
	r->global_buffer = ig_context_dbuffer_create(context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, NULL, sizeof(r->global));
	r->bd_renderer = ig_bd_renderer_create(context, 1);
	r->bg_renderer = sprite_renderer_create(context, bg_tex, 1);
	r->food_renderer = food_renderer_create(context, max_foods);
	r->bp_renderer = bp_renderer_create(context, max_circles);
	r->sprite_renderer = sprite_renderer_create(context, sprite_sheet, max_sprites);
	r->text_renderer = text_renderer_create(context, font_sheet, max_chars);
	r->mm_renderer = mm_renderer_create(context, 1);

	for (int i = 0; i < context->fif; i++) {
		vkUpdateDescriptorSets(context->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = context->global_set[i],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pImageInfo = NULL,
			.pBufferInfo = &(VkDescriptorBufferInfo) {
				.buffer = r->global_buffer[i].buffer,
				.offset = 0,
				.range = VK_WHOLE_SIZE
			},
			.pTexelBufferView = NULL
		}, 0, NULL);
	}

	imgui_init(g, context, window, r);
	return r;
}

void renderer_start_imgui_frame(renderer* renderer) {
	igImplVulkan_NewFrame();
	igImplGlfw_NewFrame();
	igNewFrame();
}

void renderer_push_bg(renderer* renderer, const sprite_instance* bg_instance) {
	sprite_renderer_push(renderer->bg_renderer, bg_instance);
}

void renderer_push_bd(renderer* renderer, const bd_instance* bd_instance) {
	ig_bd_renderer_push(renderer->bd_renderer, bd_instance);
}

void renderer_push_food(renderer* renderer, const food_instance* food_instance) {
	food_renderer_push(renderer->food_renderer, food_instance);
}

void renderer_push_bp(renderer* renderer, const bp_instance* bp_instance) {
	bp_renderer_push(renderer->bp_renderer, bp_instance);
}

void renderer_push_sprite(renderer* renderer, const sprite_instance* sprite_instance) {
	sprite_renderer_push(renderer->sprite_renderer, sprite_instance);
}

void renderer_set_map_data(renderer* renderer, const uint8_t* map_data) {
	mm_renderer_set_map_data(renderer->mm_renderer, map_data);
}

void renderer_push_mm(renderer* renderer, const mm_instance* mm_instance, int size) {
	mm_renderer_push(renderer->mm_renderer, mm_instance, size);
}

void renderer_push_text(renderer* renderer, const char* str, const ig_vec3* transform, const ig_vec4* color, ig_vec3* transform_out) {
	text_renderer_push(renderer->text_renderer, str, transform, color, transform_out);
}

void renderer_flush(renderer* renderer) {
	_ig_frame* frame = renderer->context->frames + renderer->context->frame_idx;

	ig_mat4_ortho(&renderer->global.projection, 0, renderer->context->default_frame.resolution.x, renderer->context->default_frame.resolution.y, 0, 1, -1);
	memcpy(renderer->global_buffer[renderer->context->frame_idx].data, &renderer->global, sizeof(renderer->global));

	if (renderer->mm_renderer->curr_sz) {
		mm_renderer_transfer_map(renderer->mm_renderer, renderer->context, frame);
	}

	vkCmdBeginRenderPass(frame->cmd_buffer, &(VkRenderPassBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = renderer->context->default_frame.render_pass,
		.framebuffer = renderer->context->default_frame.framebuffer,
		.renderArea = {
			.offset = { .x = 0, .y = 0 },
			.extent = {
				.width = renderer->context->default_frame.resolution.x,
				.height = renderer->context->default_frame.resolution.y
			}
		},
		.clearValueCount = 2,
		.pClearValues = (VkClearValue[]) {
			{ .color = { .float32 = { 0.086f, 0.11f, 0.13f, 1.0f } } },
			{ .depthStencil = { .depth = 0, .stencil = 0 }}
		}
	}, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindVertexBuffers(frame->cmd_buffer, 0, 1, &renderer->quad_buffer->buffer, (VkDeviceSize[]) { 0 });
	sprite_renderer_flush(renderer->bg_renderer, renderer->context, frame);
	food_renderer_flush(renderer->food_renderer, renderer->context, frame);
	bp_renderer_flush(renderer->bp_renderer, renderer->context, frame);
	ig_bd_renderer_flush(renderer->bd_renderer, renderer->context, frame);
	mm_renderer_flush(renderer->mm_renderer, renderer->context, frame);
	sprite_renderer_flush(renderer->sprite_renderer, renderer->context, frame);
	text_renderer_flush(renderer->text_renderer, renderer->context, frame);
	imgui_render(frame);
	vkCmdEndRenderPass(frame->cmd_buffer);
}

void renderer_destroy(renderer* renderer) {
	imgui_destroy();

	mm_renderer_destroy(renderer->mm_renderer, renderer->context);
	text_renderer_destroy(renderer->text_renderer, renderer->context);
	sprite_renderer_destroy(renderer->sprite_renderer, renderer->context);
	bp_renderer_destroy(renderer->bp_renderer, renderer->context);
	food_renderer_destroy(renderer->food_renderer, renderer->context);
	sprite_renderer_destroy(renderer->bg_renderer, renderer->context);
	ig_bd_renderer_destroy(renderer->bd_renderer, renderer->context);
	ig_context_dbuffer_destroy(renderer->context, renderer->global_buffer);
	ig_context_buffer_destroy(renderer->context, renderer->quad_buffer);	
	free(renderer);
}
