#include "tile.h"
#include "sprite.h"
#include "renderer.h"

Tile::Tile()
    : m_pSprite(nullptr)
{
}

Tile::~Tile()
{
    // Destructor: Clean up the sprite if it exists
    if (m_pSprite) {
        delete m_pSprite;
        m_pSprite = nullptr;
    }
}

bool Tile::Initialise(Renderer& renderer)
{
    // Create a sprite for the tile and initialize it
    m_pSprite = renderer.CreateSprite("sprites\\board8x8.png"); // Load tile sprite here

    if (!m_pSprite) {
        return false;  // Return false if sprite loading failed
    }

    // Set the initial position for the tile (you can adjust the position later)
    m_position.Set(0.0f, 0.0f);

    return true;
}

void Tile::Process(float deltaTime)
{
    // If any updates need to happen to the tile (physics, animation, etc.)
    // For now, tiles are static, so no update logic is necessary
}

void Tile::Draw(Renderer& renderer)
{
    // Set the sprite's position to match the tile's position
    if (m_pSprite) {
        m_pSprite->SetX(static_cast<int>(m_position.x));
        m_pSprite->SetY(static_cast<int>(m_position.y));

        // Draw the sprite using the renderer
        m_pSprite->Draw(renderer);
    }
}

void Tile::SetPos(int x, int y) {
    m_position.Set((float)x, (float)y);
}

void Tile::SetScale(float scale) {
    m_pSprite->SetScale(scale);
}

Vector2* Tile::GetPos() {
    return &m_position;
}