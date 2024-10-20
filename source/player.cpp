#include "player.h"
#include "renderer.h"
#include "inputsystem.h"
#include "xboxcontroller.h"
#include "entity.h"
#include "enemy.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "tile.h"
#include "powerup.h"
#include <iostream>

Player::Player(int startX, int startY, int initialLength, Renderer& renderer)
    : m_directionX(30), m_directionY(0), m_pMoving(true), m_elapsedTime(0), m_pPowerUp(0), m_powerUpTime(0) // Start moving to the right
{
    // Initialize the snake with the initial length at the starting position
    for (int i = 0; i < initialLength; ++i) {
        Entity* newEntity = new Entity();
        newEntity->Initialise(renderer, "sprites\\ballAnimated.png");
        newEntity->SetPosition(startX - i, startY);
        m_segments.push_back(newEntity);  // Place segments horizontally
    }

    m_pRenderer = &renderer;

    const int SCREEN_WIDTH = renderer.GetWidth();
    const int SCREEN_HEIGHT = renderer.GetHeight();

    m_pW = renderer.CreateSprite("sprites\\w.png");
    m_pA = renderer.CreateSprite("sprites\\a.png");
    m_pS = renderer.CreateSprite("sprites\\s.png");
    m_pD = renderer.CreateSprite("sprites\\d.png");

    m_pW->SetX((int)(SCREEN_WIDTH / 1.2));
    m_pW->SetY((int)(SCREEN_HEIGHT / 1.2));

    m_pA->SetX((int)((SCREEN_WIDTH / 1.2) - (m_pS->GetWidth())));
    m_pA->SetY((int)((SCREEN_HEIGHT / 1.2) + (m_pW->GetHeight())));

    m_pS->SetX((int)(SCREEN_WIDTH / 1.2));
    m_pS->SetY((int)((SCREEN_HEIGHT / 1.2) + (m_pW->GetHeight())));

    m_pD->SetX((int)((SCREEN_WIDTH / 1.2) + (m_pS->GetWidth())));
    m_pD->SetY((int)((SCREEN_HEIGHT / 1.2) + (m_pW->GetHeight())));
}

Player::~Player() {
    for (auto it = begin(m_segments); it != end(m_segments); ++it) {
        delete* it;
        *it = 0;
    }
    m_segments.clear();
    delete m_pW;
    m_pW = 0;
    delete m_pA;
    m_pA = 0;
    delete m_pS;
    m_pS = 0;
    delete m_pD;
    m_pD = 0;

    m_pRenderer = 0;
}

void Player::Process(float deltaTime, InputSystem& inputSystem) {

    // Accumulate deltaTime
    m_elapsedTime += deltaTime;

    // Movement interval (in seconds) - Adjust this value to control speed (e.g., 0.1 for 10 moves per second)
    const float movementInterval = 0.1f;

    for (Entity* segment : m_segments) {
        if (IsPowerful()) {
            segment->GetSprite()->SetBlueTint(0.0f);
        }
        else {
            segment->GetSprite()->SetBlueTint(1.0f);
        }
        segment->Process(deltaTime);
    }

    if (m_pPowerUp) {
        m_powerUpTime += deltaTime;
        const float powerUpDuration = 5.0f;
        //std::cout << 5.0f - m_powerUpTime;
        
        if (m_powerUpTime >= powerUpDuration) {
            makeWeak();
        }
    }

    // Only update direction and position when enough time has passed
    if (m_elapsedTime >= movementInterval) {
        // Reset the elapsed time
        m_elapsedTime = 0.0f;

        // Update direction based on input
        if (inputSystem.GetKeyState(SDL_SCANCODE_W) || inputSystem.GetKeyState(SDL_SCANCODE_UP)) {
            m_directionX = 0;
            if (m_directionY != 30 || m_segments.size() < 2) {
                m_directionY = -30;
            }
        }
        else if (inputSystem.GetKeyState(SDL_SCANCODE_S) || inputSystem.GetKeyState(SDL_SCANCODE_DOWN)) {
            m_directionX = 0;
            if (m_directionY != -30 || m_segments.size() < 2) {
                m_directionY = 30;
            }
        }
        else if (inputSystem.GetKeyState(SDL_SCANCODE_A) || inputSystem.GetKeyState(SDL_SCANCODE_LEFT)) {
            if (m_directionX != 30 || m_segments.size() < 2) {
                m_directionX = -30;
            }
            m_directionY = 0;
        }
        else if (inputSystem.GetKeyState(SDL_SCANCODE_D) || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT)) {
            if (m_directionX != -30 || m_segments.size() < 2) {
                m_directionX = 30;
            }
            m_directionY = 0;
        }
        AnimatedSprite* snakeSprite = m_segments[0]->GetSprite();
        int width = snakeSprite->GetWidth();
        if (m_pMoving) {
            // Move each segment of the snake
            int newX = (int)(m_segments.front()->GetPosition().x + m_directionX);
            int newY = (int)(m_segments.front()->GetPosition().y + m_directionY);

            // Move the body (from the last segment to the second one)
            for (size_t i = m_segments.size() - 1; i > 0; --i) {
                m_segments[i]->SetPosition((int)(m_segments[i - 1]->GetPosition().x), (int)(m_segments[i - 1]->GetPosition().y));
            }

            // Move the head
            m_segments.front()->SetPosition(newX, newY);
        }
        
    }

    RenderInput(inputSystem);

    m_pW->Process(deltaTime);
    m_pA->Process(deltaTime);
    m_pS->Process(deltaTime);
    m_pD->Process(deltaTime);
}

