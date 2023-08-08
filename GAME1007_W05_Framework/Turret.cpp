#include "Turret.h"
#include "States.h"
#include "Enemy.h"
#include "Bullet.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "MathManager.h"
#include "RenderManager.h"
#include "TextureManager.h"

Turret::Turret(const SDL_Rect src, const SDL_FRect dst) :Sprite(src, dst), m_angle(0.0), m_hasTarget(false), m_fireCtr(0)
{
	
}

void Turret::Update()
{
	// Make sure turret gets reloaded. Cooldown. 
	if (m_fireCtr > 0) m_fireCtr--;
	// Click and drag functionality. Note there is SDL_PointInFRect but the Point is float too.
	if (EVMA::MouseHeld(1) && COMA::PointAABBCheck(EVMA::GetMousePos(), m_dst))
	{ // Note: don't drag too fast. The two systems (OS for mouse, this engine) are different clocks.
		m_dst.x = EVMA::GetMousePos().x - m_dst.w / 2.0f; // Center of turret set to mouse pos.
		m_dst.y = EVMA::GetMousePos().y - m_dst.h / 2.0f;
	}
	// Reset targeting fields.
	m_angle = 0.0;
	m_hasTarget = false;

	// If there are no Enemies at all, don't acquire target, so return.
	if (GameState::Enemies().size() == 0) return;

	// Store center points of closest enemy and turret. For distance check.
	SDL_FPoint turretCenter = this->GetCenter();
	SDL_FPoint closestEnemy = GameState::Enemies()[0]->GetCenter();

	// Convert SDL_FPoint to SDL_FRect for Distance function
	SDL_FRect turretCenterRect = { turretCenter.x, turretCenter.y, 0.0f, 0.0f };
	SDL_FRect closestEnemyRect = { closestEnemy.x, closestEnemy.y, 0.0f, 0.0f };

	// Find closest enemy.
	for (unsigned i = 1; i < GameState::Enemies().size(); i++)
	{
		SDL_FPoint nextEnemy = GameState::Enemies()[i]->GetCenter();
		SDL_FRect nextEnemyRect = { nextEnemy.x, nextEnemy.y, 0.0f, 0.0f };

		if (MAMA::Distance(&turretCenterRect, &nextEnemyRect) < MAMA::Distance(&turretCenterRect, &closestEnemyRect))
			closestEnemy = nextEnemy;
	}

	// Is the closest enemy <= 300px away?
	if (MAMA::Distance(&turretCenterRect, &closestEnemyRect) <= 300.0)
	{
		// Track closest enemy.
		SDL_FPoint deltaVector = { closestEnemy.x - turretCenter.x, closestEnemy.y - turretCenter.y + 30.0f };
		m_angle = MAMA::Rad2Deg(MAMA::AngleBetweenPoints(deltaVector.y, deltaVector.x)) + 90.0;
		m_hasTarget = true;
		// Shoot closest enemy.
		if (m_fireCtr == 0)
		{
			SDL_FPoint normalVector = MAMA::Normalize(deltaVector);
			GameState::Bullets().push_back(
				new Bullet({ turretCenter.x - 2.0f, turretCenter.y - 2.0f, 4.0f, 4.0f },
					normalVector.x, normalVector.y));
			m_fireCtr = s_coolDown;
		} // Note. The Bullet should eventually know what Turret fired it in order to keep track of kills.
	}
}
int Turret::GetKills() const
{
	return m_kills;
}
void Turret::Render()
{
	
}

int Turret::s_coolDown = 120;