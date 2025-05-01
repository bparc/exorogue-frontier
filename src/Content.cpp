
struct content_t
{
	font_t Font;
	bitmap_t TileSetBitmap;

	uint8_t Reserved[MB(4)];
	assets_t *Assets;
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

	Content->Assets = Assets;
}