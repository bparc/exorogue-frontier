//
struct camera_t
{
	vec2_t Center;
	vec2_t Size;
	float_t Zoom;
};

static camera_t CreateCamera(vec2_t Size);
static float_t GetCameraScaleFactor(const camera_t *Camera);
static transform_t GetCameraTransform(const camera_t *Camera);
static bounds_t GetCameraBounds(const camera_t *Camera);

//

struct action_t
{
	object_t User;
	point_t Source;
	point_t Dest;
	float_t Time;

	bool Fence;
};

struct active_unit_t
{
	object_t Self;
	bool Bot;
};

struct game_state_t
{
	const content_t *Content;
	float_t ElapsedTime;
	memory_t Memory;

	input_t Input;
	camera_t Camera;
	map_t Map;

	//
	object_t Player;
	
	// Interface
	interface_t GUI;

	// Rendering
	game_renderer_t Renderer;

	//command_buffer_t CommandBuffers[1];
};

// ...


static void InitializeGame(game_state_t *State, const content_t *Content);
static void RunFrame(game_state_t *State, const input_t *Input, graphics_device_t *Device, const camera_t *DebugCamera);

static camera_t GetActiveCamera(game_state_t *State);

//