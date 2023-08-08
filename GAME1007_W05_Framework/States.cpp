#include <iostream>
#include "States.h"
#include "StateManager.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "Engine.h"
#include "tinyxml2.h"


using namespace std;
using namespace tinyxml2;

void State::Render()
{
	SDL_RenderPresent(REMA::Instance().GetRenderer());
}

TitleState::TitleState(){}

void TitleState::Enter()
{
	
}

void TitleState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_G))
	{
		STMA::ChangeState( new GameState() );
	}
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(REMA::Instance().GetRenderer(), 0, 0, 255, 255);
	SDL_RenderClear(REMA::Instance().GetRenderer());
	State::Render();
}

void TitleState::Exit()
{
	
}

void GameState::ClearTurrets()
{
	for (unsigned i = 0; i < m_turrets.size(); i++)
	{
		delete m_turrets[i];
		m_turrets[i] = nullptr;
	}
	m_turrets.clear();
	m_turrets.shrink_to_fit();
	// You can assimilate some parts of this code for deleting bullets and enemies.
}

GameState::GameState():m_spawnCtr(0) {}

void GameState::Enter()
{
	TEMA::Load("../Assets/img/Turret.png", "turret");
	TEMA::Load("../Assets/img/Enemies.png", "enemy");
	//spawning enm
	s_enemies.push_back(new Enemy({ 80,0,40,57 }, { 512.0f, -200.0f, 40.0f, 57.0f }));

	// Create the DOM and load the XML file.
	XMLDocument xmlDoc;
	XMLElement* pRoot = xmlDoc.NewElement("Root");
	xmlDoc.LoadFile("../Assets/dat/Turrets.xml");

	// Iterate through the Turret elements in the file and push_back new Turrets into the m_turrets vector.
	if (!pRoot)
	{
		pRoot = xmlDoc.NewElement("Root");
		xmlDoc.InsertEndChild(pRoot);
		xmlDoc.SaveFile("../Assets/dat/Turrets.xml");
	}

}

void GameState::Update()
{
	// Parse T and C events.
	if (EVMA::KeyPressed(SDL_SCANCODE_T)) 
	{
		Turret* newTurret = new Turret({ 50, 618, 100, 100 }, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_turrets.push_back(newTurret);
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_C)) 
	{
		ClearTurrets(); // Call the ClearTurrets method
	}
	// Update all GameObjects individually. Spawn enemies. Update turrets. Update enemies. Update bullets.
	if (m_spawnCtr++ % 120 == 0)
	{
		Enemy* newEnemy = new Enemy({ 80, 0, 40, 57 }, { (float)(rand() % (1024 - 40)), -57.0f, 40.0f, 200.0f});
		s_enemies.push_back(newEnemy);
	}
	// Cleanup bullets and enemies that go off screen.
	for (unsigned i = 0; i < s_bullets.size(); i++)
		s_bullets[i]->Update();
	for (unsigned i = 0; i < s_enemies.size(); i++)
		s_enemies[i]->Update();
	for (unsigned i = 0; i < s_bullets.size();) 
	{
		if (s_bullets[i]->GetDst()->x < 0 || s_bullets[i]->GetDst()->x > kWidth ||
        s_bullets[i]->GetDst()->y < 0 || s_bullets[i]->GetDst()->y > kHeight)
		{
			delete s_bullets[i];
			s_bullets[i] = nullptr;
			s_bullets.erase(s_bullets.begin() + i);
		} 
		else
		{
			i++;
		}
	}
	for (unsigned i = 0; i < s_enemies.size();) 
	{
		if (s_enemies[i]->GetDst()->x < 0 || s_enemies[i]->GetDst()->x > kWidth ||
			s_enemies[i]->GetDst()->y < 0 || s_enemies[i]->GetDst()->y > kHeight) 
		{
			delete s_enemies[i];
			s_enemies[i] = nullptr;
			s_enemies.erase(s_enemies.begin() + i);
		}
		else 
		{
			i++;
		}
	}
	// Check for collisions between bullets and enemies.
	
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(REMA::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(REMA::Instance().GetRenderer());

	for (unsigned i = 0; i < m_turrets.size(); i++)
		m_turrets[i]->Render();
	for (unsigned i = 0; i < s_enemies.size(); i++)
		s_enemies[i]->Render();
	for (unsigned i = 0; i < s_bullets.size(); i++)
		s_bullets[i]->Render();

	SDL_Rect spawnBox = { 50, 618, 100, 100 };
	SDL_SetRenderDrawColor(REMA::Instance().GetRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawRect(REMA::Instance().GetRenderer(), &spawnBox);

	// This code below prevents SDL_RenderPresent from running twice in one frame.
	if ( dynamic_cast<GameState*>( STMA::GetStates().back() ) ) // If current state is GameState.
		State::Render();
}

void GameState::Exit()
{
	// You can clear all children of the root node by calling .DeleteChildren(); and this will essentially clear the DOM.

	// Iterate through all the turrets and save their positions as child elements of the root node in the DOM.

	XMLDocument xmlDoc;
	XMLElement* pTurretsElement = xmlDoc.NewElement("Turrets");

	for (unsigned i = 0; i < m_turrets.size(); i++)
	{
		XMLElement* pTurretElement = xmlDoc.NewElement("Turret");
		pTurretElement->SetAttribute("x", m_turrets[i]->GetDst()->x);
		pTurretElement->SetAttribute("y", m_turrets[i]->GetDst()->y);
		pTurretElement->SetAttribute("kills", m_turrets[i]->GetKills());

		pTurretsElement->InsertEndChild(pTurretElement);
	}

	xmlDoc.InsertEndChild(pTurretsElement);
	xmlDoc.SaveFile("Turrets.xml");
	ClearTurrets(); // Deallocate all turrets, then all other objects.
	for (unsigned i = 0; i < s_enemies.size(); i++)
	{
		delete s_enemies[i];
		s_enemies[i] = nullptr;
	}
	s_enemies.clear();
	s_enemies.shrink_to_fit();
	for (unsigned i = 0; i < s_bullets.size(); i++)
	{
		delete s_bullets[i];
		s_bullets[i] = nullptr;
	}
	s_bullets.clear();
	s_bullets.shrink_to_fit();
}

void GameState::Resume()
{
	
}

// This is how static properties are allocated.
std::vector<Bullet*> GameState::s_bullets;
std::vector<Enemy*> GameState::s_enemies;