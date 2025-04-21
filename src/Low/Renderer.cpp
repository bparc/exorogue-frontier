
//

static transform_t Transform(vec2_t Offset, vec2_t Size)
{
	transform_t Result = {};
	Result.Center = Offset;
	Result.Size = Size;
	return Result;
}

static vec2_t MinCorner(transform_t Transform)
{
	vec2_t Result = Transform.Center - (Transform.Size * 0.5f);
	return Result;
}

static vec2_t MaxCorner(transform_t Transform)
{
	vec2_t Result = Transform.Center + (Transform.Size * 0.5f);
	return Result;
}

static bounds_t GetCameraBounds(transform_t Transform)
{
	bounds_t Result = {};
	Result.Min = MinCorner(Transform);
	Result.Max = MaxCorner(Transform);
	return Result;
}

static vec2_t MapTo(transform_t Transform, vec2_t Point)
{
	vec2_t Result = (Transform.Center - (Transform.Size * 0.5f)) - Point;
	return Result;
}

// ...

static void Setup(command_buffer_t *Cmds, vertex_t *Vertices, int32_t VertexCount, render_command_t *Commands, int32_t CommandCount)
{
	memset(Cmds, 0, sizeof(*Cmds));
	Cmds->MaxCmdCount = CommandCount;
	Cmds->Commands = Commands;
	Cmds->MaxVertexCount = VertexCount;
	Cmds->Vertices = Vertices;
}

static void Setup(command_buffer_t *Cmds, memory_t *Memory, int32_t VertexBufferSizeInBytes, int32_t CmdCount)
{
	memset(Cmds, 0, sizeof(*Cmds));

	Cmds->MaxVertexCount = VertexBufferSizeInBytes / sizeof(*Cmds->Vertices);
	Cmds->Vertices = (vertex_t *)Push(Memory, Cmds->MaxVertexCount * sizeof(*Cmds->Vertices));

	if (Cmds->CmdCount)
	{
		Cmds->MaxCmdCount = CmdCount;
		Cmds->Commands = (render_command_t *)Push(Memory, Cmds->MaxCmdCount * sizeof(*Cmds->Commands));
	}
	else
	{
		Cmds->MaxCmdCount = Len(Cmds->CmdMemory);
		Cmds->Commands = Cmds->CmdMemory;
	}
}

static void Clear(command_buffer_t *Cmds, int32_t Count)
{
	for (int32_t Index = 0; Index < Count; Index++)
	{
		Clear(&Cmds[Index]);
	}
}

static void Clear(command_buffer_t *Cmds)
{
	Cmds->VertexCount = 0;
	Cmds->CmdCount = 0;
}

static render_command_t *TryMerge(const command_buffer_t *Cmds, int32_t Count, primitive_t Prim, uint32_t Texture)
{
	render_command_t *Result = 0;
	if (Cmds->CmdCount > 0)
	{
		render_command_t *Top = &Cmds->Commands[Cmds->CmdCount - 1];
		if ((Top->Texture == Texture) && (Top->Primitive == Prim))
		{
			Result = Top;
		}
	}
	return Result;
}

static vertex_t *ReserveVertices(command_buffer_t *Cmds, int32_t Count, primitive_t Prim, uint32_t Texture)
{
	vertex_t *Result = NULL;
	render_command_t *Command;

	Command = TryMerge(Cmds, Count, Prim, Texture);
	
	if (!Command) // Couldn't merge...
	{
		Command = AppendCommand(Cmds, Prim, Texture);
	}
	
	if (Command && (Cmds->VertexCount + Count) < Cmds->MaxVertexCount) // Reserve...
	{
		Result = &Cmds->Vertices[Cmds->VertexCount];
		Cmds->VertexCount += Count;
		Command->Count += Count;
	}

	return Result;
}

static render_command_t *AppendCommand(command_buffer_t *Cmds, primitive_t Prim, uint32_t Texture)
{
	render_command_t *Command = 0;
	if (Cmds->CmdCount < Cmds->MaxCmdCount)
	{
		Command = &Cmds->Commands[Cmds->CmdCount++];
		memset(Command, 0, sizeof(*Command));
		Command->Primitive = (uint8_t)Prim;
		Command->Texture = Texture;
		Command->Offset = Cmds->VertexCount;
	}
	return Command;
}

