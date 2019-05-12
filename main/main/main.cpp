// Screen.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <time.h>

using namespace std;

//구현한 내용들을 적었습니다. 11번, 15번을 제외하고 모두 구현하였고, 14번은 플레이어가 죽엇을때 살아있던 시간만 표시하였습니다.

//1. 플레이어는 방향성을 가진다.오른쪽으로 이동하고자 한다면 머리방향이 먼저 오른쪽으로 방향전환한 후 이동한다.머리 방향이 전환되었다는 것을 확인하기 위해 머리모양에 해당하는 그림을 표시하도록 한다.
//2. 플레이어는 공격시에 현재 방향으로만 공격가능하다.
//3. 플레이어의 공격 무기로는(1) 총, (2) 레이저건 이 있다.총은 단발의 데미지량은 1인 반면 레이저건의 데미지량은 2초간 지속적으로 데미지를 준다.레어저건의 초당 데미지량은 1에 해당한다.레이저건의 경우, 타켓의 위치와 플레이어의 위치를 일직선으로 이어서 공격을 지속적으로 받고 있다는 것을 알려준다.
//4. 총 공격의 경우, 총 10발의 총알이 있으며 연사 가능하며 비어 있을 경우, 1초당 한 발씩 총알이 자동 추가된다.단, 총알이 없어야 자동 장전이 가능하다. 10초간 기다리면 모두 장전할 수 있으나 기다리지 못하고 총알을 쏘게되면 추가로 1초당 한 발씩 총알이 자동 추가되지 않는다.
//5. 레이저건 공격의 경우, 2초간 공격후, 3초간 cooltime이 적용된다.
//6. 공격시 플레이어 공격의 반대방향에 현재 공격의 상태 정보(총 공격의 경우, 남은 총알 수, 10발 재장전까지 남은 시간 등; 레이저건 공격의 경우, 남은 공격 시간, 쿨 타임시 남은 시간 등)를 보여준다.
//7. 공격 전환의 경우, ‘m’키를 이용한다.
//8. 공격 상태 정보는 공격시에만 1초간 화면에 보인다.
//9. 적은 매 10초마다 임의의 위치에서 나타나며 적의 위치를 기준으로 5m 이내 플레이어는 독 공격을 받는다.
//10. 적의 독공격은 매초 2만큼 데미지를 주며 3초간 지속된다.지속 공격시 플레이어의 hp값이 줄어드는 것을 화면에 보이도록 한다.적은 3초 지속 공격후, 2초간 cooltime을 갖는다.
//11. 화면상에 보일 수 있는 최대 적의 개수는 5마리이다. (동일 위치에 적이 겹쳐 있으면 보이지 않을 수 있다)
//12. 적은 2초당 1m만큼의 속도로 플레이어를 향해 이동한다.
//13. 적, 플레이어는 모두 hp를 가지며 적은 5, 플레이어는 10만큼 갖는다.
//14. 플레이어가 죽은 시점에 게임은 종료되며 플레이어가 살아 있었던 시간과 죽인 적들의 개수를 화면에 보여준 후 게임을 종료한다.
//15. 플레이어의 공격시 공격방향으로 여러 적들이 있으면 가장 가까운 적부터 공격한다.


void draw(char* loc, const char* face)
{
	strncpy(loc, face, strlen(face));
}

class Screen {
	int size;
	char* screen;

public:
	Screen(int sz) : size(sz), screen(new char[sz + 1]) {}
	~Screen() { delete[] screen; }

	void draw(int pos, const char* face)
	{
		if (face == nullptr) return;
		if (pos < 0 || pos >= size) return;
		strncpy(&screen[pos], face, strlen(face));
	}

	void render()
	{
		printf("%s\r", screen);
	}

	void clear()
	{
		memset(screen, ' ', size);
		screen[size] = '\0';
	}

	int length()
	{
		return size;
	}

};

class GameObject {
	int pos;
	char face[20];
	int hp;
	Screen* screen;

public:
	GameObject(int pos, const char* face, Screen* screen)
		: pos(pos), screen(screen)
	{
		strcpy(this->face, face);
	}

	GameObject(int pos, const char* face, int hp, Screen* screen)
		: pos(pos), screen(screen), hp(hp)
	{
		strcpy(this->face, face);
	}

	GameObject()
	{
	}

	int getHp()
	{
		return hp;
	}

	void decreaseHp()
	{
		hp--;
	}

	char* getFace()
	{
		return face;
	}

	void setFace(const char *face)
	{
		strcpy(this->face, face);
	}

	void setFaceInfo1(char *info1, char* info2)
	{
		strcat(this->face, info1);
		strcat(this->face, info2);
	}

	void setFaceInfo2(char *info1, char* info2)
	{
		strcat(this->face, info1);
	}

