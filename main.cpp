#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <olcPixelGameEngine.h>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;

#ifndef _WIN32

void beep()
{
	system("if [ $(whoami) != root ]; then echo \"Cannot use beep because you are not root\"; else echo - e '\\a' > /dev/console; fi");
	//system("play -n synth 0.1 sine 440");
}
#endif

std::wstring stringToWstring(const std::string &t_str)
{
	// setup converter
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	// use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(t_str);
}

class COM : public olc::PixelGameEngine
{
public:
	wstring map;
	wstring termMap;
	wstring mirrormap;
	COM()
	{
		sAppName = "3DCOM";
	}
	~COM()
	{

	}

	bool loadmap(char* filename)
	{
		cout << "Loading " << filename << endl;
		ifstream file(filename);

		if (file.fail())
		{
			return false;
		}

		if (file.is_open())
		{
			char c[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

			while (file >> c)
			{
				termMap += stringToWstring(c) + L"\n";
				map += stringToWstring(c);
				mirrormap += stringToWstring(c);
			}

			reverse(mirrormap.begin(), mirrormap.end());
			file.close();
			return true;
		}

		if (map.length() == 0)
		{
			return false;
		}
	}

	void store_var(string var, string value)
	{
		if (strcmp(var.c_str(), "PlayerX") == 0 || strcmp(var.c_str(), "PlayerX ") == 0)
		{
			stringstream val(value);
			val >> PlayerX;
		}
		else if (strcmp(var.c_str(), "PlayerY") == 0 || strcmp(var.c_str(), "PlayerY ") == 0)
		{
			stringstream val(value);
			val >> PlayerY;
		}
		else if (strcmp(var.c_str(), "speed") == 0 || strcmp(var.c_str(), "speed ") == 0)
		{
			stringstream val(value);
			val >> fSpeed;
		}
		else if (strcmp(var.c_str(), "map") == 0 || strcmp(var.c_str(), "map ") == 0)
		{
			value.replace(0, 1, "\0");
			MapName = new char[value.length()];
			strcpy(MapName, value.c_str());
		}
		else if (strcmp(var.c_str(), "Player angle") == 0 || strcmp(var.c_str(), "Player angle ") == 0)
		{
			stringstream val(value);
			val >> PlayerAngle;
		}
		else if (strcmp(var.c_str(), "Control") == 0 || strcmp(var.c_str(), "Control ") == 0)
		{
			if (strcmp(value.c_str(), "WASD") == 0 || strcmp(value.c_str(), " WASD") == 0 || strcmp(value.c_str(), "WASD ") == 0)
			{
				isWASD = true;
			}
			else if (strcmp(value.c_str(), "Arrows") == 0 || strcmp(value.c_str(), " Arrows") == 0 || strcmp(value.c_str(), "Arrows ") == 0)
			{
				isWASD = false;
			}
		}
		else if(strcmp(var.c_str(), "Boundary") == 0 || strcmp(var.c_str(), "Boundary ") == 0)
		{
			if (strcmp(value.c_str(), "True") == 0 || strcmp(value.c_str(), " true") == 0 || strcmp(value.c_str(), "True ") == 0)
			{
				Boundary = true;
			}
			else if (strcmp(value.c_str(), "False") == 0 || strcmp(value.c_str(), " false") == 0 || strcmp(value.c_str(), "False ") == 0)
			{
				Boundary = false;
			}
		}
		else if (strcmp(var.c_str(), "Beep") == 0 || strcmp(var.c_str(), "Beep ") == 0)
		{
			if (strcmp(value.c_str(), "True") == 0 || strcmp(value.c_str(), " true") == 0 || strcmp(value.c_str(), "True ") == 0)
			{
				canBeep = true;
			}
			else if (strcmp(value.c_str(), "False") == 0 || strcmp(value.c_str(), " false") == 0 || strcmp(value.c_str(), "False ") == 0)
			{
				canBeep = false;
			}
		}
		else if(strcmp(var.c_str(), "Debug") == 0 || strcmp(var.c_str(), "Debug ") == 0)
		{
			if (strcmp(value.c_str(), "True") == 0 || strcmp(value.c_str(), " true") == 0 || strcmp(value.c_str(), "True ") == 0)
			{
				canShowDebug = true;
			}
			else if (strcmp(value.c_str(), "False") == 0 || strcmp(value.c_str(), " false") == 0 || strcmp(value.c_str(), "False ") == 0)
			{
				canShowDebug = false;
			}
		}
		else
		{
			cout << "Variable not found: " << var << endl;
		}
	}

	bool loadConf(char *filename)
	{
		ifstream file(filename);
		std::string sline;
		while (std::getline(file, sline))
		{
			std::istringstream line(sline);
			std::string key;
			if (std::getline(line, key, '='))
			{
				std::string value;
				if (std::getline(line, value))
				{
					store_var(key, value);
				}
			}
		}
	}

	virtual bool OnUserCreate()
	{
		loadConf("conf.txt");
		if (!loadmap(MapName))
		{
			cout << "Failed to load map" << endl;
			cout << "Loading default map" << endl;

			map += L"################";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"#..............#";
			map += L"################";

			termMap += L"################\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"#..............#\n";
			termMap += L"################\n";
		}
		
		cout << "map loaded" << endl;
		cout << "map size: " << map.size() << endl;
		cout << "Player starting X position: " << PlayerX  << endl;
		cout << "Player starting Y position: " << PlayerY  << endl;
		cout << "Player starting angle: " << PlayerAngle << endl;
		cout << "Player speed: " << fSpeed << endl;
		cout << "Player FOV: " << FOV << endl;
		cout << "Map resolution: " << nMapWidth << "x" << nMapHeight << endl;
		cout << "map: " << endl;
		wcout << termMap << endl;
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (isWASD)
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
					if (canBeep)
					{
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
					if (canBeep)
					{
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
			}
		}
		else
		{
			if (GetKey(olc::LEFT).bHeld)
				PlayerAngle -= (fSpeed * 0.75f) * fElapsedTime;

			if (GetKey(olc::RIGHT).bHeld)
				PlayerAngle += (fSpeed * 0.75f) * fElapsedTime;

			if (GetKey(olc::UP).bHeld)
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
					if (canBeep)
					{
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
			}

			if (GetKey(olc::DOWN).bHeld)
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
					if (canBeep)
					{
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
				if(Boundary)
				{
					nShade = olc::BLACK;
				}
				else
				{
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
				}

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

		if (canShowDebug)
		{
			char *txt;
			size_t size;
			olc::vf2d pos = {0, 0};
			olc::vf2d scale = {0.3f, 0.3f};
			size = snprintf(NULL, 0, "X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", PlayerX, PlayerY, PlayerAngle, 1.0f / fElapsedTime);
			txt = (char *)malloc(size + 1);
			snprintf(txt, size + 1, "X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", PlayerX, PlayerY, PlayerAngle, 1.0f / fElapsedTime);
			DrawStringDecal(pos, txt, olc::YELLOW, scale);
		}

		olc::Pixel Shade = olc::BLACK;
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				if (mirrormap[ny * nMapWidth + nx] == '#')
					Draw(nx + 1, ny + 1, olc::DARK_GREY);
				else if (mirrormap[ny * nMapWidth + nx] == '.')
					Draw(nx + 1, ny + 1, olc::VERY_DARK_GREY);
			}
		Draw((int)PlayerX + 1, (int)PlayerY + 1, olc::BLUE);

		return true;
}

private:
	int nScreenWidth = 120;
	int nScreenHeight = 80;
	int nMapWidth = 16;
	int nMapHeight = 16;

	char *MapName = "map.txt";
	bool isWASD = true;
	bool Boundary = true;
	bool canBeep = true;
	bool canShowDebug = true;

	float PlayerX = 14.7f;
	float PlayerY = 5.09f;
	float PlayerAngle = 0.0f;
	float FOV = 3.14159f / 4.0f;
	float fDepth = 16.0f;
	float fSpeed = 5.0f;
};

int main()
{
	COM com;
	com.Construct(120, 80, 8, 8);
	com.Start();
}