static void AppendQuad(command_buffer_t *Cmds, vec2_t A, vec2_t B, vec2_t C, vec2_t D, vec4_t Color, uint32_t Texture, vec2_t TexCoordMin, vec2_t TexCoordMax)
{
	vertex_t *Vertices = ReserveVertices(Cmds, 4, Primitive_Quad, Texture);
	if (Vertices)
	{
		Vertices[0].Offset = A;
		Vertices[1].Offset = B;
		Vertices[2].Offset = C;
		Vertices[3].Offset = D;
		
		Vertices[0].TexCoord = {TexCoordMin.x, TexCoordMin.y};
		Vertices[1].TexCoord = {TexCoordMax.x, TexCoordMin.y};
		Vertices[2].TexCoord = {TexCoordMax.x, TexCoordMax.y};
		Vertices[3].TexCoord = {TexCoordMin.x, TexCoordMax.y};

		Vertices[0].Color = Color;
		Vertices[1].Color = Color;
		Vertices[2].Color = Color;
		Vertices[3].Color = Color;

		Vertices[0].Attributes = {};
		Vertices[1].Attributes = {};
		Vertices[2].Attributes = {};
		Vertices[3].Attributes = {};
	}
}

static void AppendQuad(command_buffer_t *Cmds, vec2_t Offset, vec2_t Size, vec4_t Color, uint32_t Texture, vec2_t TexCoordMin, vec2_t TexCoordMax)
{
	vec2_t Min, Max;
	Min = Offset;
	Max = Offset + Size;

	AppendQuad(Cmds, {Min.x, Min.y}, {Max.x, Min.y}, {Max.x, Max.y}, {Min.x, Max.y}, Color, Texture, TexCoordMin, TexCoordMax);
}

static void AppendLine(command_buffer_t *Cmds, vec2_t A, vec2_t B, vec4_t Color, uint32_t Texture, vec2_t TexCoordMin, vec2_t TexCoordMax)
{
	vertex_t *Vertices = ReserveVertices(Cmds, 2, Primitive_Line, Texture);
	if (Vertices)
	{
		Vertices[0].Offset = A;
		Vertices[1].Offset = B;
		Vertices[0].Color = Color;
		Vertices[1].Color = Color;
		Vertices[0].TexCoord = TexCoordMin;
		Vertices[1].TexCoord = TexCoordMax;
		Vertices[0].Attributes = {};
		Vertices[1].Attributes = {};
	}
}

static render_output_t RenderTo(command_buffer_t *Out, const assets_t *Assets, font_t Font)
{
	render_output_t Result = {};
	Result.Assets = Assets;
	Result.Font = Font;
	Result.Out = Out;
	return Result;
}

static void RenderTexturedQuad(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color, texture_resource_t Texture, vec2_t TexCoordMin, vec2_t TexCoordMax)
{
	vec2_t TexCoordScale = V2(Texture.X, Texture.Y);

	TexCoordMin = TexCoordMin / TexCoordScale;
	TexCoordMax = TexCoordMax / TexCoordScale;

	AppendQuad(Out->Out, Offset, Size, Color, Texture.Handle, TexCoordMin, TexCoordMax);
}

static void RenderTexturedQuad(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color, texture_resource_t Texture)
{
	RenderTexturedQuad(Out, Offset, Size, Color, Texture, {}, V2(Texture.X, Texture.Y));
}

static void RenderTexturedQuad(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color, texture_resource_t Texture, rect_t TexCoords)
{
	RenderTexturedQuad(Out, Offset, Size, Color, Texture, TexCoords.Offset, TexCoords.Offset + TexCoords.Size);
}

static void DrawRect(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color)
{
	asset_bitmap_t *Asset = GetBitmap(Out->Assets, Out->Assets->None);
	if (Asset)
	{
		RenderTexturedQuad(Out, Offset, Size, Color, Out->Assets->Cache.Texture, Asset->Min + V2(2, 2), Asset->Max - V2(2, 2));
	}
}

