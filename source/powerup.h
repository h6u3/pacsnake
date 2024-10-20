#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "entity.h"

class AnimatedSprite;
class Vector2;

class PowerUp : public Entity {
public:
    PowerUp(int x, int y);
    ~PowerUp();
    bool Initialise(Renderer& renderer, const char* spritePath);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);
    bool IsAlive() const;
    void SetDead();
    void SetAlive();
    Vector2& GetPosition();
    void SetPosition(int x, int y);
    AnimatedSprite* GetSprite();

public:
    AnimatedSprite* m_pSprite;
    bool m_bAlive;
    Vector2 m_position;
    Vector2 m_velocity;
};

#endif  // __FOOD_H__
