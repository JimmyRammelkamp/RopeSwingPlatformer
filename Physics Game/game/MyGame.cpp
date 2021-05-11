#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) :
	m_player(400, 540, 0, 0, 0),
	testDot(100, 100, 5, CColor::Red(), 0),
	m_key(400, 580, "key.png", CColor::White(), 0)
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
	currentswing = 0;
	ableToGrab = true;
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	if (m_state == AIRBORNE)
		m_player.Accelerate(0, -50);


	// Run and Stand

	if (m_state == STANDING || m_state == RUNNING)
	{
		m_player.SetVelocity(0, 0);
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
		{
			m_player.Accelerate(-300, 0);
			if (m_state != RUNNING || m_side != LEFT)
				m_player.SetAnimation("run_left");
			m_state = RUNNING;
			m_side = LEFT;
		}
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
		{
			m_player.Accelerate(300, 0);
			if (m_state != RUNNING || m_side != RIGHT)
				m_player.SetAnimation("run_right");
			m_state = RUNNING;
			m_side = RIGHT;
		}
		else
		{
			if (m_state == RUNNING)
				m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
			m_state = STANDING;
		}
	}

	if (m_state == HANGING || m_state == CLIMBING)
	{
		m_player.SetVelocity(0, 0);

		if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP))
		{
			m_player.Accelerate(0, 100);
			if (m_state != CLIMBING)
				m_player.SetAnimation("climb");
			m_state = CLIMBING;
		}
		else if (IsKeyDown(SDLK_s) || IsKeyDown(SDLK_DOWN))
		{
			m_player.Accelerate(0, -100);
			if (m_state != CLIMBING)
				m_player.SetAnimation("climb");
			m_state = CLIMBING;
		}
		else
		{
			if (m_state == CLIMBING)
				m_player.SetImage("hang");
			m_state = HANGING;
		}
	}
	if (m_state == SWINGING)
	{
		for (CSprite* pSprite : m_sprites)
			if ((string)pSprite->GetProperty("tag") == "swing")
				if (pSprite->GetStatus() == currentswing)
				{
					m_player.SetRotation(pSprite->GetRotation());
					m_player.SetPosition(pSprite->GetPos() + ((pSprite->GetCenter() - pSprite->GetPos()) * 2));
				}
		
	}


	// Pre-Update Position
	CVector v0 = m_player.GetPos();

	// Updates
	for (CSprite *pSprite : m_sprites)
		pSprite->Update(t);
	m_player.Update(t);

	int h = m_player.GetHeight() / 2 - 1;
	int w = m_player.GetWidth() / 2;
	
	bool bTouchingPlatform = false;
	bool bTouchingRope = false;
	bool bTouchingSwing = false;
	
	for (CSprite* pSprite : m_sprites)
		if (m_player.HitTest(pSprite, 0))
		{
			// platforms and walls
			if ((string)pSprite->GetProperty("tag") == "platform")
			{
				if (v0.m_y >= pSprite->GetTop() + h)
				{
					if (m_player.GetYVelocity() < -1500)
					{
						lives--;
						m_player.SetPos(20, 34);
					}

					m_player.SetVelocity(0, 0);
					m_player.SetY(pSprite->GetTop() + h);
					bTouchingPlatform = true;
				}
				if (v0.m_y <= pSprite->GetBottom() - h &&  m_state != CLIMBING)
				{
					m_player.SetYVelocity(m_player.GetYVelocity() - 100);
					m_player.SetY(pSprite->GetBottom() - h);
				}
				if (v0.m_x <= pSprite->GetLeft() - w)
				{
					m_player.SetXVelocity(m_player.GetXVelocity() - 10);
					m_player.SetX(pSprite->GetLeft() - w);
				}
				if (v0.m_x >= pSprite->GetRight() + w)
				{
					m_player.SetXVelocity(m_player.GetXVelocity() + 10);
					m_player.SetX(pSprite->GetRight() + w);
				}
			}
			//Rope
			if ((string)pSprite->GetProperty("tag") == "rope")
			{
				bTouchingRope = true;
				m_player.SetVelocity(0, 0);
				m_player.SetX(pSprite->GetX());
			}
			//Swing
			if ((string)pSprite->GetProperty("tag") == "swing")
			{
				if (pSprite->GetStatus() != currentswing) ableToGrab = true;

				if (ableToGrab)
				{

					currentswing = pSprite->GetStatus();
					bTouchingSwing = true;
				}
				
				//m_state = SWINGING;
				
				//m_player.SetRotation(pSprite->GetRotation());
				//m_player.SetPosition(pSprite->GetPos() + ((pSprite->GetCenter() - pSprite->GetPos()) * 2));

				//m_player.SetVelocity(0, 0);
				//m_player.SetX(pSprite->GetX());
				//m_player.SetPivot(pSprite->GetPosition());
				//m_player.SetPosition(pSprite->GetPos());
				//m_player.SetPos()
					
				//pSprite->SetOmega(50);
			}
			//Enemy
			if ((string)pSprite->GetProperty("tag") == "enemy")
			{
				lives--;
				m_player.SetPos(20, 34);	
			}
			//Hazards
			if ((string)pSprite->GetProperty("tag") == "hazard")
			{
				lives--;
				m_player.SetPos(20, 34);
			}
			//Key
			if ((string)pSprite->GetProperty("tag") == "collectible")
			{
				pSprite->Delete();
				key = true;
			}
			//gate
			if ((string)pSprite->GetProperty("tag") == "gate")
			{
				if (key == true)
				{
					gamewon = true;
					GameOver();
				}
			}
		}

	for (CSprite* pSprite : m_sprites)
		if ((string)pSprite->GetProperty("tag") == "swing")
		{
			float o = pSprite->GetOmega();
			float a = pSprite->GetRotation();
			pSprite->SetOmega(o - a / 125);
			//if (pSprite->GetStatus() == 1) testDot.SetPosition( pSprite->GetPos() + ((pSprite->GetCenter() - pSprite->GetPos()) * 2) );
			//if (pSprite->GetStatus() == 1) testDot.SetPosition(pSprite->GetCenter());
		}

	for (CSprite* pEnemy : m_sprites)
		if ((string)pEnemy->GetProperty("tag") == "enemy")
			for (CSprite* pCollider : m_sprites)
			{
				if ((string)pCollider->GetProperty("tag") == "lcollider"
					&& pEnemy->HitTest(pCollider, 0))
				{
					if (pEnemy->GetXVelocity() < 0)
					{
						pEnemy->SetVelocity(-pEnemy->GetVelocity());
						pEnemy->SetOmega(3.82 * 100);
					}
				}
				else
					if ((string)pCollider->GetProperty("tag") == "rcollider"
						&& pEnemy->HitTest(pCollider, 0))
					{
						if (pEnemy->GetXVelocity() > 0)
						{
							pEnemy->SetVelocity(-pEnemy->GetVelocity());
							pEnemy->SetOmega(3.82 * -100);
						}
					}
			}

	if (m_state == AIRBORNE && bTouchingRope /*|| m_state == AIRBORNE && bTouchingSwing*/)
	{
		m_state = HANGING;
		m_player.SetImage("hang");
	}

	if (m_state == AIRBORNE && bTouchingSwing)
	{
		m_state = SWINGING;
		m_player.SetImage("hang");
	}

	// processing of airborne condition
	if (m_state == AIRBORNE && bTouchingPlatform)
	{
		// just landed
		m_state = STANDING;
		m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
		ableToGrab = true;
	}

	if (m_state != AIRBORNE && !bTouchingPlatform && !bTouchingRope && !bTouchingSwing)
	{
		// just taken off
		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
		
	}

	if (lives == 0) GameOver();
	
	m_sprites.delete_if(deleted);
}

