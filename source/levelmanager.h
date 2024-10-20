#ifndef __LEVELMANAGER_H__
#define __LEVELMANAGER_H__

#include <string>
#include <map>
#include "level.h"

// Forward declarations:
class Renderer;
class Enemy;
class PowerUp;

class LevelManager {
public:
    static LevelManager& GetInstance();
    static void DestroyInstance();

    LevelManager();
    ~LevelManager();

    bool Initialise();  // Initialize resources if needed

    // Combined method to load a level or return it if already loaded
    Level* GetLevel(const std::string& levelKey, Renderer& renderer);
    void UnloadAllLevels();

private:
    std::map<std::string, Level*> m_loadedLevels; // Map storing all loaded levels
    static LevelManager* sm_pInstance;
};

#endif // __LEVELMANAGER_H__
