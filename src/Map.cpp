// map_object_t

// map_t

bool IsEmpty(const map_tile_t *Tile)
{
	bool Result = Tile ? (Tile->ID == 0) : false;
	return Result;
}

bool IsTraversable(const map_tile_t *Tile)
{
	bool Result = Tile ? (Tile->Attributes & TileAttrib_Traversable) : false;
	return Result;
}

static void CreateMap(map_t *Map, int32_t X, int32_t Y, map_tile_t *Tiles, int32_t TileSize)
{
	memset(Map, 0, sizeof(*Map));

	Map->TileSize = V2((float_t)TileSize);
	Map->Tiles = Tiles;
	Map->X = X;
	Map->Y = Y;

	for (int32_t y = 0; y < Map->Y; y++)
	{
		for (int32_t x = 0; x < Map->X; x++)
		{
			map_tile_t *Tile = GetTile(Map, {x, y});
			Tile->Attributes = TileAttrib_Traversable;
		}
	}
}

static bool CheckBounds(const map_t *Map, point_t Offset)
{
	bool Result = (Offset.x >= 0) && (Offset.y >= 0) &&
		(Offset.x < Map->X) && (Offset.y < Map->Y);
	return Result;
}

static const map_tile_t *GetTile(const map_t *Map, point_t Offset)
{
	const map_tile_t *Result = 0;
	if (CheckBounds(Map, Offset))
	{
		Result = &Map->Tiles[Offset.y * Map->X + Offset.x];
	}
	return Result;
}

static map_tile_t *GetTile(map_t *Map, point_t Offset)
{
	map_tile_t *Result = 0;
	if (CheckBounds(Map, Offset))
	{
		Result = &Map->Tiles[Offset.y * Map->X + Offset.x];
	}
	return Result;
}

static uint32_t GetTileValue(const map_t *Map, point_t Offset)
{
	uint32_t Result = 0;
	const map_tile_t *Tile = GetTile(Map, Offset);
	if (Tile)
	{
		Result = Tile->ID;
	}
	return Result;
}

static void SetTileValue(map_t *Map, point_t Offset, int32_t Value)
{
	map_tile_t *Tile = GetTile(Map, Offset);
	if (Tile && !Tile->Object)
	{
		Tile->ID = (uint8_t)Value;
	}
}

static rect_t GetTileBounds(const map_t *Map, point_t Offset)
{
	rect_t Result = {};
	Result.Size = Map->TileSize;
	Result.Offset = CellToCoords(Map, Offset);
	return Result;
}

static vec2_t GetTileCenter(const map_t *Map, point_t Offset)
{
	vec2_t Result = RectCenter(GetTileBounds(Map, Offset));
	return Result;
}

static vec2_t CellToCoords(const map_t *Map, point_t Offset)
{
	vec2_t Result = V2(Offset) * Map->TileSize;
	return Result;
}

static point_t CoordsToCell(const map_t *Map, vec2_t Coords)
{
	point_t Result = {};
	Result.x = (int32_t)floorf(Coords.x / Map->TileSize.x);
	Result.y = (int32_t)floorf(Coords.y / Map->TileSize.y);
	return Result;
}

static map_object_t *CreateMapObject(map_t *Map, point_t At, const object_behaviour_t *Behaviour)
{
	map_object_t *Result = 0;;

	map_tile_t *Tile = GetTile(Map, At);
	if (Tile && !Tile->Object)
	{
		Result = Map->Objects.Push();
	}

	if (Result)
	{
		Assert((Map->TotalObjectCount <= (UINT16_MAX - 1)));
		Result->Self = (object_t)(++Map->TotalObjectCount);
		Result->OccupiedTile = At;
		Result->Type = Behaviour ? Behaviour : &Map->None;

		Assert(Result->Self < Len(Map->ObjectLookasideTable));
		Map->ObjectLookasideTable[Result->Self] = (uint16_t)Map->Objects.Count;
		
		Result->BitmapOffset = RectCenter(GetTileBounds(Map, Result->OccupiedTile));

		Tile->Object = Result->Self;
	}
	return Result;
}

static map_object_t *GetMapObject(map_t *Map, object_t Object)
{
	map_object_t *Result = 0;

	uint16_t Reference = Map->ObjectLookasideTable[Object];
	if (Reference > 0)
	{
		Result = &Map->Objects[Reference - 1];
	}

	return Result;
}

static map_object_t *GetMapObject(map_t *Map, point_t Offset)
{
	map_object_t *Result = 0;
	map_tile_t *Tile = GetTile(Map, Offset);
	if (Tile)
	{
		Result = GetMapObject(Map, Tile->Object);
	}
	return Result;
}

static bool Translate(map_t *Map, map_object_t *Object, point_t At)
{	
	bool Result = false;

	map_tile_t *Prev, *Next;

	Prev = GetTile(Map, Object->OccupiedTile);
	Next = GetTile(Map, At);

	if (Prev && (Next && !Next->Object))
	{
		Object->OccupiedTile = At;

		Prev->Object = 0;
		Next->Object = Object->Self;

		Result = true;
	}

	return Result;
}

static bool Translate(map_t *Map, object_t Object, point_t At)
{
	bool Result = false;

	map_object_t *Reference = GetMapObject(Map, Object);
	if (Reference)
		Result = Translate(Map, Reference, At);
	return Result;
}

static bool IsPassable(const map_t *Map, point_t At)
{
	bool Result = 0;

	const map_tile_t *Tile = GetTile(Map, At);
	Result = Tile && (Tile->Object == 0);

	return Result;
}
//


//