void CMyGame::OnDraw(CGraphics* g)
{
	for (CSprite* pSprite : m_sprites)
		if ((string)pSprite->GetProperty("tag") != "rcollider"
			&& (string)pSprite->GetProperty("tag") != "lcollider")
			pSprite->Draw(g);

	m_player.Draw(g);
	//testDot.Draw(g);

	if (key) m_key.Draw(g);
	
	*g << font(14) << color(CColor::Black()) << xy(100, 580) << "Lives:" << lives;


	//DEBUG
	*g << font(14) << color(CColor::Red()) << xy(700, 580) << "Velocity:" << m_player.GetYVelocity();
	*g << font(14) << color(CColor::Red()) << xy(700, 560) << "x" << m_player.GetX();
	*g << font(14) << color(CColor::Red()) << xy(700, 540) << "y" << m_player.GetY();
	*g << font(14) << color(CColor::Red()) << xy(700, 520) << "currentSwing" << currentswing;
	

	for (CSprite* pSprite : m_sprites)
		if ((string)pSprite->GetProperty("tag") == "swing")
			if (pSprite->GetStatus() == 1)
			{
				*g << font(14) << color(CColor::Red()) << xy(700, 500) << "omega" << pSprite->GetOmega();
				*g << font(14) << color(CColor::Red()) << xy(700, 480) << "rotation" << pSprite->GetRotation();
			}
			
		

	if (m_state == AIRBORNE)
		*g << font(14) << color(CColor::Red()) << xy(400, 550) << "AIRBORNE";
	if (m_state == STANDING)
		*g << font(14) << color(CColor::Red()) << xy(400, 550) << "STANDING";
	if (m_state == RUNNING)
		*g << font(14) << color(CColor::Red()) << xy(400, 550) << "RUNNING";
	if (m_state == CLIMBING)
		*g << font(14) << color(CColor::Red()) << xy(400, 550) << "CLIMBING";
	if (m_state == HANGING)
		*g << font(14) << color(CColor::Red()) << xy(400, 550) << "HANGING";
	if (m_state == SWINGING)
		*g << font(14) << color(CColor::Red()) << xy(400, 550) << "SWINGING";

	if (IsGameOverMode())
	{
		if (gamewon)
		{
			*g << font(60) << color(CColor::Green()) << vcenter << center << " GAME WON ";
		}
		else
		{
			*g << font(60) << color(CColor::Red()) << vcenter << center << " GAME OVER ";
		}
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_player.LoadImage("player.png", "stand_right", 11, 6, 0, 0, CColor::White());
	m_player.LoadImage("player.png", "stand_left", 11, 6, 0, 1, CColor::White());
	m_player.AddImage("player.png", "run_right",     11, 6, 0, 0, 10, 0, CColor::White());
	m_player.AddImage("player.png", "run_left",      11, 6, 0, 1, 10, 1, CColor::White());
	m_player.LoadImage("player.png", "jump_right",   11, 6, 3, 2, CColor::White());
	m_player.LoadImage("player.png", "jump_left",    11, 6, 3, 3, CColor::White());
	m_player.LoadImage("player.png", "crouch_right", 11, 6, 2, 4, CColor::White());
	m_player.LoadImage("player.png", "crouch_left",  11, 6, 2, 5, CColor::White());
	m_player.LoadImage("player.png", "hang",		 11, 6, 10, 2, CColor::White());
	m_player.AddImage("player.png", "climb",		 11, 6, 9, 2, 10, 2, CColor::White());
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	lives = 10;
	gamewon = false;
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	// Clean up first
	for (CSprite *pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();

	CSprite *pSprite;

	switch (nLevel)
	{
	case 1:// build Level 1 sprites

		// spawn the player
		m_player.SetPos(20, 100);
		m_player.SetImage("stand_right");

		// spawn all other sprites here ...

		//PLATFORMS

		//left wall
		pSprite = new CSpriteRect(-10, 300, 20, 800, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//right wall
		pSprite = new CSpriteRect(810, 300, 20, 800, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//floor
		pSprite = new CSpriteRect(400, 10, 800, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//low right
		pSprite = new CSpriteRect(630, 70, 200, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//above rope right
		pSprite = new CSpriteRect(600, 240, 200, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//small middle
		pSprite = new CSpriteRect(400, 240, 50, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//lower left
		pSprite = new CSpriteRect(200, 200, 200, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//Center Left
		pSprite = new CSpriteRect(50, 280, 100, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//Top with Gate
		pSprite = new CSpriteRect(700, 520, 200, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//Top with Key
		pSprite = new CSpriteRect(125, 520, 250, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//ROPES

		//first rope
		pSprite = new CSpriteRect(650, 198, 3, 100, CColor::DarkGray(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		//right rope
		pSprite = new CSpriteRect(780, 403, 3, 250, CColor::DarkGray(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		//center rope
		pSprite = new CSpriteRect(400, 500, 3, 200, CColor::DarkGray(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		//left rope
		pSprite = new CSpriteRect(150, 453, 3, 150, CColor::DarkGray(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		//Swings

		pSprite = new CSpriteRect(200, 150, 3, 150, CColor::Blue(), GetTime());
		pSprite->SetStatus(1);
		pSprite->SetPivot(pSprite->GetX(), pSprite->GetY() + 75);
		pSprite->SetOmega(50);
		pSprite->SetProperty("tag", "swing");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(400, 100, 3, 100, CColor::Red(), GetTime());
		pSprite->SetStatus(2);
		pSprite->SetPivot(pSprite->GetX(), pSprite->GetY() + 50);
		pSprite->SetOmega(-50);
		pSprite->SetProperty("tag", "swing");
		m_sprites.push_back(pSprite);
		

		//HAZARDS
		
		////first hazard
		//pSprite = new CSprite(680, 90, "spikes.png", CColor::White(), GetTime());
		//pSprite->SetProperty("tag", "hazard");
		//m_sprites.push_back(pSprite);

		//lowerleft
		pSprite = new CSprite(270, 220, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		//lowerleft
		pSprite = new CSprite(0, 330, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		pSprite->SetRotation(90);
		m_sprites.push_back(pSprite);

		//ENEMIES

		//floor enemy
		/*pSprite = new CSprite(790, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(-200, 0);
		pSprite->SetOmega(3.82 * -100);
		m_sprites.push_back(pSprite);*/

		pSprite = new CSpriteRect(100, 30, 5, 5, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "lcollider");
		m_sprites.push_back(pSprite);
		
		pSprite = new CSpriteRect(790, 30, 5, 5, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		//rope enemy 1
		pSprite = new CSprite(790, 400, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(-25, 0);
		pSprite->SetOmega(3.82 * -100);
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(700, 400, 5, 5, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "lcollider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(805, 400, 5, 5, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		//rope enemy 2
		pSprite = new CSprite(715, 470, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(25, 0);
		pSprite->SetOmega(3.82 * -100);
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(700, 470, 5, 5, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "lcollider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(805, 470, 5, 5, CColor::Red(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		//Key
		pSprite = new CSprite(50, 550, "key.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "collectible");
		m_sprites.push_back(pSprite);

		//Gate
		pSprite = new CSprite(670, 560, "gate.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "gate");
		m_sprites.push_back(pSprite);


		break;

	case 2:// Level 2 (EMPTY)

		break;
	}

	// any initialisation common to all levels
	m_player.SetImage("stand_right");
	m_player.SetVelocity(0, 0);
	m_player.SetRotation(0);
	m_state = STANDING;
	m_side = RIGHT;
	key = false;
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

	if ((sym == SDLK_w || sym == SDLK_UP) && (m_state == STANDING || m_state == RUNNING /*|| SWINGING*/))
	{
		m_player.Accelerate(0, 800);
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
			m_player.Accelerate(-300, 0);
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
			m_player.Accelerate(300, 0);

		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");

	}

	if (m_state == SWINGING)
	{
		if ( sym == SDLK_a || sym == SDLK_LEFT)
		{
			m_player.SetVelocity(-300, 400);
			if (m_state != AIRBORNE || m_side != LEFT)
				m_player.SetImage("jump_left");
			m_state = AIRBORNE;
			m_side = LEFT;
			m_player.SetRotation(0);

			ableToGrab = false;
		}
		else if (sym == SDLK_d || sym == SDLK_RIGHT)
		{
			m_player.SetVelocity(300, 400);
			if (m_state != AIRBORNE || m_side != RIGHT)
				m_player.SetImage("jump_right");
			m_state = AIRBORNE;
			m_side = RIGHT;
			m_player.SetRotation(0);

			ableToGrab = false;
		}
	}

	

}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
