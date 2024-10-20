#include "tilefactory.h"
#include "tile.h"
#include "renderer.h"

Tile* TileFactory::CreateTile(int x, int y, float scale, Renderer& renderer)
{
    // Create a new Tile object
    Tile* pTile = new Tile();
    pTile->Initialise(renderer);
        // Set the position of the tile
    pTile->SetPos(x, y);
    pTile->SetScale(scale);
    return pTile;
}
