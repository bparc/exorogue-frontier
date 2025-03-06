
//

struct automap_input_data_t
{
	uint8_t Attribs;
};

// Patterns

static int32_t MatchCorners(const surface_t *Input, point_t At, enum automap_input_t Color, enum automap_input_t Match, enum tile_palette_t Palette)
{
	int32_t Result = -1;

	if (GetPixel(Input, At) == (uint32_t)Match)
	{
		if (GetPixel(Input, At + Point(0, -1)) == (uint32_t)Color)
		{
			Result = Palette;
		}
	}

	return Result;
}

//

//

enum automap_input_t
{
	MapColor_Grass = 2,// = COLOR(32, 181, 98),
	MapColor_Water = 1,// = COLOR(10, 137, 255),
	MapColor_Stone = 5,// = COLOR(226, 201, 255),
	MapColor_Count,
};

enum tile_palette_t
{
	TilePalette_WaterCliff = 4,
};

static int32_t MatchTileBitmap(const surface_t *Input, point_t At)
{
	int32_t Result = -1;

	#define CORNERS(Color, Match, Palette) Result = MatchCorners(Input, At, Color, Match, Palette)
	
	CORNERS(MapColor_Grass, MapColor_Water, TilePalette_WaterCliff);

	#undef CLIIF

	return Result;
}

static void LoadAutoMapInput(map_t *Map, const char *Dir)
{
	surface_t Input = LoadSurfaceFromDirectory(Dir);

	automap_input_data_t Colors[MapColor_Count] = {};
	Colors[MapColor_Grass] = {TileAttrib_Traversable}; // COLOR(32, 181, 98)
	Colors[MapColor_Water] = {}; // COLOR(10, 137, 255)
	Colors[MapColor_Stone] = {};

	// Convert to the internal color palette.

	#define COLOR(R, G, B) (uint32_t)((0xff << 24) | (B << 16) | (G << 8) | R)

	for (int32_t y = 0; y < Input.Y; y++)
	{
		for (int32_t x = 0; x < Input.X; x++)
		{
			uint8_t TileValue = 0;

			uint32_t Pixel = GetPixel(&Input, x, y);
			uint32_t Color = 0;
			switch (Pixel)
			{
			case COLOR(32, 181, 98): Color = MapColor_Grass; break;
			case COLOR(10, 137, 255): Color = MapColor_Water; break;
			case COLOR(226, 201, 255): Color = MapColor_Stone; break;
			}

			SetPixel(&Input, x, y, Color);
		}
	}

	#undef COLOR

	//

	for (int32_t y = 0; y < Input.Y; y++)
	{
		for (int32_t x = 0; x < Input.X; x++)
		{
			map_tile_t *Tile = GetTile(Map, {x, y});
			if (Tile)
			{
				Tile->ID = (uint8_t)GetPixel(&Input, {x, y});
				Assert(Tile->ID < Len(Colors));
				Tile->Attributes = Colors[Tile->ID].Attribs;

				int32_t ID = MatchTileBitmap(&Input, {x, y});
				if (ID > 0)
				{
					Tile->ID = (uint8_t)ID;
				}
			}
		}
	}

	FreeLoadedSurface(&Input);
}