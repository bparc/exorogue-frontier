
struct renderer_data_t
{
	const content_t *Content;
	map_t *Map;

	bounds_t ClipPlane;
	
	// Camera

	transform_t Transform;
	vec4_t Viewport;
};

struct game_renderer_t
{
	command_buffer_t Buffers[1];
	render_output_t Out;

	// Memory
	vertex_t Vertices[65536];
	render_command_t Commands[32];
};

// Main

static void RenderScene(game_renderer_t *Renderer, const renderer_data_t *Data, graphics_device_t *Dev);

// Utils

static vec2_t ScrToIso(vec2_t Coords);
static vec2_t IsoToScr(vec2_t Coords);