	int getPosition()
	{
		return pos;
	}
	void setPosition(int pos)
	{
		this->pos = pos;
	}
	void draw()
	{
		screen->draw(pos, face);
	}
};

class Player : public GameObject {
	int prepos;
	bool fireDirection;
	bool poisonTime;
	bool poisonCoolTime;

	int poisonCount;
	int poisonCoolTimeCount;
	int infoTime;

	bool alive;
public:
	Player(int pos, const char* face, int hp, Screen* screen)
		: GameObject(pos, face, hp, screen)
	{
		prepos = pos;
		fireDirection = true;
		poisonTime = false;
		poisonCount = 0;
		poisonCoolTime = false;
		poisonCoolTimeCount = 0;
		infoTime = 0;

		alive = true;
	}

	void moveLeft()
	{
		prepos = getPosition();
		setPosition(getPosition() - 1);
		fireDirection = false;

		if (getPosition() <= 0)
			setPosition(79);
	}

	void moveRight()
	{
		prepos = getPosition();
		setPosition(getPosition() + 1);
		fireDirection = true;

		if (getPosition() > 80)
			setPosition(1);
	}

	void seeInfoTime()
	{
		infoTime = 15;
	}

	bool getfireDirection()
	{
		return fireDirection;
	}

	bool getAlive()
	{
		return alive;
	}

	void update(int enemy_pos, int bulletNumber, int reroader, int razerTime, int coolTime, bool gunMode)
	{
		char faceinfo[20] = { NULL };
		char tmp[20] = { NULL };

		infoTime--;

		if (prepos > getPosition())
		{
			setFace("--(^_^)");
			
			if (infoTime > 0)
			{
				if (gunMode)
				{
					strcpy(faceinfo, getFace());
					itoa(bulletNumber, tmp, 10);
					strcat(faceinfo, tmp);
					strcat(faceinfo, ", ");
					itoa(reroader, tmp, 10);
					strcat(faceinfo, tmp);
					setFace(faceinfo);
				}
				else if (!gunMode)
				{
					strcpy(faceinfo, getFace());
					itoa(razerTime, tmp, 10);
					strcat(faceinfo, tmp);
					strcat(faceinfo, ", ");
					itoa(coolTime, tmp, 10);
					strcat(faceinfo, tmp);
					setFace(faceinfo);
				}
			}

			if (poisonTime) 
			{
				setFace("--(>_<)");

				if (infoTime > 0)
				{
					if (gunMode)
					{
						strcpy(faceinfo, getFace());
						itoa(bulletNumber, tmp, 10);
						strcat(faceinfo, tmp);
						strcat(faceinfo, ", ");
						itoa(reroader, tmp, 10);
						strcat(faceinfo, tmp);
						setFace(faceinfo);
					}
					else
					{
						strcpy(faceinfo, getFace());
						itoa(razerTime, tmp, 10);
						strcat(faceinfo, tmp);
						strcat(faceinfo, ", ");
						itoa(coolTime, tmp, 10);
						strcat(faceinfo, tmp);
						setFace(faceinfo);
					}
				}
			}
		}
		else if (prepos <= getPosition())
		{
			setFace("(^_^)--");

			if (infoTime > 0)
			{
				if (gunMode)
				{
					itoa(bulletNumber, tmp, 10);
					strcpy(faceinfo, tmp);
					strcat(faceinfo, ", ");
					itoa(reroader, tmp, 10);
					strcat(faceinfo, tmp);
					strcat(faceinfo, getFace());
					setFace(faceinfo);
				}
				
				else if (!gunMode)
				{
					itoa(razerTime, tmp, 10);
					strcpy(faceinfo, tmp);
					strcat(faceinfo, ", ");
					itoa(coolTime, tmp, 10);
					strcat(faceinfo, tmp);
					strcat(faceinfo, getFace());
					setFace(faceinfo);
				}
				
			}

			if (poisonTime)
			{
				setFace("(>_<)--");

				if (infoTime > 0)
				{
					if (gunMode)
					{
						itoa(bulletNumber, tmp, 10);
						strcpy(faceinfo, tmp);
						strcat(faceinfo, ", ");
						itoa(reroader, tmp, 10);
						strcat(faceinfo, tmp);
						strcat(faceinfo, getFace());
						setFace(faceinfo);
					}
					else if (!gunMode)
					{
						itoa(razerTime, tmp, 10);
						strcpy(faceinfo, tmp);
						strcat(faceinfo, ", ");
						itoa(coolTime, tmp, 10);
						strcat(faceinfo, tmp);
						strcat(faceinfo, getFace());
						setFace(faceinfo);
					}
				}
			}
		}

		if (enemy_pos <= getPosition() + 11 && getPosition() <= enemy_pos + 10 && poisonCoolTime == false)
		{
			poisonTime = true;
			poisonCoolTime = true;
		}

		if (poisonTime)
		{
			poisonCount++;


			if (poisonCount % 15 == 0)
			{
				decreaseHp();
				decreaseHp();
			}

			if (poisonCount >= 45)
			{
				poisonCount = 0;
				poisonTime = false;
			}


		}

		if (poisonCoolTime)
		{
			poisonCoolTimeCount++;

			if (poisonCoolTimeCount >= 75)
			{
				poisonCoolTimeCount = 0;
				poisonCoolTime = false;
			}
		}

		if (getHp() <= 0)
		{
			alive = false;
		}
	}

};

