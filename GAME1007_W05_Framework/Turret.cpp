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
	// Click and drag functionality. Note there is SDL_PointInFRect but the Point is float too.
	
	// Store center points of closest enemy and turret. For distance check.
	
	// Find closest enemy.
	
	// Is the closest enemy <= 300px away?
	
}

void Turret::Render()
{
	
}

int Turret::s_coolDown = 120;