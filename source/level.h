#ifndef __LEVEL_H__
#define __LEVEL_H__

#include <vector>
#include "tile.h"

// Forward declarations:
class Renderer;
class Player;
class Enemy;
class InputSystem;
class PowerUp;
class Sprite;
class AnimatedSprite;
class SoundSystem;

class Level {
public:
    Level(int width, int height);
    ~Level();

    bool Initialise(Renderer& renderer);   // Initialize the level (loading resources, etc.)
    void Process(float deltaTime, InputSystem& inputSystem);         // Process level updates
    void Draw(Renderer& renderer);         // Render the entire level

    void AddTile(Tile* tile);              // Add a tile to the level
    void AddEnemy(Enemy* enemy);
    void AddPowerUp(PowerUp* pUp);
    Tile* GetTile(int x, int y) const;     // Retrieve a tile at a given position

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    void updateScore(Renderer& renderer);
    void updateSpace(Renderer& renderer);
    void collect_digits(unsigned long num);

    int GetSpace() const;
    void AddSpace();

    bool IsPlaying();

    Player* GetPlayer();

private:
    int m_width;                           // Level dimensions
    int m_height;

    int score;

    int enemyCount;
    float enemyResTimer;

    int pUpCount;
    float pUpResTimer;

    int m_iSpace;

    std::vector<Tile*> m_tiles;            // Collection of tiles that make up the level
    Player* m_pPlayer;

    std::vector<Enemy*> m_enemies;
    std::vector<PowerUp*> m_powerUps;
    //std::vector<Coin*> m_coins;

    bool pUpRespawnTimer;
    bool enemyRespawnTimer;
    bool pUpRespawn;
    bool enemyRespawn;

    Renderer* m_pRenderer;

    Sprite* m_pScoreText;
    AnimatedSprite* thousandsScoreDigit;
    AnimatedSprite* hundredsScoreDigit;
    AnimatedSprite* tensScoreDigit;
    AnimatedSprite* unitsScoreDigit;

    Sprite* m_pSpaceText;
    AnimatedSprite* thousandsSpaceDigit;
    AnimatedSprite* hundredsSpaceDigit;
    AnimatedSprite* tensSpaceDigit;
    AnimatedSprite* unitsSpaceDigit;

    std::vector<int> digits;
};

#endif // __LEVEL_H__
