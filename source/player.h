#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include <deque>

class Renderer;
class InputSystem;
class Sprite;
class Enemy;
class Tile;
class PowerUp;

class Player : public Entity {
public:
    Player(int startX, int startY, int initialLength, Renderer& renderer);
    ~Player();

    void Process(float deltaTime, InputSystem& inputSystem);  // Handle movement and direction
    void Draw(Renderer& renderer);  // Render the snake

    void Grow();  // Add a new segment to the snake

    bool IsCollidingWithSelf() const;  // Check if the snake has hit itself
    bool CollideEnemy(Enemy* enemy);
    bool CollideTile(Tile* tile);
    bool CollidePowerUp(PowerUp* pUp);

    bool GetMoving();
    void SetMoving(bool moving);

    bool IsPowerful();
    void setPowerful(bool powerUp);
    void makePowerful();
    void makeWeak();

    int GetSize();
    bool IsAlive();

    void RenderInput(InputSystem& inputSystem);

    Vector2 GetPosition();
private:
    std::deque<Entity*> m_segments;  // Deque to manage the snake's segments (head to tail)
    float m_elapsedTime;  // Time since the last move
    int m_directionX;  // Current direction on X axis
    int m_directionY;  // Current direction on Y axis
    Renderer* m_pRenderer;
    bool m_pMoving;
    bool m_pPowerUp;
    float m_powerUpTime;

    Sprite* m_pW;
    Sprite* m_pA;
    Sprite* m_pS;
    Sprite* m_pD;
};

#endif  // __PLAYER_H__
