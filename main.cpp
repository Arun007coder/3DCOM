#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <olcPixelGameEngine.h>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;

void beep()
{
	system("sudo sh -c \" echo - e '\a' > /dev/console \"");
}

class COM : public olc::PixelGameEngine
{
public:
	COM()
	{
		sAppName = "3DCOM";
	}
	~COM()
	{

	}

	virtual bool OnUserCreate()
	{
		map += L"#########.......";
		map += L"#...............";
		map += L"#.......########";
		map += L"#..............#";
		map += L"#......###.....#";
		map += L"#......###.....#";
		map += L"#..............#";
		map += L"###............#";
		map += L"##.............#";
		map += L"#......####..###";
		map += L"#......#.......#";
		map += L"#......#.......#";
		map += L"#..............#";
		map += L"#......#########";
		map += L"#..............#";
		map += L"################";
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (GetKey(olc::A).bHeld)
			PlayerAngle -= (fSpeed * 0.75f) * fElapsedTime;

		if (GetKey(olc::D).bHeld)
			PlayerAngle += (fSpeed * 0.75f) * fElapsedTime;

		if (GetKey(olc::W).bHeld)
		{
			PlayerX += sinf(PlayerAngle) * fSpeed * fElapsedTime;
			;
			PlayerY += cosf(PlayerAngle) * fSpeed * fElapsedTime;
			;
			if (map.c_str()[(int)PlayerX * nMapWidth + (int)PlayerY] == '#')
			{
				PlayerX -= sinf(PlayerAngle) * fSpeed * fElapsedTime;
				;
				PlayerY -= cosf(PlayerAngle) * fSpeed * fElapsedTime;
				;
				beep();
			}
		}

		if (GetKey(olc::S).bHeld)
		{
			PlayerX -= sinf(PlayerAngle) * fSpeed * fElapsedTime;
			;
			PlayerY -= cosf(PlayerAngle) * fSpeed * fElapsedTime;
			;
			if (map.c_str()[(int)PlayerX * nMapWidth + (int)PlayerY] == '#')
			{
				PlayerX += sinf(PlayerAngle) * fSpeed * fElapsedTime;
				;
				PlayerY += cosf(PlayerAngle) * fSpeed * fElapsedTime;
				;
#ifdef __linux__
				beep();
#pragma message("OS is Linux")
#elif _WIN32
#pragma message("OS is Windows")
				Beep(10000, 100);
#else
#pragma message("OS is unknown")
#endif
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (PlayerAngle - FOV / 2.0f) + ((float)x / (float)nScreenWidth) * FOV;

			float fStepSize = 0.1f;
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(PlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(PlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						bHitWall = true;

						vector<pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - PlayerY;
								float vx = (float)nTestX + tx - PlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right)
							 { return left.first < right.first; });

						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound)
						{
							bBoundary = true;
						}
						if (acos(p.at(1).second) < fBound)
						{
							bBoundary = true;
						}
						if (acos(p.at(2).second) < fBound)
						{
							bBoundary = true;
						}
					}
				}
			}

			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			olc::Pixel nShade = olc::BLACK;
			if (fDistanceToWall <= fDepth / 4.0f)
				nShade = olc::GREEN;
			else if (fDistanceToWall < fDepth / 3.0f)
				nShade = olc::DARK_GREEN;
			else if (fDistanceToWall < fDepth / 2.0f)
				nShade = olc::VERY_DARK_GREEN;
			else if (fDistanceToWall < fDepth)
				nShade = olc::VERY_DARK_GREY;
			else
				nShade = olc::BLACK;

			if (bBoundary)
				nShade = ' ';

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
					Draw(x, y, olc::RED);
				else if (y > nCeiling && y <= nFloor)
					Draw(x,y, nShade);
				else
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)
						nShade = olc::CYAN;
					else if (b < 0.5)
						nShade = olc::DARK_CYAN;
					else if (b < 0.75)
						nShade = olc::VERY_DARK_CYAN;
					else if (b < 0.9)
						nShade = olc::DARK_BLUE;
					else
						nShade = olc::BLACK;
					Draw(x, y, nShade);
				}
			}
		}

		//swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", PlayerX, PlayerY, PlayerAngle, 1.0f / fElapsedTime);

		olc::Pixel Shade = olc::BLACK;
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				if (map[ny * nMapWidth + nx] == '#')
					Shade = olc::DARK_GREY;
				else if (map[ny * nMapWidth + nx] == '.')
					Shade = olc::VERY_DARK_GREY;
				Draw(nx + 1, ny + 1, Shade);
			}
		Draw((int)PlayerX + 1, (int)PlayerY + 1, olc::BLUE);

		//screen[nScreenWidth * nScreenHeight - 1] = '\0';

		return true;
	}

private:
	int nScreenWidth = 120;
	int nScreenHeight = 80;
	int nMapWidth = 16;
	int nMapHeight = 16;

	float PlayerX = 14.7f;
	float PlayerY = 5.09f;
	float PlayerAngle = 0.0f;
	float FOV = 3.14159f / 4.0f;
	float fDepth = 16.0f;
	wstring map;
	float fSpeed = 5.0f;
};

int main()
{
	COM com;
	com.Construct(120, 80, 8, 8);
	com.Start();
}