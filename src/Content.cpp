enum object_type_t
{
	ObjectType_Player = 1,
	ObjectType_Bonfire,
	ObjectType_Slime,
	ObjectType_Count,
};

enum object_tag_t
{
	Tag_Hostile = 1,
	Tag_Bonfire,
};

struct content_t
{
	font_t Font;
	bitmap_t TileSetBitmap;

	object_behaviour_t ObjectTypes[ObjectType_Count];

	uint8_t Reserved[MB(4)];
	const assets_t *Assets;
};

static void LoadGameContent(assets_t *Assets, content_t *Content)
{	
	//
	Content->Font = LoadFontFromDirectory(Assets, "PICO-8.fnt");

	memory_t Memory = {};
	SetupMemory(&Memory, Content->Reserved, Len(Content->Reserved));

	// Tiles

	Content->TileSetBitmap = LoadBitmapFromDirectory(Assets, "Tileset_000.tga");
	
	// Objects

	object_behaviour_t *Bonfire = &Content->ObjectTypes[ObjectType_Bonfire];
	Bonfire->Tag = Tag_Bonfire;
	Bonfire->Animation[0] = LoadBitmapFromDirectory(Assets, "Bonfire_001.tga");
	Bonfire->Animation[1] = LoadBitmapFromDirectory(Assets, "Bonfire_001.tga");

	object_behaviour_t *Player = &Content->ObjectTypes[ObjectType_Player];
	Player->Animation[0] = LoadBitmapFromDirectory(Assets, "Char_001.tga");
	Player->Animation[1] = LoadBitmapFromDirectory(Assets, "Char_002.tga");
	Player->Alignment = Alignment_Bot;

	object_behaviour_t *Slime = &Content->ObjectTypes[ObjectType_Slime];
	Slime->Animation[0] = LoadBitmapFromDirectory(Assets, "Slime_001.tga");
	Slime->Animation[1] = LoadBitmapFromDirectory(Assets, "Slime_002.tga");;
	Slime->Tag = Tag_Hostile;
	//Slime->Alignment = Alignment_Bot;
	
	Content->Assets = Assets;
}