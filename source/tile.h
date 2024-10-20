#ifndef __TILE_H__
#define __TILE_H__

#include "entity.h"

// Forward declarations:
class Sprite;
class Renderer;

class Tile : public Entity {
public:
    Tile();
    ~Tile();
    bool Initialise(Renderer& renderer);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);
    void SetPos(int x, int y);
    void SetScale(float scale);
    Vector2* GetPos();
protected:
private:
    Tile(const Tile& tile);
    Tile& operator=(const Tile& tile);
    // Member data: 
public:
protected:
    Sprite* m_pSprite;
    Vector2 m_position;
private:
};

#endif // __TILE_H__
