#ifndef REZHUDS
#define REZHUDS

#define MAX_IT_ITEMS 32

//Speedometer
class CHudSpeedometer : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
		INT pPlayerSpeed;
	private:
		INT width; INT height;
		char pSpeed[64];
};

//Clocks
class CHudClocks : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
		FLOAT m_flRealTimer;
	private:
		char pRealTimer[64];
		time_t tRealTime; tm* realtime;
		INT pTimerStrLen;
		FLOAT pTimerPos[2];
};

//Item-spawn timer
class CItemSpawnTimer : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
		BOOL pShouldDrawTimer[MAX_IT_ITEMS], bFirst[MAX_IT_ITEMS];
		FLOAT pTimerSeconds[MAX_IT_ITEMS];
		char pItemNames[MAX_IT_ITEMS][64]; //item names
		INT iLastUsedIndex;
	private:
		FLOAT iClTime[MAX_IT_ITEMS], pos[2];
		INT width, height, y_pos, offsets[2];
		char pTimerInfo[256];
};

//Teams board
class CHudTeamsBoard : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
	private:
		char pFrags[256];
		INT y_offset; INT title_y_offset;
		FLOAT pos[2];
		INT font_color[3];
};

//Smart crosshair
class CCustomCrosshair : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
		BOOL bCrosshairMustBeRed;
	private:
		INT pCrossColors[3];
		char szCrosshairSprite[64];
};

//Weapon reload status
class CHudReloadStatus : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
	private:
		FLOAT pReloadStatusPos[2];
};

//Spectator hud
class CSpectatorHud : public CHudBase {
	public:
		virtual INT Init( void );
		virtual INT Draw( float flTime );
	private:
		INT pSpecPlayers[2], iStartHeight[2], r, g, b;
		char pTeamInfo[256];
		std::string pTempWeapon;
};

#endif