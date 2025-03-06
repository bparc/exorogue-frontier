static void RenderView(const map_t *Map, bounds_t ClipPlane, render_output_t *Out, const content_t *Content, float_t ElapsedTime)
{
	DrawRect(Out, ClipPlane.Min, ClipPlane.Max - ClipPlane.Min, RGBA(13, 32, 48));
	DrawString(Out, {16, 5}, "Hello, World!");

	for (int32_t y = 0; y < Map->Y; y++)
	{
		for (int32_t x = 0; x < Map->X; x++)
		{
			point_t Index = Point(x, y);

			const map_tile_t *Tile = GetTile(Map, Index);
			if (Tile)
			{
				int32_t ID = Tile->ID;
				rect_t Bounds = GetTileBounds(Map, Index);

				//

				uint8_t Padding = 2;
				uint8_t GridSize = 8;

				int32_t UVx = (ID - 1);
				vec2_t UVMin = V2(((GridSize + (Padding * 2)) * UVx) + Padding, Padding);
				vec2_t UVMax = UVMin + V2((float_t)GridSize);

				DrawBitmapScaled(Out, Bounds.Offset, Content->TileSetBitmap, Map->TileSize, UVMin, UVMax);
			}
		}
	}

	// Objects

	for (int32_t Index = 0; Index < Map->Objects.Count; Index++)
	{
		const map_object_t *Obj = &Map->Objects.Values[Index];
		
		rect_t TileBounds = GetTileBounds(Map, Obj->OccupiedTile);
		vec2_t BitmapSize = GetSize(Content->Assets, Obj->Type->Animation[0]);
		
		vec2_t Center = RectCenter(TileBounds); //Obj->BitmapOffset;
		vec2_t Alignment = {};

		//if (Obj->RenderFlags & render_flags_CustomBitmapPosition)
		{
			Center = Obj->BitmapOffset;
		}

		switch (Obj->Type->Alignment)
		{
		case Alignment_Mid: Alignment = (BitmapSize * 0.5f); break;
		case Alignment_Bot: Alignment = V2(BitmapSize.x * 0.5f, BitmapSize.y - 3); break;
		}

		float_t Time = fmodf(ElapsedTime, 1.0f);
		int32_t Frame = (int32_t)(Time * Len(Obj->Type->Animation));
		 //DrawRect(Out, TileBounds, ColorRed * 0.6f);
		vec2_t BitmapOffset = (Center - Alignment);
		BitmapOffset.y -= Obj->Height;
			
		DrawRect(Out, TileBounds, ColorYellow);
		DrawRect(Out, Center - V2(2.0f, -1.0f), V2(5.0f, 3.0f), {0.1f, 0.1f, 0.1f, 0.5f});

		DrawBitmap(Out, BitmapOffset, Obj->Type->Animation[Frame]);

		//DrawString(&Out[1], Center, "1234", ColorWhite, 0.18f);
		//DebugRect(BitmapOffset, BitmapSize, ColorRed);
		//DebugPoint(Center, RGBA(255, 0 , 255));

		//DebugPoint(Center, ColorRed);
	}
}