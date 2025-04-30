// ...

struct transform_t
{
	vec2_t Center; // Center
	vec2_t Size;
};

static transform_t Transform(vec2_t Offset, vec2_t Size);
static vec2_t MinCorner(transform_t Transform);
static vec2_t MaxCorner(transform_t Transform);
static bounds_t GetCameraBounds(transform_t Transform);
static vec2_t MapTo(transform_t Transform, vec2_t Point);

//

struct vertex_t
{
	vec2_t Offset;
	vec2_t TexCoord;
	vec4_t Color;
	vec4_t Attributes;
};

enum primitive_t
{
	Primitive_Quad,
	Primitive_Line,
};

struct render_command_t
{
	uint8_t Primitive;
	uint32_t Texture;
	int32_t Offset;
	int32_t Count;
};

struct command_buffer_t
{
	render_command_t CmdMemory[8];

	int32_t MaxCmdCount;
	int32_t CmdCount;
	render_command_t *Commands;

	int32_t MaxVertexCount;
	int32_t VertexCount;
	vertex_t *Vertices;
};

static void SetupCmdBuffer(command_buffer_t *Cmds, vertex_t *Vertices, int32_t VertexCount, render_command_t *Commands, int32_t CommandCount);
static void SetupCmdBuffer(command_buffer_t *Cmds, memory_t *Memory, int32_t VertexBufferSizeInBytes, int32_t CmdCount = 0);
static void Clear(command_buffer_t *Cmds, int32_t Count);
static void Clear(command_buffer_t *Cmds);

static vertex_t *ReserveVertices(command_buffer_t *Cmds, int32_t Count, primitive_t Prim, uint32_t Texture = 0);
static void AppendQuad(command_buffer_t *Cmds, vec2_t A, vec2_t B, vec2_t C, vec2_t D, vec4_t Color, uint32_t Texture = 0, vec2_t TexCoordMin = {}, vec2_t TexCoordMax = {});
static void AppendQuad(command_buffer_t *Cmds, vec2_t Offset, vec2_t Size, vec4_t Color, uint32_t Texture = 0, vec2_t TexCoordMin = {}, vec2_t TexCoordMax = {});
static void AppendLine(command_buffer_t *Cmds, vec2_t A, vec2_t B, vec4_t Color, uint32_t Texture = 0, vec2_t TexCoordMin = {}, vec2_t TexCoordMax = {});
static render_command_t *AppendCommand(command_buffer_t *Cmds, primitive_t Prim, uint32_t Texture = 0);
//

struct render_output_t
{
	command_buffer_t *Out;
	const assets_t *Assets;

	font_t Font;

	float_t LineThickness; // Thickness = 1.0f + LineThickness
};

static render_output_t RenderTo(command_buffer_t *Out, const assets_t *Assets, font_t Font = NULL);

// Primitive_Quad
static void ClearCommands(render_output_t *Out);
static void DrawRect(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color);
static void DrawRect(render_output_t *Out, rect_t Rectangle, vec4_t Color);
static void DrawPoint(render_output_t *Out, vec2_t Offset, vec4_t Color);

static void DrawBitmap(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec4_t Color = {1, 1, 1, 1});
static void DrawBitmap(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec2_t UVMin, vec2_t UVMax, vec4_t Color = {1, 1, 1, 1});

static void DrawShilouette();

static void DrawString(render_output_t *Out, vec2_t Offset, const char *Text, rect_t Bounds, vec4_t Color = {1, 1, 1, 1}, float_t Scale = 1.0f);
static void DrawString(render_output_t *Out, vec2_t Offset, const char *Text, vec4_t Color = {1, 1, 1, 1}, float_t Scale = 1.0f);

// Scaled
static void DrawBitmapScaled(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec2_t Size, vec4_t Color = {1, 1, 1, 1});
static void DrawBitmapScaled(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec2_t Size, vec2_t UVMin, vec2_t UVMax, vec4_t Color = {1, 1, 1, 1});

// Primitive_Line
static void PushLineThickness(render_output_t *Out, float_t Thickness);
static void DrawLine(render_output_t *Out, vec2_t From, vec2_t To, vec4_t Color);
static void DrawRectOutline(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color);
static void DrawCircleOutline(render_output_t *Out, vec2_t Offset, float_t Radius, vec4_t Color);

// Util
static float_t GetTextWidth(const bmfont_t *Info, const char *Text, float_t Scale);
static float_t GetWordWidth(const bmfont_t *Info, const char *Text, float_t Scale);