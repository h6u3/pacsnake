#ifndef __TILEFACTORY_H__
#define __TILEFACTORY_H__

#include "tile.h"
#include "renderer.h"

// TileFactory responsible for creating Tile objects based on input characters
class TileFactory {
public:
    static Tile* CreateTile(int x, int y, float scale, Renderer& renderer);
};

#endif // __TILEFACTORY_H__
