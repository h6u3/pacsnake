#include "level.h"
#include "renderer.h"
#include "iniparser.h"
#include "player.h"
#include "enemy.h"
#include "powerup.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "inlinehelpers.h"
#include "logmanager.h"
#include "soundsystem.h"
#include <iostream>

Level::Level(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_iSpace(0)
    , score(0)
    , enemyCount(0)
    , pUpCount(0)
    , enemyResTimer(0)
    , pUpResTimer(0)
    , thousandsScoreDigit(0)
    , hundredsScoreDigit(0)
    , tensScoreDigit(0)
    , unitsScoreDigit(0)
    , pUpRespawn(false)
    , pUpRespawnTimer(false)
    , enemyRespawn(false)
    , enemyRespawnTimer(false)
    , m_pPlayer(0)
    , thousandsSpaceDigit(0)
    , hundredsSpaceDigit(0)
    , tensSpaceDigit(0)
    , unitsSpaceDigit(0)
    , m_pRenderer(0)
    , m_pScoreText(0)
    , m_pSpaceText(0)
{
    // Reserve space in the vector to avoid reallocations
    m_tiles.reserve(width * height);
}

Level::~Level()
{
    // Delete all dynamically allocated tiles
    for (auto it = begin(m_tiles); it != end(m_tiles); ++it) {
        delete* it;
        *it = nullptr;
    }
    m_tiles.clear();
    for (auto it = begin(m_powerUps); it != end(m_powerUps); ++it) {
        delete* it;
        *it = nullptr;
    }
    m_powerUps.clear();
    for (auto it = begin(m_enemies); it != end(m_enemies); ++it) {
        delete* it;
        *it = nullptr;
    }
    m_enemies.clear();

    delete m_pPlayer;
    m_pPlayer = nullptr;
    delete thousandsScoreDigit;
    thousandsScoreDigit = nullptr;
    delete hundredsScoreDigit;
    hundredsScoreDigit = nullptr;
    delete tensScoreDigit;
    tensScoreDigit = nullptr;
    delete unitsScoreDigit;
    unitsScoreDigit = nullptr;

    delete thousandsSpaceDigit;
    delete hundredsSpaceDigit;
    delete tensSpaceDigit;
    delete unitsSpaceDigit;
    delete m_pScoreText;
    delete m_pSpaceText;
    thousandsSpaceDigit = nullptr;
    hundredsSpaceDigit = nullptr;
    tensSpaceDigit = nullptr;
    unitsSpaceDigit = nullptr;
    m_pScoreText = nullptr;
    m_pSpaceText = nullptr;

    m_pRenderer = nullptr;
}

bool Level::Initialise(Renderer& renderer)
{
    m_pRenderer = &renderer;
    // Load player, other entities, etc.
    // Initialize the player (snake)

    int winHeight = IniParser::GetInstance().GetValueAsInt("Window", "Height");
    float gridHeight = (float)winHeight / m_height;

    m_pPlayer = new Player((int)(gridHeight * 1.5), (int)(gridHeight * 1.5), 1, renderer);  // Start the snake at (1,1) with length 1

    m_iSpace--;

    const int SCREEN_WIDTH = renderer.GetWidth();
    const int SCREEN_HEIGHT = renderer.GetHeight();

    // Load static text textures into the Texture Manager...
    std::string sText = "Score: ";
    const char* scoretext = sText.c_str();
    renderer.CreateStaticText(scoretext, 60);

    // Generate sprites that use the static text textures...
    m_pScoreText = renderer.CreateSprite(scoretext);
    m_pScoreText->SetX((int)(SCREEN_WIDTH / 1.25f));
    m_pScoreText->SetY((int)(SCREEN_HEIGHT / 2));

    //scores 
    thousandsScoreDigit = renderer.CreateAnimatedSprite("sprites\\number.png");
    hundredsScoreDigit = renderer.CreateAnimatedSprite("sprites\\number.png");
    tensScoreDigit = renderer.CreateAnimatedSprite("sprites\\number.png");
    unitsScoreDigit = renderer.CreateAnimatedSprite("sprites\\number.png");

    thousandsScoreDigit->SetupFrames(64,64);
    hundredsScoreDigit->SetupFrames(64, 64);
    tensScoreDigit->SetupFrames(64, 64);
    unitsScoreDigit->SetupFrames(64, 64);

    thousandsScoreDigit->SetX((int)((SCREEN_WIDTH / 1.25f)+(m_pScoreText->GetWidth() / 2)));
    thousandsScoreDigit->SetY((int)((SCREEN_HEIGHT / 2) + 5));

    hundredsScoreDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pScoreText->GetWidth() / 2)+32));
    hundredsScoreDigit->SetY((int)((SCREEN_HEIGHT / 2) + 5));

    tensScoreDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pScoreText->GetWidth() / 2) + 64));
    tensScoreDigit->SetY((int)((SCREEN_HEIGHT / 2) + 5));

    unitsScoreDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pScoreText->GetWidth() / 2) + 96));
    unitsScoreDigit->SetY((int)((SCREEN_HEIGHT / 2) + 5));


    // Load static text textures into the Texture Manager...
    std::string spText = "Space Left: ";
    const char* spacetext = spText.c_str();
    renderer.CreateStaticText(spacetext, 60);

    // Generate sprites that use the static text textures...
    m_pSpaceText = renderer.CreateSprite(spacetext);
    m_pSpaceText->SetX((int)(SCREEN_WIDTH / 1.25f));
    m_pSpaceText->SetY((int)(SCREEN_HEIGHT / 3));

    //space
    thousandsSpaceDigit = renderer.CreateAnimatedSprite("sprites\\number.png");
    hundredsSpaceDigit = renderer.CreateAnimatedSprite("sprites\\number.png");
    tensSpaceDigit = renderer.CreateAnimatedSprite("sprites\\number.png");
    unitsSpaceDigit = renderer.CreateAnimatedSprite("sprites\\number.png");

    thousandsSpaceDigit->SetupFrames(64, 64);
    hundredsSpaceDigit->SetupFrames(64, 64);
    tensSpaceDigit->SetupFrames(64, 64);
    unitsSpaceDigit->SetupFrames(64, 64);

    thousandsSpaceDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pSpaceText->GetWidth() / 2)));
    thousandsSpaceDigit->SetY((int)((SCREEN_HEIGHT / 3) + 5));

    hundredsSpaceDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pSpaceText->GetWidth() / 2) + 32));
    hundredsSpaceDigit->SetY((int)((SCREEN_HEIGHT / 3) + 5));

    tensSpaceDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pSpaceText->GetWidth() / 2) + 64));
    tensSpaceDigit->SetY((int)((SCREEN_HEIGHT / 3) + 5));

    unitsSpaceDigit->SetX((int)((SCREEN_WIDTH / 1.25f) + (m_pSpaceText->GetWidth() / 2) + 96));
    unitsSpaceDigit->SetY((int)((SCREEN_HEIGHT / 3) + 5));


    updateSpace(renderer);

    SoundSystem::GetInstance().PlaySound("sounds\\BM_GameLoopMusic4.mp3");

    return true;
}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
    if (m_pPlayer->GetMoving()) {
        int winHeight = IniParser::GetInstance().GetValueAsInt("Window", "Height");
        float gridHeight = (float)winHeight / m_height;

        // Process each tile in the level (if needed)
        for (Tile* tile : m_tiles) {
            tile->Process(deltaTime);
            if (m_pPlayer->CollideTile(tile)) {
                m_pPlayer->SetMoving(false);
            }
        }

        for (Enemy* enemy : m_enemies) {
            enemy->Process(deltaTime);
            if (m_pPlayer->CollideEnemy(enemy) && enemy->IsAlive() == true) {
                //LogManager::GetInstance().Log("collided");
                std::cout << m_pPlayer->IsPowerful();
                if (m_pPlayer->IsPowerful()) {
                    //SoundSystem::GetInstance().PlaySound("sounds\\SE-EnemyExplosion.wav");
                    enemy->SetDead();
                    enemyCount--;
                    m_pPlayer->Grow();
                    m_iSpace--;
                    updateSpace(*m_pRenderer);
                    score += 100;
                    updateScore(*m_pRenderer);
                }
                else {
                    //kill player
                    //do gameover stuff
                    m_pPlayer->SetMoving(false);
                }
            }
        }

        for (PowerUp* pUp : m_powerUps) {
            pUp->Process(deltaTime);
            if (m_pPlayer->CollidePowerUp(pUp)) {
                SoundSystem::GetInstance().PlaySound("sounds\\SE-ShipSpawn.wav");
                pUp->SetDead();
                pUpCount--;
                m_pPlayer->makePowerful();
                score += 50;
                updateScore(*m_pRenderer);
            }
        }

        if (pUpCount<=0) {
            pUpRespawnTimer = true;
        }
        if (pUpRespawnTimer) {
            pUpResTimer += deltaTime;
            const float RespawnTime = 5.0f;

            if (pUpResTimer >= RespawnTime) {
                pUpRespawn = true;
                pUpRespawnTimer = false;
                pUpResTimer = 0.0f;
            }
        }
        if (pUpRespawn) {
            for (PowerUp* pUp : m_powerUps) {
                pUp->SetAlive();
                pUpCount=m_powerUps.size();
            }
            pUpRespawn = false;
        }

        if (enemyCount<=0) {
            enemyRespawnTimer = true;
        }
        if (enemyRespawnTimer) {
            enemyResTimer += deltaTime;
            const float RespawnTime = 5.0f;
            if (enemyResTimer >= RespawnTime) {
                enemyRespawn = true;
                enemyRespawnTimer = false;
                enemyResTimer = 0.0f;
            }
        }
        if (enemyRespawn) {
            for (Enemy* enemy : m_enemies) {
                enemy->Respawn();
                enemyCount = m_enemies.size();
            }
            enemyRespawn = false;
        }

        m_pPlayer->Process(deltaTime, inputSystem);

        if (m_pPlayer->IsCollidingWithSelf()) {
            //LogManager::GetInstance().Log("collided");
            //gameover stuff
            if (m_pPlayer->GetSize() > 2) {
                m_pPlayer->SetMoving(false);
            }
        }
    }
    else {
        //player dead
        //do game over stuff
        //m_pPlayer->SetMoving(false);
    }
    m_pScoreText->Process(deltaTime);
    thousandsScoreDigit->Process(deltaTime);
    hundredsScoreDigit->Process(deltaTime);
    tensScoreDigit->Process(deltaTime);
    unitsScoreDigit->Process(deltaTime);

    m_pSpaceText->Process(deltaTime);
    thousandsSpaceDigit->Process(deltaTime);
    hundredsSpaceDigit->Process(deltaTime);
    tensSpaceDigit->Process(deltaTime);
    unitsSpaceDigit->Process(deltaTime);
}

