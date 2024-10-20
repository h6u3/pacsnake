// COMP710 GP Framework 2022

#ifndef __SCENEGAMELEVEL_H   
#define __SCENEGAMELEVEL_H 

// Local includes: 
#include "scene.h"

// Forward declarations: 
class Renderer;
class SoundSystem;
class Sprite;
class Level; // Forward declaration for Level class
class LevelManager;
class ParticleEmitter;

// Class declaration:
class SceneGameLevel : public Scene
{
    // Member methods:
public:
    SceneGameLevel();
    virtual ~SceneGameLevel();
    virtual bool Initialise(Renderer& renderer, SoundSystem* soundSystem) override;
    virtual void Process(float deltaTime, InputSystem& inputSystem) override;
    virtual void Draw(Renderer& renderer) override;
    virtual void DebugDraw() override;

    void Restart();
    void GameOver();
    void Victory();

protected:
private:
    SceneGameLevel(const SceneGameLevel& sceneGameLevel);
    SceneGameLevel& operator=(const SceneGameLevel& sceneGameLevel);

    // Member data: 
public:
    // None
protected:
    SoundSystem* m_pSoundSystem;
    Level* m_pLevel; // Pointer to the Level object
    Renderer* m_pRenderer;

    Sprite* m_pGameOverSprite;
    Sprite* m_pRestartPromptText;
    Sprite* m_pQuitPromptText;
    Sprite* m_pBlackSquare;
    Sprite* m_pVictorySprite;

    ParticleEmitter* m_pLeftVictoryEmitter;
    ParticleEmitter* m_pRightVictoryEmitter;
    ParticleEmitter* m_pPlayerExplodeEmitter;

    bool m_isGameOver;  // Flag to track if GameOver has been called
    bool isVictory;

private:
    // None
};

#endif // __SCENEGAMELEVEL_H