class Enemy : public GameObject {
	int razerTime;
	int warpcount;
	int approchcount;

public:
	Enemy(int pos, const char* face, int hp, Screen* screen)
		: GameObject(pos, face, hp, screen)
	{
		razerTime = 0;
		warpcount = 0;
		approchcount = 0;
	}

	void enemyMove(int player_pos)
	{
		warpcount++;
		if (warpcount >= 150)
		{
			setPosition(rand() % 80);

			if (getPosition() > 80)
				setPosition(1);
			if (getPosition() <= 0)
				setPosition(79);
			warpcount = 0;
		}

		approchcount++;
		if (approchcount >= 30)
		{
			if (player_pos > getPosition())
				setPosition(getPosition() + 1);
			else
				setPosition(getPosition() - 1);
			approchcount = 0;
		}
	}

	void update(int bullet_pos, int player_pos, bool razerhitcheck)
	{

		if (getPosition() == bullet_pos || getPosition() + 5 == bullet_pos)
			decreaseHp();

		if (razerhitcheck)
		{
			razerTime++;
			if (razerTime >= 15)
			{
				decreaseHp();
				razerTime = 0;
			}
		}

		if (getHp() <= 0)
		{
			setPosition(-5);
		}
	}
};

class Bullet : public GameObject {
protected:
	bool isFiring;
	bool directioncheck;
	bool firedirection;
	bool gunMode;
	bool reroader;

	int bulletNumber;
	int reroaderTime;
	int poschange;
public:
	Bullet(int pos, const char* face, Screen* screen)
		: GameObject(pos, face, screen), isFiring(false)
	{
		directioncheck = false;
		firedirection = true;
		gunMode = true;
		reroader = false;
		bulletNumber = 0;
		reroaderTime = 0;
		poschange = 0;
	}

	Bullet()
		: isFiring(false)
	{
		setPosition(-1);
		setFace("+");
		directioncheck = false;
		firedirection = true;
		gunMode = true;
		bulletNumber = 0;
		reroaderTime = 0;
	}
	void gunModeChange()
	{
		gunMode = !gunMode;
	}


	void moveLeft()
	{
		setPosition(getPosition() - 1);
	}
	void moveRight()
	{
		setPosition(getPosition() + 1);
	}

	void draw()
	{
		if (isFiring == false) return;
		GameObject::draw();
	}

	int getreroaderTime() {
		return reroaderTime;
	}

	void maxBullet()
	{
		if (bulletNumber == 10)
			isFiring = false;
	}

	void fire(int player_pos, int reroadercheck)
	{
		if (reroadercheck >= 10)
			return;
		isFiring = true;
		//setPosition(player_pos);
		poschange = player_pos;
		directioncheck = true;
		reroader = false;
	}

	void resetReroaderTime()
	{
		isFiring = true;
		reroaderTime = 0;
	}

	void increaseBulletNumber()
	{
		bulletNumber++;
		if (bulletNumber >= 10)
		{
			reroader = true;
		}
	}

	int getBulletNumber()
	{
		return bulletNumber;
	}
	bool getGunMode()
	{
		return gunMode;
	}

	void update(bool player_fireDirection, int enemy_pos, int reroadercheck)
	{
		maxBullet();

		if (isFiring == false)
		{
			if (reroadercheck == 10)
				reroader = true;
			if (reroader)
			{
				reroaderTime++;
				if (reroaderTime >= 15)
				{
					bulletNumber--;
					reroaderTime = 0;
				}
				if (bulletNumber == 0)
					reroader = false;

			}
			setPosition(-1);
			return;
		}
		if (directioncheck)
		{
			setPosition(poschange);
			if (player_fireDirection)
			{
				setPosition(getPosition() + 6);
				firedirection = true;
			}
			else
				firedirection = false;

			directioncheck = false;
		}

		int pos = getPosition();



		if (firedirection)
		{
			pos = pos + 1;
		}
		else
		{
			pos = pos - 1;
		}

		if (pos >= enemy_pos && pos <= enemy_pos + 5)
		{
			isFiring = false;
		}


		setPosition(pos);
	}
	/*
			if (pos < enemy_pos) {
				pos = pos + 1;
			}
			else if (pos > enemy_pos) {
				pos = pos - 1;
			}
			else {
				isFiring = false;
			}
			setPosition(pos);
	*/

};