void Level::Draw(Renderer& renderer)
{
    // Draw each tile in the level
    for (Tile* tile : m_tiles) {
        tile->Draw(renderer);
    }
    for (Enemy* enemy : m_enemies) {
        enemy->Draw(renderer);
    }
    for (PowerUp* pUp : m_powerUps) {
        pUp->Draw(renderer);
    }

    m_pPlayer->Draw(renderer);
    m_pScoreText->Draw(renderer);
    thousandsScoreDigit->Draw(renderer);
    hundredsScoreDigit->Draw(renderer);
    tensScoreDigit->Draw(renderer);
    unitsScoreDigit->Draw(renderer);
    m_pSpaceText->Draw(renderer);
    thousandsSpaceDigit->Draw(renderer);
    hundredsSpaceDigit->Draw(renderer);
    tensSpaceDigit->Draw(renderer);
    unitsSpaceDigit->Draw(renderer);
}

void Level::AddTile(Tile* tile)
{
    // Add the tile to the collection
    if (tile != nullptr) {
        m_tiles.push_back(tile);
    }
}

void Level::AddEnemy(Enemy* enemy)
{
    // Add the tile to the collection
    if (enemy != nullptr) {
        m_enemies.push_back(enemy);
        enemyCount++;
    }
}

void Level::AddPowerUp(PowerUp* pUp)
{
    // Add the tile to the collection
    if (pUp != nullptr) {
        m_powerUps.push_back(pUp);
        pUpCount++;
    }
}