static void DrawRect(render_output_t *Out, rect_t Rectangle, vec4_t Color)
{
	DrawRect(Out, Rectangle.Offset, Rectangle.Size, Color);
}

static void DrawRectOutline(render_output_t *Out, vec2_t Offset, vec2_t Size, vec4_t Color)
{
	vec2_t Min = Offset;
	vec2_t Max = Offset + Size;
	DrawLine(Out, {Min.x, Min.y}, {Max.x, Min.y}, Color); 
	DrawLine(Out, {Max.x, Min.y}, {Max.x, Max.y}, Color); 
	DrawLine(Out, {Max.x, Max.y}, {Min.x, Max.y}, Color); 
	DrawLine(Out, {Min.x, Min.y}, {Min.x, Max.y}, Color); 
}

static void DrawCircleOutline(render_output_t *Out, vec2_t Offset, float_t Radius, vec4_t Color)
{
	int32_t PointCount = 32;
	for (int32_t Index = 0; Index < PointCount; Index++)
	{
		float_t T0 = (float_t)(Index + 0) / (float_t)PointCount;
		float_t T1 = (float_t)(Index + 1) / (float_t)PointCount;
		vec2_t P0 = {Sin(T0), Cos(T0)};
		vec2_t P1 = {Sin(T1), Cos(T1)};

		P0 = (P0 * Radius) + Offset;
		P1 = (P1 * Radius) + Offset;

		DrawLine(Out, P0, P1, Color);
	}
}

static void DrawBitmap(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec4_t Color)
{
	asset_bitmap_t *Asset = GetBitmap(Out->Assets, Bitmap);
	if (Asset)
	{
		RenderTexturedQuad(Out, Offset, Asset->Max - Asset->Min, Color, Out->Assets->Cache.Texture, Asset->Min, Asset->Max);
	}
}

static void DrawBitmap(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec2_t UVMin, vec2_t UVMax, vec4_t Color)
{
	asset_bitmap_t *Asset = GetBitmap(Out->Assets, Bitmap);
	if (Asset)
	{
		vec2_t Min = Asset->Min + UVMin;
		vec2_t Max = Min + UVMax;
		Min = Clamp(Min, Asset->Min, Asset->Max);
		Max = Clamp(Max, Asset->Min, Asset->Max);
		RenderTexturedQuad(Out, Offset, Asset->Max - Asset->Min, Color, Out->Assets->Cache.Texture, Min, Max);
	}
}

static void DrawBitmapScaled(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec2_t Size, vec2_t UVMin, vec2_t UVMax, vec4_t Color)
{
	asset_bitmap_t *Asset = GetBitmap(Out->Assets, Bitmap);
	if (Asset)
	{
		vec2_t Min = Asset->Min + UVMin;
		vec2_t Max = Min + (UVMax - UVMin);
		Min = Clamp(Min, Asset->Min, Asset->Max);
		Max = Clamp(Max, Asset->Min, Asset->Max);
		RenderTexturedQuad(Out, Offset, Size, Color, Out->Assets->Cache.Texture, Min, Max);
	}
}

static void DrawBitmapScaled(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, vec2_t Size, vec4_t Color)
{
	asset_bitmap_t *Asset = GetBitmap(Out->Assets, Bitmap);
	if (Asset)
	{
		RenderTexturedQuad(Out, Offset, Size, Color, Out->Assets->Cache.Texture, Asset->Min, Asset->Max);
	}
}

static void RenderChar(render_output_t *Out, vec2_t *Offset, bitmap_t Bitmap, const bmfont_char_t *Ch, vec4_t Color, float_t Scale)
{
	vec2_t TexCoordMin = V2(Ch->X, Ch->Y);
	vec2_t TexCoordMax = TexCoordMin + V2(Ch->Width, Ch->Height);

	vec2_t QuadSize = V2(Ch->Width, Ch->Height) * Scale;
	vec2_t QuadOffset = *Offset;

	QuadOffset.x += Ch->XOffset * Scale;
	QuadOffset.y += Ch->YOffset * Scale;

	DrawBitmapScaled(Out, QuadOffset, Bitmap, QuadSize, TexCoordMin, TexCoordMax, Color);
	Offset->x += Ch->XAdvance * Scale;
}

