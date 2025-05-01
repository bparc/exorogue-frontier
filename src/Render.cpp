
//

static vec2_t ScrToIso(vec2_t Coords)
{
	vec2_t Result = {0};
	Result.x = Dot(Coords, V2(+0.50f,-0.50f));
	Result.y = Dot(Coords, V2(+0.25f,+0.25f));
	return Result;
}

static vec2_t IsoToScr(vec2_t Coords)
{
	vec2_t Result = {0};
	Result.x = Dot(Coords, V2(+2.0f,+4.0f));
	Result.y = Dot(Coords, V2(-2.0f,+4.0f));
	Result = Result * 0.5f;
	return Result;
}

//

static void WriteCommands(game_renderer_t *Renderer, const renderer_data_t *Data)
{
	render_output_t *Out;
	map_t *Map;

	Out = &Renderer->Out;
	Map = Data->Map;

	// Sky

	rect_t SkyBox = Rect(Data->ClipPlane.Min, Data->ClipPlane.Max - Data->ClipPlane.Min);
	DrawRect(Out, SkyBox, RGBA(13, 32, 48));
	
	// Tiles

	for (int32_t y = 0; y < Map->Y; y++)
	{
		for (int32_t x = 0; x < Map->X; x++)
		{
			const map_tile_t *Tile = GetTile(Map, {x, y});

			if (Tile && IsTraversable(Tile))
			{
				rect_t Bounds = GetTileBounds(Map, {x, y});

				vec2_t UVMin = {1, 1};
				vec2_t UVMax = UVMin + V2(32, 32);

				vec4_t Color = V4(1);

				if (Tile->Object != 0)
					Color = ColorRed;

				vec2_t BitmapOffset = ScrToIso(Bounds.Offset + V2(-4.0f, 4.0f));
				DrawBitmapScaled(Out, BitmapOffset, Data->Content->TileSetBitmap, {8.0f, 8.0f}, UVMin, UVMax, Color);
				//DrawRect(Out, BitmapOffset, V2(8.0f, 8.0f), V4(ColorRed, 0.5f));
			}
		}
	}

	// Objects

	for (int32_t Index = 0; Index < Map->Objects.Count; Index++)
	{
		const map_object_t *Obj = &Map->Objects.Values[Index];

		rect_t Bounds = GetTileBounds(Map, Obj->OccupiedTile);
		vec2_t BitmapOffset = ScrToIso(RectCenter(Bounds));
		vec2_t BitmapSize = V2(2.0f, 4.0f);
		DrawRect(Out, BitmapOffset - V2(BitmapSize.x * 0.5f, BitmapSize.y), BitmapSize, ColorYellow);
	}
}

static void RenderScene(game_renderer_t *Renderer, const renderer_data_t *Data, graphics_device_t *Dev)
{
	// Setup
	SetupCmdBuffer(&Renderer->Buffers[0], Renderer->Vertices, Len(Renderer->Vertices), Renderer->Commands, Len(Renderer->Commands));

 	Renderer->Out = RenderTo(&Renderer->Buffers[0], Data->Content->Assets);

 	// Render
	WriteCommands(Renderer, Data);

	Dispatch(Dev, &Renderer->Buffers[0], Data->Viewport, Data->Transform, SHADER_NEAREST_NEIGHBOUR_ANTIALIASING);
}