Tile* Level::GetTile(int x, int y) const
{
    // Calculate the 1D index from 2D coordinates and retrieve the tile
    int index = y * m_width + x;

    if (index >= 0 && index < static_cast<int>(m_tiles.size())) {
        return m_tiles[index];
    }

    // If the index is out of bounds, return nullptr
    return nullptr;
}

void Level::updateScore(Renderer& renderer) {
    digits.clear();  // Clear the previous digits
    collect_digits(score);  // Extract the digits from score

    // Ensure that digits vector has enough elements to access safely
    if (digits.size() >= 4) {
        thousandsScoreDigit->SetFrame(digits[3]);  // Thousands place
        hundredsScoreDigit->SetFrame(digits[2]);   // Hundreds place
        tensScoreDigit->SetFrame(digits[1]);       // Tens place
        unitsScoreDigit->SetFrame(digits[0]);      // Units place
    }
    else if (digits.size() >= 3) {
        // No thousands place, handle only hundreds, tens, and units
        hundredsScoreDigit->SetFrame(digits[2]);
        tensScoreDigit->SetFrame(digits[1]);
        unitsScoreDigit->SetFrame(digits[0]);
    }
    else if (digits.size() >= 2) {
        // No hundreds or thousands
        tensScoreDigit->SetFrame(digits[1]);
        unitsScoreDigit->SetFrame(digits[0]);
    }
    else if (digits.size() >= 1) {
        // Only units place
        unitsScoreDigit->SetFrame(digits[0]);
    }
}

