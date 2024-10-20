// COMP710 GP Framework 2022
// This include:
#include "scenegamelevel.h"

// Local includes:
#include "renderer.h" 
#include "imgui.h"
#include "soundsystem.h"
#include "inputsystem.h"
#include "levelmanager.h"
#include "player.h"
#include "enemy.h"
#include "sprite.h"
#include "inlinehelpers.h"
#include <iostream>
#include "particleemitter.h"


// Library includes:
#include <cassert>
#include "iniparser.h"
#include "game.h"

SceneGameLevel::SceneGameLevel()
    : m_pSoundSystem(nullptr), m_pLevel(nullptr), 
    m_isGameOver(false), isVictory(false), m_pBlackSquare(nullptr),
    m_pGameOverSprite(nullptr), m_pLeftVictoryEmitter(nullptr),
    m_pRightVictoryEmitter(nullptr), m_pPlayerExplodeEmitter(nullptr),
    m_pQuitPromptText(nullptr), m_pRenderer(nullptr),
    m_pRestartPromptText(nullptr), m_pVictorySprite(nullptr)
{
}

SceneGameLevel::~SceneGameLevel()
{
    LevelManager::DestroyInstance();

    // Clean up sprites and emitters
    delete m_pBlackSquare; m_pBlackSquare = nullptr;
    delete m_pGameOverSprite; m_pGameOverSprite = nullptr;
    delete m_pLeftVictoryEmitter; m_pLeftVictoryEmitter = nullptr;
    delete m_pRightVictoryEmitter; m_pRightVictoryEmitter = nullptr;
    delete m_pPlayerExplodeEmitter; m_pPlayerExplodeEmitter = nullptr;
    delete m_pQuitPromptText; m_pQuitPromptText = nullptr;
    delete m_pRestartPromptText; m_pRestartPromptText = nullptr;
    delete m_pVictorySprite; m_pVictorySprite = nullptr;

    m_pSoundSystem = nullptr;
    m_pRenderer = nullptr;
    m_pLevel = nullptr;
}

bool
SceneGameLevel::Initialise(Renderer& renderer, SoundSystem* soundSystem)
{
    m_pRenderer = &renderer;
    m_pSoundSystem = soundSystem;
    //load background sound
    m_pSoundSystem->LoadSound("sounds\\BM_GameLoopMusic4.mp3",false,true,false);
    //load death sound
    m_pSoundSystem->LoadSound("sounds\\SE-EnemyExplosion.wav");
    //load powerup sound
    m_pSoundSystem->LoadSound("sounds\\SE-ShipSpawn.wav");

    // Load the level using LevelManager
    std::string levelFilePath = "levels\\examplelevel.txt";
    m_pLevel = LevelManager::GetInstance().GetLevel(levelFilePath, renderer);

    if (m_pLevel == nullptr) {
        std::cerr << "Failed to load level from file: " << levelFilePath << std::endl;
        return false;
    }

    m_pLevel->Initialise(renderer);

    const int SCREEN_WIDTH = renderer.GetWidth();
    const int SCREEN_HEIGHT = renderer.GetHeight();


    m_pBlackSquare = renderer.CreateSprite("sprites\\blacksquare.png");
    m_pBlackSquare->SetX(SCREEN_WIDTH / 2);
    m_pBlackSquare->SetY(SCREEN_HEIGHT / 2);
    m_pBlackSquare->SetScale(2.0f);
    m_pBlackSquare->SetAlpha(0.75f);

    m_pGameOverSprite = renderer.CreateSprite("sprites\\gameover.png");
    m_pGameOverSprite->SetX(SCREEN_WIDTH / 2);
    m_pGameOverSprite->SetY(SCREEN_HEIGHT / 2);
    m_pGameOverSprite->SetScale(0.5f);

    m_pVictorySprite = renderer.CreateSprite("sprites\\victory.png");
    m_pVictorySprite->SetX(SCREEN_WIDTH / 2);
    m_pVictorySprite->SetY(SCREEN_HEIGHT / 2);
    m_pVictorySprite->SetScale(0.3f);

    // Load static text textures into the Texture Manager... 
    const char* restartprompt = "Press SPACE to Restart";
    renderer.CreateStaticText(restartprompt, 60);
    // Generate sprites that use the static text textures... 
    m_pRestartPromptText = renderer.CreateSprite(restartprompt);
    m_pRestartPromptText->SetX((int)(SCREEN_WIDTH / 2));
    m_pRestartPromptText->SetY((int)((SCREEN_HEIGHT / 2) + (m_pGameOverSprite->GetHeight() / 2) + (m_pRestartPromptText->GetHeight())));

    // Load static text textures into the Texture Manager... 
    const char* quitprompt = "Press X to Quit";
    renderer.CreateStaticText(quitprompt, 60);
    // Generate sprites that use the static text textures... 
    m_pQuitPromptText = renderer.CreateSprite(quitprompt);
    m_pQuitPromptText->SetX((int)(SCREEN_WIDTH / 2));
    m_pQuitPromptText->SetY((int)((SCREEN_HEIGHT / 2) + (m_pGameOverSprite->GetHeight() / 2) + (m_pQuitPromptText->GetHeight()) + (m_pRestartPromptText->GetHeight())));

    m_pLeftVictoryEmitter = new ParticleEmitter();
    m_pRightVictoryEmitter = new ParticleEmitter();
    m_pPlayerExplodeEmitter = new ParticleEmitter();

    m_pLeftVictoryEmitter->Initialise(renderer);
    m_pRightVictoryEmitter->Initialise(renderer);
    m_pPlayerExplodeEmitter->Initialise(renderer);

    m_pLeftVictoryEmitter->SetXY((float)(SCREEN_WIDTH / 3), (float)(SCREEN_HEIGHT / 2));
    m_pRightVictoryEmitter->SetXY((float)((SCREEN_WIDTH / 3)*2), (float)(SCREEN_HEIGHT / 2));

    m_pLeftVictoryEmitter->SetTint((float)GetRandom(0, 1), (float)GetRandom(0, 1), (float)GetRandom(0, 1));
    m_pLeftVictoryEmitter->SetTint((float)GetRandom(0, 1), (float)GetRandom(0, 1), (float)GetRandom(0, 1));
    m_pPlayerExplodeEmitter->SetTint(1.0f, 0.0f, 0.0f);

    m_pLeftVictoryEmitter->SetAccelerationScalar(100.0f);
    m_pRightVictoryEmitter->SetAccelerationScalar(100.0f);
    m_pPlayerExplodeEmitter->SetAccelerationScalar(15.0f);

    return true;
}


