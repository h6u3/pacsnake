#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"

class AnimatedSprite;
class Vector2;

class Enemy : public Entity {
public:
    Enemy(int x, int y);
    ~Enemy();
    bool Initialise(Renderer& renderer, const char* spritePath);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);
    bool IsAlive() const;
    void SetDead();
    void SetAlive();
    Vector2& GetPosition();
    void SetPosition(int x, int y);
    AnimatedSprite* GetSprite();

    void Respawn();

public:
    AnimatedSprite* m_pSprite; 
    float m_elapsedTime;  // Time since the last move
    int m_directionX;  // Current direction on X axis
    int m_directionY;  // Current direction on Y axis
    bool m_bAlive;
    Vector2 m_position;
    Vector2 m_InitPos;
    Vector2 m_velocity;
};

#endif  // __FOOD_H__
