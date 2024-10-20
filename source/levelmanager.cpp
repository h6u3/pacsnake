#include "levelmanager.h"
#include "renderer.h"
#include "tilefactory.h"
#include "enemy.h"
#include "powerup.h"
#include <fstream>
#include <iostream>
#include "iniparser.h"
#include "level.h"
#include "logmanager.h"

LevelManager* LevelManager::sm_pInstance = 0;
LevelManager&
LevelManager::GetInstance()
{
    if (sm_pInstance ==
        0)
    {
        sm_pInstance =
            new LevelManager();
    }return (*sm_pInstance);
}
void
LevelManager::DestroyInstance()
{
    delete sm_pInstance;
    sm_pInstance = 0;
}

LevelManager::LevelManager() {
    // Constructor
}

LevelManager::~LevelManager() {
    UnloadAllLevels();
}

bool LevelManager::Initialise() {
    // Initialization logic, if needed
    return true;
}

Level* LevelManager::GetLevel(const std::string& levelKey, Renderer& renderer) {
    // Check if the level is already loaded in memory
    auto iter = m_loadedLevels.find(levelKey);
    if (iter != m_loadedLevels.end()) {
        // Return the already loaded level
        return iter->second;
    }

    // Level not loaded, so load it from the file
    std::ifstream file(levelKey);

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << levelKey << std::endl;
        return nullptr;
    }

    int width = 0;
    int height = 0;
    std::vector<std::string> lines;
    std::string line;

    // Read the level file line by line
    while (std::getline(file, line)) {
        lines.push_back(line);
        width = std::max(width, static_cast<int>(line.length()));
        height++;
    }

    file.close();

    // Check if there were any lines in the file
    if (lines.empty()) {
        std::cerr << "Level file is empty: " << levelKey << std::endl;
        return nullptr;
    }

    // Create the level
    Level* pLevel = new Level(width, height);
    int winHeight = IniParser::GetInstance().GetValueAsInt("Window", "Height");
    float tileHeight = (float)winHeight / height;
    float tileScale = tileHeight / 442; //442 is the sprite height

    // Create tiles from the file content
    for (int y = 0; y < height; ++y) {
        const std::string& row = lines[y];
        for (int x = 0; x < static_cast<int>(row.length()); ++x) {
            char tileChar = row[x];

            // For now, create tiles for all non-space characters
            if (tileChar == 'W') {
                Tile* pTile = TileFactory::CreateTile((int)(x*tileHeight+(tileHeight/2)), (int)(y*tileHeight+(tileHeight/2)), tileScale, renderer);
                if (pTile != nullptr) {
                    pLevel->AddTile(pTile);
                }
            }
            else if (tileChar == 'E') {
                Enemy* pEnemy = new Enemy((int)(x * tileHeight + (tileHeight / 2)), (int)(y * tileHeight + (tileHeight / 2)));
                pEnemy->Initialise(renderer, "sprites\\enemy.png");
                if (pEnemy != nullptr) {
                    pLevel->AddEnemy(pEnemy);
                    pLevel->AddSpace();
                }
            }
            else if (tileChar == 'P') {
                PowerUp* pPUp = new PowerUp((int)(x * tileHeight + (tileHeight / 2)), (int)(y * tileHeight + (tileHeight / 2)));
                pPUp->Initialise(renderer, "sprites\\powerup.png");
                if (pPUp != nullptr) {
                    pLevel->AddPowerUp(pPUp);
                    pLevel->AddSpace();
                }
            }
            else {
                pLevel->AddSpace();
            }
        }
    }


    std::string space = "Space: " + std::to_string(pLevel->GetSpace());
    LogManager::GetInstance().Log(space.c_str());

    // Store the loaded level in memory for future access
    m_loadedLevels[levelKey] = pLevel;


    return pLevel;
}

void LevelManager::UnloadAllLevels() {
    std::map<std::string, Level*>::iterator iter = m_loadedLevels.begin();

    // Loop through the map and delete the level objects
    while (iter != m_loadedLevels.end()) {
        Level* pLevel = iter->second;

        if (pLevel != nullptr) { // Check if the pointer is not null
            delete pLevel;
            iter->second = nullptr; // Set pointer to nullptr after deletion
        }

        ++iter;
    }

    // Clear the map after deleting the objects
    m_loadedLevels.clear();
}