void SceneGameLevel::Process(float deltaTime, InputSystem& inputSystem)
{
    // Process level and other game elements
    if (m_pLevel) {
        m_pLevel->Process(deltaTime, inputSystem);

        // Check if the game is over
        if (!m_pLevel->IsPlaying() && !m_isGameOver) {
            GameOver();
            m_isGameOver = true;  // Set the flag to prevent GameOver from running again
        }

        // Allow restarting if the player presses space
        if ((m_isGameOver || isVictory) && inputSystem.GetKeyState(SDL_SCANCODE_SPACE)) {
            Restart();
        }
        // Allow restarting if the player presses space
        if ((m_isGameOver || isVictory) && inputSystem.GetKeyState(SDL_SCANCODE_X)) {
            Game::GetInstance().Quit();
        }

        if (m_isGameOver) {
            m_pBlackSquare->Process(deltaTime);

            Vector2 pos = m_pLevel->GetPlayer()->GetPosition();
            m_pPlayerExplodeEmitter->SetXY(pos.x, pos.y);
            m_pPlayerExplodeEmitter->Process(deltaTime);

            m_pGameOverSprite->Process(deltaTime);
            m_pRestartPromptText->Process(deltaTime);
            m_pQuitPromptText->Process(deltaTime);
        }

        if (m_pLevel->GetSpace() <= 0 && !isVictory) {
            //victory
            isVictory = true;
        }

        if (isVictory) {
            Victory();
        }

        if (isVictory) {
            //process victory sprites
            m_pBlackSquare->Process(deltaTime);

            m_pLeftVictoryEmitter->SetTint((float)GetRandom(0, 1), (float)GetRandom(0, 1), (float)GetRandom(0, 1));
            m_pLeftVictoryEmitter->Process(deltaTime);

            m_pRightVictoryEmitter->SetTint((float)GetRandom(0, 1), (float)GetRandom(0, 1), (float)GetRandom(0, 1));
            m_pRightVictoryEmitter->Process(deltaTime);

            m_pVictorySprite->Process(deltaTime);
            m_pRestartPromptText->Process(deltaTime);
            m_pQuitPromptText->Process(deltaTime);
        }
    }
}

