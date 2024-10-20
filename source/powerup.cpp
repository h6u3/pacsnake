#include "powerup.h"
#include "Sprite.h"
#include "animatedsprite.h"
#include "Renderer.h"
#include <cmath>  // for std::abs
#include "iniparser.h"


PowerUp::PowerUp(int x, int y)
    : m_pSprite(nullptr)
    , m_bAlive(true)
    , m_velocity(0, 0)
{
    m_position.x = (float)x;
    m_position.y = (float)y;
}

PowerUp::~PowerUp()
{
    delete m_pSprite;
    m_pSprite = nullptr;
}

bool PowerUp::Initialise(Renderer& renderer, const char* spritePath)
{
    m_pSprite = renderer.CreateAnimatedSprite(spritePath);  // Allow dynamic loading of sprites for different entities
    m_pSprite->SetFrameDuration(1);
    m_pSprite->SetLooping(true);
    m_pSprite->SetupFrames(66, 66);
    m_pSprite->Animate();
    int winHeight = IniParser::GetInstance().GetValueAsInt("Window", "Height");
    float tileHeight = (float)winHeight / 24;
    float tileScale = (float)tileHeight / 66;
    m_pSprite->SetScale(tileScale);
    return (m_pSprite != nullptr);
}

void PowerUp::Process(float deltaTime)
{
    if (m_bAlive) {
        //m_position += m_velocity;
        float angle = m_pSprite->GetAngle();
        angle += 0.1f;
        m_pSprite->SetAngle(angle);
        m_pSprite->Process(deltaTime);
    }
}

void PowerUp::Draw(Renderer& renderer)
{
    if (m_bAlive)
    {
        m_pSprite->SetPosition(static_cast<int>(m_position.x), static_cast<int>(m_position.y));  // Set grid-based position
        m_pSprite->Draw(renderer);
    }
}

bool PowerUp::IsAlive() const
{
    return m_bAlive;
}

void PowerUp::SetDead()
{
    m_bAlive = false;
}

void PowerUp::SetAlive()
{
    m_bAlive = true;
}

Vector2& PowerUp::GetPosition()
{
    return m_position;
}

void PowerUp::SetPosition(int x, int y)
{
    m_position.x = (float)x;
    m_position.y = (float)y;
}

AnimatedSprite* PowerUp::GetSprite() {
    return m_pSprite;
}