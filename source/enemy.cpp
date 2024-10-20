#include "enemy.h"
#include "Sprite.h"
#include "animatedsprite.h"
#include "Renderer.h"
#include "inlinehelpers.h"
#include "iniparser.h"


Enemy::Enemy(int x, int y)
    : m_pSprite(nullptr)
    , m_bAlive(true)
    , m_velocity(0, 0)
    , m_directionX(0)
    , m_directionY(0)
    , m_elapsedTime(0.0f)
{
    m_position.x = (float)x;
    m_position.y = (float)y;
    m_InitPos.x = (float)x;
    m_InitPos.y = (float)y;
}

Enemy::~Enemy()
{
    delete m_pSprite;
    m_pSprite = nullptr;
}

bool Enemy::Initialise(Renderer& renderer, const char* spritePath)
{
    m_pSprite = renderer.CreateAnimatedSprite(spritePath);
    if (m_pSprite == nullptr)
    {
        return false; // Fail early if sprite creation failed
    }

    m_pSprite->SetFrameDuration(0.3f);
    m_pSprite->SetLooping(true);
    m_pSprite->SetupFrames(307, 307);
    m_pSprite->Animate();

    int winHeight = IniParser::GetInstance().GetValueAsInt("Window", "Height");
    float tileHeight = (float)winHeight / 24;
    float tileScale = tileHeight / 307; // 307 is the sprite height
    m_pSprite->SetScale(tileScale);

    return true;
}


void Enemy::Process(float deltaTime)
{
    if (m_bAlive) {
        //m_position += m_velocity;
        float angle = m_pSprite->GetAngle();
        angle += 0.05f;
        m_pSprite->Process(deltaTime);


        // Accumulate deltaTime
        m_elapsedTime += deltaTime;

        // Movement interval (in seconds) - Adjust this value to control speed (e.g., 0.1 for 10 moves per second)
        const float movementInterval = 0.75f;

        // Only update direction and position when enough time has passed
        if (m_elapsedTime >= movementInterval) {
            // Reset the elapsed time
            m_elapsedTime = 0.0f;
            int move = GetRandom(0,3);
            // Update direction based on rng
            if (move == 0) {
                m_directionX = 0;
                m_directionY = -30;
            }
            else if (move == 1) {
                m_directionX = 0;
                m_directionY = 30;
            }
            else if (move == 2) {
                m_directionX = -30;
                m_directionY = 0;
            }
            else if (move == 3) {
                m_directionX = 30;
                m_directionY = 0;
            }

            // Move
            float newX = m_position.x + m_directionX;
            float newY = m_position.y + m_directionY;

            // move
            m_position.Set(newX, newY);
        }
    }
}

void Enemy::Draw(Renderer& renderer)
{
    if (m_bAlive)
    {
        m_pSprite->SetPosition(static_cast<int>(m_position.x), static_cast<int>(m_position.y));  // Set grid-based position
        m_pSprite->Draw(renderer);
    }
}

bool Enemy::IsAlive() const
{
    return m_bAlive;
}

void Enemy::SetDead()
{
    m_bAlive = false;
}

void Enemy::SetAlive()
{
    m_bAlive = true;
}

void Enemy::Respawn() {
    m_bAlive = true;
    m_position.x = m_InitPos.x;
    m_position.y = m_InitPos.y;
}

Vector2& Enemy::GetPosition()
{
    return m_position;
}

void Enemy::SetPosition(int x, int y)
{
    m_position.x = (float)x;
    m_position.y = (float)y;
}
/*
bool Enemy::IsCollidingWith(Entity& toCheck)
{
    // For grid-based movement, collision occurs when both entities are on the same tile
    return (m_position.x == toCheck.m_position.x && m_position.y == toCheck.m_position.y);
}*/


AnimatedSprite* Enemy::GetSprite() {
    return m_pSprite;
}