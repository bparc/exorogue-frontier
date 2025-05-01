// ...

typedef uint32_t object_t;

// ...

enum tile_attribute_t
{
	TileAttrib_Traversable = (1 << 1),
};

struct map_tile_t
{
	uint16_t Attributes;
	object_t Object; // Occupier

	//
	
	uint8_t ID;
	uint8_t Decals[1];
};

bool IsEmpty(const map_tile_t *Tile);
bool IsTraversable(const map_tile_t *Tile);

struct object_behaviour_t
{
	bitmap_t Animation[2]; 
	uint8_t Tag;
};

struct map_object_t
{
	const object_behaviour_t *Type;

	object_t Self;
	point_t OccupiedTile; // NOTE: Has to be set via Translate()!!!

	// Rendering
	float_t DamageFlickerTime; // NOTE: Object will flicker if > 0.0.
	float_t Height;
	vec2_t BitmapOffset;

	// ...

	// ...
};

static vec2_t GetObjectCenter(const map_object_t *Obj);

struct map_t
{
	vec2_t TileSize;
	int32_t X, Y;
	map_tile_t *Tiles;

	uint16_t ObjectLookasideTable[UINT16_MAX]; // Maps ID (object_t) -> Objects[Index]
	array_t <map_object_t, 1024> Objects;
	uint64_t TotalObjectCount; // The number of objects that ever existed

	object_behaviour_t None;
};

static void CreateMap(map_t *Map, int32_t X, int32_t Y, map_tile_t *Tiles, int32_t TileSize);

static bool CheckBounds(const map_t *Map, point_t Offset);
static map_tile_t *GetTile(map_t *Map, point_t Offset);
static uint32_t GetTileValue(const map_t *Map, point_t Offset);
static void SetTileValue(map_t *Map, point_t Offset, int32_t Value);

static rect_t GetTileBounds(const map_t *Map, point_t Offset);
static vec2_t GetTileCenter(const map_t *Map, point_t Offset);

static vec2_t CellToCoords(const map_t *Map, point_t Offset);
static point_t CoordsToCell(const map_t *Map, vec2_t Coords);

static map_object_t *CreateMapObject(map_t *Map, point_t At, const object_behaviour_t *Behaviour = NULL);
static map_object_t *GetMapObject(map_t *Map, object_t Object);
static map_object_t *GetMapObject(map_t *Map, point_t Offset);
//static map_object_t *GetMapObject(map_t *Map, point_t Offset);

static bool Translate(map_t *Map, map_object_t *Object, point_t At);
static bool Translate(map_t *Map, object_t Object, point_t At);
static bool IsPassable(const map_t *Map, point_t At);

//

// ...

// game_state_t