static void _RenderString(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, const bmfont_t *Info, vec4_t Color,
	const char *Text, rect_t Bounds, float_t Scale = 1.0f)
{
	const char *At = Text;
	Color = Color * Color.w;
	Offset.x = Bounds.Offset.x;

	vec2_t Pen = Offset;
	float StartX = Bounds.Offset.x;
	float Cursor = 0.0f;
	float MaxWidth = Bounds.Size.x;
	float LineHeight = Info->Common.LineHeight * Scale;

	while (*At)
	{
		uint8_t Codepoint = (uint8_t)*At++;

		if (Codepoint == '\n') {
			Pen.x = StartX;
			Pen.y += LineHeight;
			Cursor = 0;
			continue;
		}

		const bmfont_char_t *Ch = &Info->Chars[Codepoint];
		float_t AdvanceX = Ch->XAdvance * Scale;

		if (Cursor + AdvanceX > MaxWidth) {
			Pen.x = StartX;
			Pen.y += LineHeight;
			Cursor = 0;
		}

		RenderChar(Out, &Pen, Bitmap, Ch, Color, Scale);

		Cursor = Pen.x - StartX;
	}
}

static void _RenderString(render_output_t *Out, vec2_t Offset, bitmap_t Bitmap, const bmfont_t *Info, vec4_t Color, const char *Text, float_t Scale = 1.0f)
{
	const char *At = Text;
	Color = Color * Color.w;

	while (*At)
	{
		uint8_t Codepoint = (uint8_t)*At++;
		const bmfont_char_t *Ch = &Info->Chars[Codepoint];
		RenderChar(Out, &Offset, Bitmap, Ch, Color, Scale);
	}
}

static float_t getTextWidth(bmfont_t *Info, const char *Text, float_t Scale)
{
	const char *At = Text;
	float_t CursorX = 0.0f;

	while (*At) {
		uint8_t Codepoint = *At++;
		const bmfont_char_t *Ch = &Info->Chars[Codepoint];

		float_t AdvanceX = Ch->XAdvance * Scale;
		CursorX += AdvanceX;
	}

	return CursorX;
}

static void DrawString(render_output_t *Out, vec2_t Offset, const char *Text, rect_t Bounds, vec4_t Color, float_t Scale)
{
	font_t Font = Out->Font ? Out->Font : Out->Assets->FontDefault;
	if (Font)
	{
		//Offset.x = floorf(Offset.x);
		//Offset.y = floorf(Offset.y);

		_RenderString(Out, Offset - (V2(1, 1) * Scale), Font->Bitmap, &Font->Info, V4(ColorBlack, Color.w), Text, Bounds, Scale);
		_RenderString(Out, Offset, Font->Bitmap, &Font->Info, Color, Text, Bounds, Scale);
	}
}

static void DrawString(render_output_t *Out, vec2_t Offset, const char *Text, vec4_t Color, float_t Scale)
{
	font_t Font = Out->Font ? Out->Font : Out->Assets->FontDefault;
	if (Font)
	{
		//Offset.x = floorf(Offset.x);
		//Offset.y = floorf(Offset.y);

		_RenderString(Out, Offset - (V2(1, 1) * Scale), Font->Bitmap, &Font->Info, V4(ColorBlack, Color.w), Text, Scale);
		_RenderString(Out, Offset, Font->Bitmap, &Font->Info, Color, Text, Scale);
	}
}

static void DrawLine(render_output_t *Out, vec2_t From, vec2_t To, vec4_t Color)
{
	AppendLine(Out->Out, From, To, Color);	
}

static void DrawPoint(render_output_t *Out, vec2_t Offset, vec4_t Color)
{
	DrawRect(Out, Offset - V2(0.5f), V2(1), Color);
}

static void PushLineThickness(render_output_t *Out, float_t Thickness)
{
	Out->LineThickness = (-1.0f + Thickness);
}