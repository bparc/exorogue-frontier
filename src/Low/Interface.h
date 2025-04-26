
// ...

typedef uint32_t hash_t;

struct interface_draw_command_t
{
	uint16_t First;
	uint16_t Last;
	interface_draw_command_t *Next;
};

struct window_settings_t
{
	const char *Title;
	float_t ElementSpacing;
	float_t PaddingX;
	float_t PaddingY;
	float_t MaxRowHeight;
};

struct window_t
{
	rect_t ClientBounds;
	hash_t Name;
	interface_draw_command_t *DrawListHead;
	interface_draw_command_t *DrawListBack;
	bool Inited;

	window_settings_t Settings;

	vec2_t LayoutCursor;
};

struct interface_t
{
	// Windows.

	#define COUNT 32
	array_t<uint16_t,COUNT>SortBuffer;
	window_t Windows[COUNT];
	//uint16_t SortBuffer[COUNT];
	#undef COUNT

	window_t *Wnd; // Currently bound window.

	// Inputs.

	bool InterceptInputs;
	vec2_t MouseCursor;
	bool Interact[2];

	// Interaction state.

	vec2_t ClickOffset; // The place where the cursor was at the moment of interaction.
	hash_t Hovered;
	hash_t Active; // The widget we're currently interacting with.
	hash_t RootWnd;

	// Rendering.

	render_output_t Out; // All widgets are rendered into this output.

	// Internal renderig state.

	bool RenderOutputInvalid;
	command_buffer_t CommandBuffer;
	vertex_t Vertices[MB(10)];
	render_command_t Commands[1024];
	array_t<interface_draw_command_t,256>WindowCommands;
};

static void InitGUI(interface_t *State);

static void BeginGUI(interface_t *State, const input_t *Input, assets_t *Assets);
static void EndGUI(interface_t *State);

static void RenderGUI(interface_t *State, graphics_device_t *Device);

// Windows.

static bool BeginWindow(interface_t *State, hash_t WindowIndex, vec2_t Offset, vec2_t Size, window_settings_t Settings);
static void EndWindow(interface_t *State);

static bool IsHovered(const interface_t *State, rect_t Bb);
static bool IsHovered(const interface_t *State, hash_t ID);
static bool IsActive(const interface_t *State, hash_t ID);
static bool Interact(interface_t *State, rect_t Bb, hash_t ID);

static hash_t HashName(const interface_t *State, const char *Name);

// Widgets.

static void Text(interface_t *State, char* Text, vec2_t Pos, vec4_t Color, float_t Scale);
static bool Button(interface_t *State, const char *Name, vec2_t Pos);
static vec2_t GetNextWidgetPos(interface_t *UI, vec2_t WidgetSize);

// INTERNAL:

static void BindWindow(interface_t *State, window_t *Window);