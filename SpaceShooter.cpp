#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <random>
#include <cmath>
#include <sstream>

#include "CommonFunctions.h"
#include "ConsoleGameEngine.hpp"

using namespace std;

// class definitions
class Entity {
public:
	int xPos;
	int yPos;
	int height;
	int width;
	int speed;
	bool exists;
};

class Enemy : public Entity {
public:
	int health = 1;
	std::wstring sprite[3];

public:
	Enemy() {
		sprite[0] = L"  [-]   ";
		sprite[1] = L"[======]";
		sprite[2] = L"  [-]   ";

		width = sprite[0].size();
		height = 3;

		yPos = 0;
		xPos = 0;
		exists = true;
		speed = 5;
	}
	~Enemy() {
		exists = false;
	}
};
class CruiserEnemy : public Entity {
public:
	int health = 3;
	std::wstring sprite[4];
	bool canFire = true;
	int fireCoolDownItterations = 100;
public:
	CruiserEnemy() {
		sprite[0] = L"  <<<<<  //   {{    ";
		sprite[1] = L"  <[-]===[]===[]==={";
		sprite[2] = L"  <[-]===[]===[]==={";
		sprite[3] = L"  <<<<<  \\\\   {{    ";

		width = sprite[0].size();
		height = 4;

		yPos = 0;
		xPos = 0;
		exists = true;
		speed = 5;
	}
	~CruiserEnemy() {
		exists = false;
	}
};
class EnemyBullet : public Entity {
public:
	std::wstring sprite[3];
public:
	EnemyBullet() {
		sprite[0] = L"   ";
		sprite[1] = L"<<<";
		sprite[2] = L"   ";

		width = sprite[0].size();
		height = 3;

		yPos = 0;
		xPos = 0;
		exists = true;
		speed = 200;
	}
	~EnemyBullet() {
		exists = false;
	}
};
class Bullet : public Entity {
public:
	std::wstring sprite[3];
public:
	Bullet() {
		sprite[0] = L"   ";
		sprite[1] = L" o ";
		sprite[2] = L"   ";

		width = sprite[0].size();
		height = 3;

		yPos = 0;
		xPos = 0;
		exists = true;
		speed = 200;
	}
	~Bullet() {
		exists = false;
	}
};
class ParticleSystem {
public:
	int speed = 1;
	int maxParticles = 12;
	int healthitterations = 250;

	std::wstring sprite[12];
	std::wstring chars = L"£$%&*(){}@[]<>#"; //15
	bool exists;

	int position[12][2];
    float direction[12][2]; // {x,y} (6x for each particle)
private:
	int directions_taken[12] = {NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

public:
	ParticleSystem() {
		std::random_device seeder;
		std::mt19937 engine(seeder());
		for (signed int i = 0; i < maxParticles; i++)
		{
			//sprites
			std::uniform_int_distribution<int> chars_dist(0, 14);
			std::uniform_int_distribution<int> direction_dist(0,360);
			int rand_angle = direction_dist(engine);
			int rand_sptr = chars_dist(engine);

			sprite[i] = chars[rand_sptr];

			// direction
			direction[i][0] = cos(RadToDeg(rand_angle));
			direction[i][1] = sin(RadToDeg(rand_angle));
			Add_to_Array(directions_taken, rand_angle, 12);
		}

		exists = true;
	}
	~ParticleSystem() {
		exists = false;
	}
private:
	double RadToDeg(double rad)
	{
		return rad * 3.1415926535 / 180;
	}
	void Add_to_Array(int in_array[], int value, int array_length) {
		bool allocated = false;
		for (signed int i = 0; i < array_length; i++)
		{
			if (in_array[i] == NULL && allocated == false) {
				in_array[i] = value;
				allocated = true;
			}
		}
	}
};
class Player : public Entity {
public:

	int health = 5;
	int fullHealth = 5;
	std::wstring sprite[5];