void
SceneGameLevel::Draw(Renderer& renderer)
{
    // Draw the level and other game elements
    if (m_pLevel) {
        m_pLevel->Draw(renderer);

        if (m_isGameOver) {
            m_pBlackSquare->Draw(renderer);
            m_pLevel->GetPlayer()->Draw(renderer);
            m_pPlayerExplodeEmitter->Draw(renderer);
            m_pGameOverSprite->Draw(renderer);
            m_pRestartPromptText->Draw(renderer);
            m_pQuitPromptText->Draw(renderer);
        }

        if (isVictory) {
            //draw victory sprites
            m_pBlackSquare->Draw(renderer);
            m_pLeftVictoryEmitter->Draw(renderer);
            m_pRightVictoryEmitter->Draw(renderer);
            m_pVictorySprite->Draw(renderer);
            m_pRestartPromptText->Draw(renderer);
            m_pQuitPromptText->Draw(renderer);
        }
    }
}

void SceneGameLevel::DebugDraw()
{
    // Implement debug drawing if needed
    if (ImGui::Button("Restart")) {
        Restart();
    }
    ImGui::Checkbox("Show Victory Screen", &isVictory);
    ImGui::Checkbox("Show GameOver Screen", &m_isGameOver);
}

void SceneGameLevel::Restart() {
    delete m_pLeftVictoryEmitter;
    m_pLeftVictoryEmitter = nullptr;
    delete m_pRightVictoryEmitter;
    m_pRightVictoryEmitter = nullptr;
    delete m_pPlayerExplodeEmitter;
    m_pPlayerExplodeEmitter = nullptr;

    LevelManager::DestroyInstance();
    m_pLevel = nullptr;

    m_pLevel = LevelManager::GetInstance().GetLevel("levels\\examplelevel.txt", *m_pRenderer);
    m_pLevel->Initialise(*m_pRenderer);

    m_pLeftVictoryEmitter = new ParticleEmitter();
    m_pRightVictoryEmitter = new ParticleEmitter();
    m_pPlayerExplodeEmitter = new ParticleEmitter();

    m_pLeftVictoryEmitter->Initialise(*m_pRenderer);
    m_pRightVictoryEmitter->Initialise(*m_pRenderer);
    m_pPlayerExplodeEmitter->Initialise(*m_pRenderer);

    m_pLeftVictoryEmitter->SetActive(false);
    m_pRightVictoryEmitter->SetActive(false);
    m_pPlayerExplodeEmitter->SetActive(false);

    const int SCREEN_WIDTH = m_pRenderer->GetWidth();
    const int SCREEN_HEIGHT = m_pRenderer->GetHeight();

    m_pLeftVictoryEmitter->SetXY((float)(SCREEN_WIDTH / 3), (float)(SCREEN_HEIGHT / 2));
    m_pRightVictoryEmitter->SetXY((float)((SCREEN_WIDTH / 3) * 2), (float)(SCREEN_HEIGHT / 2));

    m_pLeftVictoryEmitter->SetTint((float)GetRandom(0, 1), (float)GetRandom(0, 1), (float)GetRandom(0, 1));
    m_pLeftVictoryEmitter->SetTint((float)GetRandom(0, 1), (float)GetRandom(0, 1), (float)GetRandom(0, 1));
    m_pPlayerExplodeEmitter->SetTint(1.0f, 0.0f, 0.0f);

    m_pLeftVictoryEmitter->SetAccelerationScalar(100.0f);
    m_pRightVictoryEmitter->SetAccelerationScalar(100.0f);
    m_pPlayerExplodeEmitter->SetAccelerationScalar(15.0f);

    m_isGameOver = false;  // Reset the flag when restarting the game
    isVictory = false;
}

void SceneGameLevel::GameOver() {
    // Show game over sprite or message
    //std::cout << "Game Over!" << std::endl;
    m_pPlayerExplodeEmitter->SetActive(true);

    // Stop all sounds and play the game over sound
    SoundSystem::GetInstance().StopAllSounds();
    SoundSystem::GetInstance().PlaySound("sounds\\SE-EnemyExplosion.wav");
}

void SceneGameLevel::Victory() {
    // Stop all sounds and play the victory sound
    SoundSystem::GetInstance().StopAllSounds();
    m_pLeftVictoryEmitter->SetActive(true);
    m_pRightVictoryEmitter->SetActive(true);
    
    //victory sound
    //SoundSystem::GetInstance().PlaySound("sounds\\SE-EnemyExplosion.wav");
}