class Razer :Bullet {
	bool isFiring;
	bool hitcheck;

	int razerTime;
	int razerCoolTime;


	int pos;
	char face[20];
	int hp;
	Screen* screen;
public:
	Razer(int pos, const char* face, Screen* screen)
		: pos(pos), screen(screen), isFiring(false)
	{
		strcpy(this->face, face);
		firedirection = true;
		razerTime = 0;
		razerCoolTime = 0;
	}

	void fire()
	{
		if (razerCoolTime > 0) return;
		isFiring = true;
		hitcheck = false;
	}

	bool getHitCheck()
	{
		return hitcheck;
	}

	int getRazerCoolTime()
	{
		return razerCoolTime;
	}

	int getRazerTime()
	{
		return razerTime;
	}

	void draw(int player_pos, int enemy_pos, const char* player_face, const char* enemy_face)
	{
		if (isFiring == false) return;

		if (firedirection) {
			pos = player_pos + strlen(player_face);

			for (int i = 0; i < screen->length(); i++)
			{
				screen->draw(pos + i, face);
				if (enemy_pos == pos + i || enemy_pos + 5 == pos + i)
				{
					hitcheck = true;
					return;
				}


				if (screen->length() == pos + i)
				{
					hitcheck = false;
					return;
				}
			}
			return;
		}
		else {
			pos = player_pos - 1;
			for (int i = 0; i < screen->length(); i++)
			{
				screen->draw(pos - i, face);

				if (enemy_pos == pos - i || enemy_pos + 5 == pos - i)
				{
					hitcheck = true;
					return;
				}

				if (0 == pos - i)
				{
					hitcheck = false;
					return;
				}
			}
			return;
		}
	}

	void update(bool player_fireDirection)
	{
		if (player_fireDirection)
			firedirection = true;
		else
			firedirection = false;

		if (isFiring)
		{
			razerTime++;
			if (razerTime > 30)
			{
				isFiring = false;
				razerTime = 0;
				razerCoolTime = 45;
				hitcheck = false;
			}
		}

		if (razerCoolTime > 0)
		{
			razerCoolTime--;
		}
	}
};


int main()
{
	Screen screen{ 90 };
	Player player = { 30, "(^_^)--", 10, &screen };
	Enemy enemy{ 50, "(*--*)", 5, &screen };
	Bullet * bullet[10];
	Razer razer(-1, "=", &screen);

	int playTime = 0;

	for (int i = 0; i < 10; i++)
	{
		bullet[i] = new Bullet(-1, "+", &screen);
	}


	while (true)
	{
		playTime++;
		if (player.getAlive() == false)
			break;
		screen.clear();

		if (_kbhit())
		{
			int c = _getch();
			switch (c) {
			case 'a':
				player.moveLeft();
				break;
			case 'd':
				player.moveRight();
				break;
			case ' ':
				if (bullet[0]->getGunMode()) {
					if (bullet[0]->getBulletNumber() < 10) {
						bullet[bullet[0]->getBulletNumber()]->fire(player.getPosition(), bullet[0]->getBulletNumber());
						bullet[0]->increaseBulletNumber();
						bullet[0]->resetReroaderTime();

					}
				}
				else {
					razer.fire();
				}

				player.seeInfoTime();
				break;
			case 'm':
				bullet[0]->gunModeChange();
				break;
			}
		}
		player.draw();
		enemy.draw();
		for (int i = 0; i < 10; i++)
		{
			bullet[i]->draw();
		}
		razer.draw(player.getPosition(), enemy.getPosition(), player.getFace(), enemy.getFace());
		enemy.enemyMove(player.getPosition());
		
		player.update(enemy.getPosition(), bullet[0]->getBulletNumber(), bullet[0]->getreroaderTime(), razer.getRazerTime(), razer.getRazerCoolTime(), bullet[0]->getGunMode());
		for (int i = 0; i < 10; i++)
		{
			enemy.update(bullet[i]->getPosition(), player.getPosition(), 0);
			bullet[i]->update(player.getfireDirection(), enemy.getPosition(), bullet[0]->getBulletNumber());
		}
		razer.update(player.getfireDirection());
		enemy.update(0, player.getPosition(), razer.getHitCheck());
		printf("%d", playTime);
		screen.render();
		Sleep(66);
	}

	printf("총 플레이 타임 : %d\n", playTime);
	printf("3초뒤에 종료됩니다.");
	Sleep(3000);

	return 0;
}