	int cannons = 1;
	bool canShoot = true;
	int coolDownItterationTime = 80;
public:
	Player() {
		sprite[0] = L"   <===>    ";
		sprite[1] = L"    [+]     ";
		sprite[2] = L"<>========<>";
		sprite[3] = L"    [+]     ";
		sprite[4] = L"   <===>    ";

		yPos = 0;
		xPos = 0;
		width = sprite[0].size();
		height = 5;
		exists = true;
		speed = 200;
	}
	~Player() {
		exists = false;
	}
};
class MediPack : public Entity {
public:
		std::wstring sprite[3];
public:
	MediPack() {
		sprite[0] = L"   ";
		sprite[1] = L"[+]";
		sprite[2] = L"   ";

		width = sprite[0].size();
		height = 3;

		yPos = 0;
		xPos = 0;
		exists = true;
		speed = 5;
	}
	~MediPack() {
		exists = false;
	}
};
class PowerUp : public Entity {
public:
	std::wstring sprite[3];
public:
	PowerUp() {
		sprite[0] = L"   ";
		sprite[1] = L"[$]";
		sprite[2] = L"   ";

		width = sprite[0].size();
		height = 3;

		yPos = 0;
		xPos = 0;
		exists = true;
		speed = 5;
	}
	~PowerUp() {
		exists = false;
	}
};


class Game : public GameEngine {
public:
	int itteration = 0;
	int score = 0;

	int oldwave = 1;
	int wave = 1;
	int waveTextTimer = 0;
	int waveTextSeconds = 2;

	bool collision = false;
	int maxBullets = 30;
	int maxEnemyBullets = 20;
	int maxcuiserEnemies = 3;
    int maxEnemies = 10;
	int maxparticles = 10;

	bool alive = false;

	Enemy* EnemiesOnScreen[10];	
	CruiserEnemy* CruiserEnemysOnScreen[3];

	Bullet* BulletsOnScreen[30];
	EnemyBullet* EnemyBulletsOnScreen[20];

	Player* player;
	MediPack* medipack = nullptr;
	PowerUp* powerup = nullptr;

	ParticleSystem* particlesystems[10];

	bool highscore_written_on_death = false;
	std::string filename = "HIGHSCORES.txt";
	int highScore;