void Level::updateSpace(Renderer& renderer) {
    digits.clear();  // Clear the previous digits
    collect_digits(m_iSpace);  // Extract the digits from score

    // Ensure that digits vector has enough elements to access safely
    if (digits.size() >= 4) {
        thousandsSpaceDigit->SetFrame(digits[3]);  // Thousands place
        hundredsSpaceDigit->SetFrame(digits[2]);   // Hundreds place
        tensSpaceDigit->SetFrame(digits[1]);       // Tens place
        unitsSpaceDigit->SetFrame(digits[0]);      // Units place
    }
    else if (digits.size() >= 3) {
        // No thousands place, handle only hundreds, tens, and units
        hundredsSpaceDigit->SetFrame(digits[2]);
        tensSpaceDigit->SetFrame(digits[1]);
        unitsSpaceDigit->SetFrame(digits[0]);
    }
    else if (digits.size() >= 2) {
        // No hundreds or thousands
        tensSpaceDigit->SetFrame(digits[1]);
        unitsSpaceDigit->SetFrame(digits[0]);
    }
    else if (digits.size() >= 1) {
        // Only units place
        unitsSpaceDigit->SetFrame(digits[0]);
    }
}

void Level::collect_digits(unsigned long num) {
    if (num == 0 && digits.empty()) {
        digits.push_back(0); // Handle the case where score is 0
    }

    // Recursively collect digits
    while (num > 0) {
        digits.push_back(num % 10); // Extract least significant digit
        num /= 10; // Move to the next digit
    }
}

int Level::GetSpace() const {
    return m_iSpace;
}

void Level::AddSpace() {
    m_iSpace++;
}

bool Level::IsPlaying() {
    return m_pPlayer->IsAlive();
}

Player* Level::GetPlayer() {
    return m_pPlayer;
}