void Player::Grow() {
    // Add a new segment at the position of the tail
    Entity* newSegment = new Entity();
    newSegment->Initialise(*m_pRenderer, "sprites\\ballAnimated.png");
    newSegment->SetPosition((int)(m_segments.back()->GetPosition().x), (int)(m_segments.back()->GetPosition().y));
    m_segments.push_back(newSegment);
    //m_pPowerUp = !m_pPowerUp;
}

bool Player::IsCollidingWithSelf() const {
    // Check if the head is colliding with any other segment
    int headX = (int)m_segments.front()->GetPosition().x;
    int headY = (int)m_segments.front()->GetPosition().y;
    for (size_t i = 1; i < m_segments.size(); ++i) {
        if (m_segments[i]->GetPosition().x == headX && m_segments[i]->GetPosition().y == headY) {
            return true;
        }
    }
    return false;
}

void Player::Draw(Renderer& renderer) {
    for (Entity* segment : m_segments) {
        segment->Draw(renderer);  // Draw each segment of the snake
    }
    m_pW->Draw(renderer);
    m_pA->Draw(renderer);
    m_pS->Draw(renderer);
    m_pD->Draw(renderer);
}

bool Player::CollideEnemy(Enemy* enemy) {
    return (m_segments.front()->GetPosition().x == enemy->m_position.x && m_segments.front()->GetPosition().y == enemy->m_position.y);
}

bool Player::CollidePowerUp(PowerUp* pUp) {
    if (pUp->IsAlive()) {
        return (m_segments.front()->GetPosition().x == pUp->m_position.x && m_segments.front()->GetPosition().y == pUp->m_position.y);
    }
    else {
        return false;
    }
}

bool Player::CollideTile(Tile* tile) {
    return (m_segments.front()->GetPosition().x == tile->GetPos()->x && m_segments.front()->GetPosition().y == tile->GetPos()->y);
}

bool Player::GetMoving() {
    return m_pMoving;
}

void Player::SetMoving(bool moving) {
    m_pMoving = moving;
}

bool Player::IsPowerful() {
    return m_pPowerUp;
}
void Player::setPowerful(bool powerUp) {
    m_pPowerUp = powerUp;
}

void Player::makePowerful() {
    m_powerUpTime = 0.0f;
    m_pPowerUp = true;
}

void Player::makeWeak() {
    m_pPowerUp = false;
}

int Player::GetSize() {
    return m_segments.size();
}

bool Player::IsAlive() {
    return m_pMoving;
}

void Player::RenderInput(InputSystem& inputSystem){
    ButtonState wState = (inputSystem.GetKeyState(SDL_SCANCODE_W));
    ButtonState upState = (inputSystem.GetKeyState(SDL_SCANCODE_UP)); 
    //ButtonState xboxUpState = inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP);
    if (wState == BS_PRESSED || upState == BS_PRESSED/* || xboxUpState == BS_PRESSED*/)
    {
        m_pW->SetAlpha(0.5f);
    }
    else if (wState == BS_RELEASED || upState == BS_RELEASED /* || xboxUpState == BS_RELEASED */)
    {
        m_pW->SetAlpha(1.0f);
    }

    ButtonState aState = (inputSystem.GetKeyState(SDL_SCANCODE_A));
    ButtonState leftState = (inputSystem.GetKeyState(SDL_SCANCODE_LEFT));
    //ButtonState xboxLeftState = inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    if (aState == BS_PRESSED || leftState == BS_PRESSED /* || xboxLeftState == BS_PRESSED*/)
    {
        m_pA->SetAlpha(0.5f);
    }
    else if (aState == BS_RELEASED || leftState == BS_RELEASED /* || xboxLeftState == BS_RELEASED*/)
    {
        m_pA->SetAlpha(1.0f);
    }

    ButtonState sState = (inputSystem.GetKeyState(SDL_SCANCODE_S));
    ButtonState downState = (inputSystem.GetKeyState(SDL_SCANCODE_DOWN));
    //ButtonState xboxDownState = inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    if (sState == BS_PRESSED || downState == BS_PRESSED /* || xboxDownState == BS_PRESSED*/)
    {
        m_pS->SetAlpha(0.5f);
    }
    else if (sState == BS_RELEASED || downState == BS_RELEASED /* || xboxDownState == BS_RELEASED*/)
    {
        m_pS->SetAlpha(1.0f);
    }

    ButtonState dState = (inputSystem.GetKeyState(SDL_SCANCODE_D));
    ButtonState rightState = (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT));
    //ButtonState xboxRightState = inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    if (dState == BS_PRESSED || rightState == BS_PRESSED /* || xboxRightState == BS_PRESSED*/)
    {
        m_pD->SetAlpha(0.5f);
    }
    else if (dState == BS_RELEASED || rightState == BS_RELEASED /* || xboxRightState == BS_RELEASED*/)
    {
        m_pD->SetAlpha(1.0f);
    }
}

Vector2 Player::GetPosition() {
    return m_segments.front()->GetPosition();
}