	virtual bool OnUserCreate() {
		highScore = GetHighScore(filename);
		//memset(player, 0, sizeof(player));
		//memset(EnemiesOnScreen, 0, sizeof(Enemy) * maxEnemies);
		//memset(BulletsOnScreen, 0, sizeof(Bullet) * maxBullets);

		srand(time(NULL));
		for (signed int i = 0; i < maxEnemies; i++)
		{
			Enemy *newEnemy = new Enemy;
			newEnemy->xPos = ScreenWidth() - 10;
			newEnemy->yPos = rand() % ((ScreenHeight()-5) - (10) + 1) + (10); //int randNum = rand()%(max-min + 1) + min;
			newEnemy->speed = rand() % ((20) - (1) + 1) + (1);
			EnemiesOnScreen[i] = newEnemy;
		}

		//initalise player
		player = new Player;
		player->xPos = 3;
		player->yPos = ScreenHeight() / 2;

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime) {

		Fill(0,0, ScreenWidth(),  ScreenHeight(), L' ');

		wave = round((int)score / 15) + 1;
		
		if (oldwave != wave) {
			if(maxEnemies > 10)
				maxEnemies++;
			oldwave = wave;

			waveTextTimer = (int)round(1 / fElapsedTime * waveTextSeconds);
		}

		if (maxEnemies > 10)
			maxEnemies = 10;
		

		if (alive == true) {
			// Check input

			if (player == nullptr) {
				alive = false;
			}

			//KEY INPUT
			if (player != nullptr) {
				if (keys[VK_LEFT].bHeld)
					player->xPos -= (int)(player->speed * fElapsedTime);
				if (keys[VK_RIGHT].bHeld)
					player->xPos += (int)(player->speed * fElapsedTime);
				if (keys[VK_UP].bHeld)
					player->yPos -= (int)(player->speed * fElapsedTime);
				if (keys[VK_DOWN].bHeld)
					player->yPos += (int)(player->speed * fElapsedTime);
				if (keys[VK_SPACE].bHeld) {
					//spawn bullet

					int toAllocate = player->cannons;

					for (signed int i = 0; i < maxBullets; i++)
					{
						if (BulletsOnScreen[i] == nullptr) {
							if (player->canShoot) {
								if (toAllocate != 0) {
									Bullet* newBullet = new Bullet;

									if (player->cannons == 1) {
										newBullet->yPos = (int)(player->yPos + ( player->height) / 2); //center
										toAllocate = 0;
									}

									if (player->cannons == 2) {

										if (toAllocate == 2) 
											newBullet->yPos = (int)(player->yPos); //top
										else if (toAllocate == 1) 
											newBullet->yPos = (int)(player->yPos + player->height); //bottom
										toAllocate--;
									}

									if (player->cannons == 3) {
									    if (toAllocate == 3)
											newBullet->yPos = (int)(player->yPos + (player->height) / 2); //center	
										else if (toAllocate == 2) 
											newBullet->yPos = (int)(player->yPos); //top
										else if (toAllocate == 1)
											newBullet->yPos = (int)(player->yPos + player->height); //bottom	
										toAllocate--;
									}

									newBullet->xPos = (int)(player->xPos + player->width);
									BulletsOnScreen[i] = newBullet;
								}
							}
						}
					}
					if(player->canShoot)
						player->canShoot = false;
				}
			}


			//can shoot cooldown
			if (player != nullptr) {
				if (fmod(itteration, player->coolDownItterationTime) == 0) {
					player->canShoot = true;
				}
			}
			//check player's health
			if (player != nullptr) {
				if (player->health <= 0) {
					player->exists = false;
					player->health = 0;
					player->canShoot = false;
					player = nullptr;
					delete player;
				}
			}

			//check all collision

			//enemies and player
			for (signed int i = 0; i < maxEnemies; i++){
				if(cfn::OnCollision<Enemy, Player>(EnemiesOnScreen[i], player)){
					player->health -= 1;
					EnemiesOnScreen[i]->health -= 1;
				}
			}
			//player and enemy bullets
			for (signed int i = 0; i < maxEnemyBullets; i++)
			{
				if (cfn::OnCollision(EnemyBulletsOnScreen[i], player)) 
				{
						EnemyBulletsOnScreen[i]->exists = false;
						EnemyBulletsOnScreen[i] = nullptr;
						delete EnemyBulletsOnScreen[i];
						player->health-=1;
				}
			}
			//player and enemy cruisers
			for (signed int i = 0; i < maxcuiserEnemies; i++)
			{
				if (cfn::OnCollision(CruiserEnemysOnScreen[i], player)) {
					collision = true;
					player->health-=1;
					CruiserEnemysOnScreen[i]->health -= 1;
				}
			}
			//Bullets and enemies
			for (signed int i = 0; i < maxEnemies; i++)
			{
				for (signed int j = 0; j < maxBullets; j++)
				{
					if (cfn::OnCollision(EnemiesOnScreen[i], BulletsOnScreen[j]))
					{
						collision = true;
						BulletsOnScreen[j]->exists = false;
						BulletsOnScreen[j] = nullptr;
     					delete BulletsOnScreen[j];
						EnemiesOnScreen[i]->health -= 1;
						score++;
					}
				}
			}
			//Bullets and cruisers
			for (signed int i = 0; i < maxcuiserEnemies; i++)
			{
				for (signed int j = 0; j < maxBullets; j++)
				{
					if (cfn::OnCollision(CruiserEnemysOnScreen[i], BulletsOnScreen[j]))
					{
						collision = true;
						BulletsOnScreen[j]->exists = false;
						BulletsOnScreen[j] = nullptr;
	    				delete BulletsOnScreen[j];
						CruiserEnemysOnScreen[i]->health -= 1;
						score+=3;
							
					}
				}
				
			}
			// medipack and player
			if (cfn::OnCollision(medipack, player)) {
					player->health += rand() % (3 - 1 + 1) + 1;
					if (player->health > player->fullHealth) 
						player->health = player->fullHealth;
					medipack->exists = false;
					medipack = nullptr;
					delete medipack;
				
			}
			//Powerup and player
			if (cfn::OnCollision(powerup, player)) {
				player->cannons++;
				if (player->cannons > 3)
					player->cannons = 3;
				powerup->exists = false;
				powerup = nullptr;
				delete powerup;
				
			}
			

			// Check Bounds player
			if (player != nullptr) {
				if (player->xPos > ScreenWidth() - 15) {
					player->xPos = ScreenWidth() - 15;
				}
				if (player->xPos < 2) {
					player->xPos = 2;
				}
				if (player->yPos > ScreenHeight() - 7) {
					player->yPos = ScreenHeight() - 7;
				}
				if (player->yPos < 2) {
					player->yPos = 2;
				}
				if(player != nullptr)
					DrawTextSprite(player->xPos, player->yPos, player->sprite, player->height, FG_BLUE);
			}
			//chech medipack bounds
			if (medipack != nullptr) {
				if (medipack->xPos < 2) {
					medipack->exists = false;
					medipack = nullptr;
					delete medipack;
				}
			}
			//check powerup bounds
			if (powerup != nullptr) {
				if (powerup->xPos > ScreenWidth()-2) {
					powerup->exists = false;
					powerup = nullptr;
					delete powerup;
				}
			}

			// draw bullets
			for (signed int i = 0; i < maxBullets; i++)
			{
				if (BulletsOnScreen[i] != nullptr) {
					if (BulletsOnScreen[i]->xPos > ScreenWidth() - 5) {
						BulletsOnScreen[i]->exists = false;
						BulletsOnScreen[i] = nullptr;
						delete BulletsOnScreen[i];
					}
					if (BulletsOnScreen[i] != nullptr)
						//if(fmod(itteration, 10)) // only update the speed once in a while to slow down the sprites
						BulletsOnScreen[i]->xPos += (int)(BulletsOnScreen[i]->speed * fElapsedTime);
					if (BulletsOnScreen[i] != nullptr)
						DrawTextSprite(BulletsOnScreen[i]->xPos, BulletsOnScreen[i]->yPos, BulletsOnScreen[i]->sprite, BulletsOnScreen[i]->height, FG_RED);
				}
			}

			//draw enemy bullets
			for (signed int i = 0; i < maxEnemyBullets; i++)
			{
				if (EnemyBulletsOnScreen[i] != nullptr) {
					if (EnemyBulletsOnScreen[i]->xPos < 5) {
						EnemyBulletsOnScreen[i]->exists = false;
						EnemyBulletsOnScreen[i] = nullptr;
						delete EnemyBulletsOnScreen[i];
					}
					if (EnemyBulletsOnScreen[i] != nullptr)
						EnemyBulletsOnScreen[i]->xPos -= (int)(EnemyBulletsOnScreen[i]->speed * fElapsedTime);
					if (EnemyBulletsOnScreen[i] != nullptr)
						DrawTextSprite(EnemyBulletsOnScreen[i]->xPos, EnemyBulletsOnScreen[i]->yPos, EnemyBulletsOnScreen[i]->sprite, EnemyBulletsOnScreen[i]->height, FG_GREEN);
				}
			}

			// CHECK ENEMY'S HEALTH
			for (int i = 0; i < maxEnemies; i++)
			{
				if (EnemiesOnScreen[i] != nullptr) {
					if (EnemiesOnScreen[i]->health <= 0) {

						int emenyX = EnemiesOnScreen[i]->xPos;
						int enemyY = EnemiesOnScreen[i]->yPos;

						EnemiesOnScreen[i]->exists = false;
						EnemiesOnScreen[i]->health = 0;
						EnemiesOnScreen[i] = nullptr;
						delete EnemiesOnScreen[i];


						//INSTANSIATE PARTICLE SYSTEM "ParticleSystem"
						bool allocated = false;
						for (signed int i = 0; i < maxparticles; i++)
						{
							if (allocated == false) {
								if (particlesystems[i] == nullptr) {
									ParticleSystem* newsystem = new ParticleSystem;
									for (signed int i = 0; i < newsystem->maxParticles; i++)
									{
										newsystem->position[i][0] = emenyX;
										newsystem->position[i][1] = enemyY;
									}
									particlesystems[i] = newsystem;
									allocated = true;
								}
							}
						}
					}
				}
			}


			//CHECK CRUISER ENEMIES HEALTH
			for (int i = 0; i < maxcuiserEnemies; i++)
			{
				if (CruiserEnemysOnScreen[i] != nullptr) {
					if (CruiserEnemysOnScreen[i]->health <= 0) {

						int emenyX = CruiserEnemysOnScreen[i]->xPos;
						int enemyY = CruiserEnemysOnScreen[i]->yPos;

						CruiserEnemysOnScreen[i]->exists = false;
						CruiserEnemysOnScreen[i]->health = 0;
						CruiserEnemysOnScreen[i] = nullptr;
						delete CruiserEnemysOnScreen[i];


						//INSTANSIATE PARTICLE SYSTEM "ParticleSystem"
						bool allocated = false;
						for (signed int i = 0; i < maxparticles; i++)
						{
							if (allocated == false) {
								if (particlesystems[i] == nullptr) {
									ParticleSystem* newsystem = new ParticleSystem;
									for (signed int i = 0; i < newsystem->maxParticles; i++)
									{
										newsystem->position[i][0] = emenyX;
										newsystem->position[i][1] = enemyY;
									}
									particlesystems[i] = newsystem;
									allocated = true;
								}
							}
						}
					}
				}
			}

			//move each particle in each system
			for (signed int i = 0; i < maxparticles; i++)
			{
				if (particlesystems[i] != nullptr)
				{
					for (signed int j = 0; j < particlesystems[i]->maxParticles; j++) //loop through each individual particle
					{
						if (fmod(itteration,75) == 0) {
							particlesystems[i]->position[j][0] -= particlesystems[i]->direction[j][0] * particlesystems[i]->speed * fElapsedTime;
							particlesystems[i]->position[j][1] -= particlesystems[i]->direction[j][1] * particlesystems[i]->speed * fElapsedTime;
						}
						DrawTextSprite(particlesystems[i]->position[j][0], particlesystems[i]->position[j][1], &particlesystems[i]->sprite[j] ,1, FG_YELLOW);
					}
				}
			}

			//check every systems life
			for (signed int i = 0; i < maxparticles; i++)
			{
				if (particlesystems[i] != nullptr) {
					particlesystems[i]->healthitterations--;

					if (particlesystems[i]->healthitterations <= 0) {
						particlesystems[i]->exists = false;
						particlesystems[i] = nullptr;
						delete particlesystems[i];
					}
				}
			}


			//check if enemys are less than max if so respawn
			for (signed i = 0; i < maxEnemies; i++)
			{
				if (EnemiesOnScreen[i] == nullptr) {
					Enemy* newEnemy = new Enemy;
					newEnemy->xPos = ScreenWidth() - 10;
					newEnemy->yPos = rand() % ((ScreenHeight() - 5) - (10) + 1) + (10); //int randNum = rand()%(max-min + 1) + min;
					newEnemy->speed = rand() % ((50) - (1) + 1) + (1);

					EnemiesOnScreen[i] = newEnemy;
				}
			}


			//check if medipack does not exist...if so spawn one
			if (fmod(itteration, 2000) == 0) {
				if (medipack == nullptr) {
					MediPack* newmedipack = new MediPack;
					newmedipack->xPos = ScreenWidth() - 10;
					newmedipack->yPos = rand() % ((ScreenHeight() - 5) - (10) + 1) + (10); //int randNum = rand()%(max-min + 1) + min;
					medipack = newmedipack;
				}
			}

			//draw medipack
			if (medipack != nullptr) {
				if (fmod(itteration, 15) == 0)
					medipack->xPos -= medipack->speed;
				DrawTextSprite(medipack->xPos, medipack->yPos, medipack->sprite, medipack->height);

			}


			//check if powerup does not exist...if so spawn one
			if (fmod(itteration, 7500) == 0) {
				if (powerup == nullptr) {
					PowerUp* newpowerup = new PowerUp;
					newpowerup->xPos = 2;
					newpowerup->yPos = rand() % ((ScreenHeight() - 5) - (10) + 1) + (10); //int randNum = rand()%(max-min + 1) + min;
					powerup = newpowerup;
				}
			}

			//draw powerup
			if (powerup != nullptr) {
				if (fmod(itteration, 15) == 0)
					powerup->xPos += powerup->speed;
				DrawTextSprite(powerup->xPos, powerup->yPos, powerup->sprite, powerup->height);

			}

			//draw enemies
			for (signed i = 0; i < maxEnemies; i++)
			{
				if (EnemiesOnScreen[i] != nullptr) {
					if (EnemiesOnScreen[i]->xPos < 2) {
						EnemiesOnScreen[i]->exists = false;
						delete EnemiesOnScreen[i];
						EnemiesOnScreen[i] = nullptr;

						//Create new enemy
						bool allocated = false;
						for (signed i = 0; i < maxEnemies; i++)
						{
							if (EnemiesOnScreen[i] == nullptr && allocated == false) {
								Enemy* newEnemy = new Enemy;
								newEnemy->xPos = ScreenWidth() - 10;
								newEnemy->yPos = rand() % ((ScreenHeight() - 5) - (10) + 1) + (10); //int randNum = rand()%(max-min + 1) + min;
								newEnemy->speed = rand() % ((50) - (1) + 1) + (1);

								EnemiesOnScreen[i] = newEnemy;
								allocated = true;
							}
						}
					}
				}
				if (EnemiesOnScreen[i] != nullptr) {
					if (EnemiesOnScreen[i]->exists == true) {
						if (fmod(itteration, 5) == 0)  // only update the speed once in a while to slow down the sprites
							EnemiesOnScreen[i]->xPos -= EnemiesOnScreen[i]->speed * fElapsedTime;
						if(EnemiesOnScreen[i] != nullptr)
							DrawTextSprite(EnemiesOnScreen[i]->xPos, EnemiesOnScreen[i]->yPos, EnemiesOnScreen[i]->sprite, EnemiesOnScreen[i]->height);
					}
				}
			}


			//draw cruiser enemies
			for (signed i = 0; i < maxcuiserEnemies; i++)
			{
				if (CruiserEnemysOnScreen[i] != nullptr) {
					if (CruiserEnemysOnScreen[i]->xPos < 2) {
						CruiserEnemysOnScreen[i]->exists = false;
						delete CruiserEnemysOnScreen[i];
						CruiserEnemysOnScreen[i] = nullptr;
					}
				}
				if (CruiserEnemysOnScreen[i] != nullptr) {
					if (CruiserEnemysOnScreen[i]->exists == true) {
						if (fmod(itteration, 10) == 0)  // only update the speed once in a while to slow down the sprites
							CruiserEnemysOnScreen[i]->xPos -= CruiserEnemysOnScreen[i]->speed * fElapsedTime;
						if (CruiserEnemysOnScreen[i] != nullptr)
							DrawTextSprite(CruiserEnemysOnScreen[i]->xPos, CruiserEnemysOnScreen[i]->yPos, CruiserEnemysOnScreen[i]->sprite, CruiserEnemysOnScreen[i]->height);
						if (fmod(itteration, 200) == 0) {
							CruiserEnemysOnScreen[i]->canFire = true;
						}	
					}
				}
			}

			//spawn Enemy bullet
			for (signed int i = 0; i < maxcuiserEnemies; i++)
			{
				if (CruiserEnemysOnScreen[i] != nullptr) {
					if (CruiserEnemysOnScreen[i]->canFire == true) {
						bool allocated = false;
						for (signed int j = 0; j < maxEnemyBullets; j++)
						{
							if (EnemyBulletsOnScreen[j] == nullptr && allocated == false) {
								EnemyBullet* newBullet = new EnemyBullet;
								newBullet->xPos = CruiserEnemysOnScreen[i]->xPos;
								newBullet->yPos = CruiserEnemysOnScreen[i]->yPos;

								EnemyBulletsOnScreen[j] = newBullet;
								allocated = true;
							}
						}
						CruiserEnemysOnScreen[i]->canFire = false;
					}
				}
			}

			//spawn cruiser enemy
			if (fmod(itteration, 750) == 0) {
				bool allocated = false;
				for (signed i = 0; i < maxcuiserEnemies; i++)
				{
					if (CruiserEnemysOnScreen[i] == nullptr && allocated == false) {
						CruiserEnemy* newEnemy = new CruiserEnemy;
						newEnemy->xPos = ScreenWidth() - (newEnemy->width + 1);
						newEnemy->yPos = rand() % ((ScreenHeight() - 5) - (10) + 1) + (10); //int randNum = rand()%(max-min + 1) + min;
						newEnemy->speed = rand() % ((50) - (1) + 1) + (1);

						CruiserEnemysOnScreen[i] = newEnemy;
						allocated = true;
					}
				}
			}

			//=========== UI ============= 
			if (player != nullptr) {
				std::wstring _string;
				_string = L" SCORE: " + std::to_wstring((int)score) + L" Wave: " + std::to_wstring((int)wave) + L" Highscore: " + std::to_wstring((int)highScore);

				if (player != nullptr) {
					_string += L" Health: |";

					int temp = 0;
					for (signed i = 0; i < player->health; i++) {
						temp++;
						_string += L"\u2588";
					}
					for (signed i = 0; i < player->fullHealth - temp; i++)
						_string += L" ";
					_string += L"|";

				}
				else { _string += L"Health: DEAD"; }
				DrawString(0, 0, _string);
			}
			else {
				alive = false;
			}

			//highscore
			if (score > highScore)
				highScore = score;

			if (fmod(itteration, 1000) == 0)
				writeHighScore(filename, highScore);


			// Draw Wave Text 
			if (waveTextTimer > 0) {
				waveTextTimer -= 1;
				std::wstring _string = L"###  WAVE "+ std::to_wstring(wave) + L"  INCOMMING  ###";
				DrawString((ScreenWidth() / 2 - _string.size() / 2), ScreenHeight() / 2, _string);
			}
		}
		else {
			//START
			if (itteration == 0) {
				std::wstring _string = L"### SPACE SHOOTER ###";
				std::wstring _stringrestart = L" Press ENTER Key To Start";
				DrawString((ScreenWidth() / 2 - _string.size() / 2), ScreenHeight() / 2 - 2, _string);
				DrawString((ScreenWidth() / 2 - _stringrestart.size() / 2), ScreenHeight() / 2 + 2, _stringrestart);
			}
			// RESPAWN
			else{
				//HIGHSCORE
				if (highscore_written_on_death == false) {
					writeHighScore(filename, highScore);
					highscore_written_on_death = true;
				}
				std::wstring _string = L"### YOU DIED ###";
				std::wstring _stringinfo = L"score: " + std::to_wstring(score) + L" Wave: " + std::to_wstring(wave) + L" Highscore: " + std::to_wstring((int)highScore);
				std::wstring _stringrestart = L" Press ENTER Key To Restart";
				DrawString((ScreenWidth() / 2 - _string.size() / 2), ScreenHeight() / 2 - 3, _string);
				DrawString((ScreenWidth() / 2 - _stringinfo.size() / 2), ScreenHeight() / 2 - 2, _stringinfo);
				DrawString((ScreenWidth() / 2 - _stringrestart.size() / 2), ScreenHeight() / 2 + 2, _stringrestart);
			}

			//ON CLICK ENTER
			if (keys[VK_RETURN].bHeld) {
				alive = true;
				wave = 1;
				score = 0;
				maxEnemies = 3;

				for (signed int i = 0; i < maxEnemies; i++)
					EnemiesOnScreen[i] = nullptr;
				for (signed int i = 0; i < maxBullets; i++)
					BulletsOnScreen[i] = nullptr;
				for (signed int i = 0; i < maxparticles; i++)
					particlesystems[i] = nullptr;
				for (signed int i = 0; i < maxcuiserEnemies; i++)
					CruiserEnemysOnScreen[i] = nullptr;

				writeHighScore(filename,highScore);
				highScore = GetHighScore(filename);

				medipack = nullptr;
				powerup = nullptr;
				player = new Player;

				itteration = 0;
			}
			
		}
		if (alive == true) { itteration++; }

		return true;
	}

	int GetHighScore(std::string _filename) {
		int hs = 0;
		ifstream f(_filename); //taking file as inputstream
		std::string str;
		if (f) {
			ostringstream ss;
			ss << f.rdbuf(); // reading data
			str = ss.str();
			std::istringstream(str) >> hs;
			f.close();
		}
		return hs;
	}
	void writeHighScore(std::string _filename, int hs) {
		ofstream f;
		f.open(_filename);
		f << std::to_string(hs);
		f.close();
	}
};


//==============================
int main() { 
	Game game; 
	game.ConstructConsole(120, 40,14,8, false, (std::wstring)L"Space Shooter");
	game.Start();
}
