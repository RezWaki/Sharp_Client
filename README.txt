==- SHARP CLIENT -==
    update v11.0

This is client library for Half-Life / Adrenaline Gamer mod
Feel free to use it under AG because it contains all basic AG features

To install it just extract all from archive into "valve" / "ag" folder,
agree the replace if needed

You can set up this client via console commands, simply put them into your
cfg file either type in console to test, full commands list:

  | COMMAND	  	| VALUE TYPE| DESCRIPTION
========================================================================================
  | cl_drawteams  	| 1/0	    | Toggle teamscores panel for duel/teamplay
  | cl_scorecolor 	| R G B	    | You can control the scoreboard color with it
  | cl_scorehighlight	| 0/1/2	    | Scoreboard nickname highlight modes, 0 - disable
  | cl_drawteamscores	| 1/0	    | Toggle score draw on teamscores panel
  | cl_crosshair	| 0/1/2/3/4/5|Switch between custom crosshairs, 0 - disable
  | cl_crosscolor	| R G B	    | Control the custom crosshair color
  | cl_crossdotsize	| integer   | Control the "dot" custom crosshair size
  | cl_autobhop		| 1/0	    | Autobunnyhop
  | cl_slide		| 0/1/2	    | DuckSpam modes (slide/doubleduck emulation)
  | cl_slide_interval	| Float	    | DuckSpam frequency for slidemode (0.5 recommended)
  | cl_hudcolorforname  | 1/0	    | Use last-used color for nickname in scoreboard
  | cl_hudcolor		| R G B	    | Control the whole HUD color
  | cl_vote_defaultpos  | 1/0	    | Use default position for AG vote HUDs
  | cl_gaussbeam_color  | R G B	    | Control the Gauss beam color
  | r_fakedrawentities  | 0/1/2/3/4 | Control the "r_drawentities" cvar without limits
  | cl_hudweapon	| 1/0	    | Draw current weapon in HUD
  | cl_smart_crosshair  | 1/0	    | Crosshair becomes red on players
  | r_noplayerlights	| 1/0	    | Remove playermodels light processing
  | r_noitemlights	| 1/0	    | Remove all models light processing
  | r_forcerendercolors | R G B	    | Your colors on models with disabled light proc.
  | cl_newhud		| 0/1/2	    | New HUD style, 2 = replace ammo hud too
  | cl_usenewteamcolors | 1/0	    | New teamcolors for scoreboard
  | cl_hltvmode		| 0/1/2	    | Weapon models drawing HLTV-like 2 modes
  | r_extrachrome	| 0/1/2	    | More fancy chrome render modes, 0 disable
  | cl_flashplayer	| 0/1	    | Make players flashing red every second
  | cl_specwh		| 0/1	    | Wallhack+chams for SPECTATOR ONLY
  | cl_itemtimer	| 0/1	    | Weapons & items next-spawn timer
  | cl_rainbowhud	| 0/1/2	    | Rainbow color hud changer 2 modes
  | cl_spechud		| 0/1/2	    | Toggle spectator hud, 1 = duel, 2 = teamplay
  | cl_specbk		| 0/string  | Sprite background for spec duel hud (never tested)
  | cl_specplayers	| int int   | Type here indexes (NOT userids) for players
  | cl_spechud_offset	| integer   | Enter here Y-axis offset for teamplay spechud
  | cl_damagepunch	| 0/1	    | Toggle punch-effect when getting damage
  | cl_autorecord	| 0/1	    | Toggle agstart demo autorecord
  | cl_specoffset	| integer   | Set spectator mode camera offset (height)
  | cl_oldladdersteps	| 0/1	    | Remove stairs climbing smoothing effect
  | cl_weaponpos	| x y z     | Weaponmodel offset (x, y, z)
  | cl_cross_sprname	| string    | Sprite filename for crosshair mode 5
  | cl_cross_sprsize	| integer   | Width of sprite frame (width & height must be equal)
  | cl_newscoreboard	| 0/1	    | Toggle new scoreboard
  | cl_newscore_offset_x| integer   | New scoreboard's x offset
  | cl_newscore_offset_y| integer   | New scoreboard's y offset
  | cl_traceline	| integer   | Draw bullet traceline with specified width, 0 - disable
  | cl_maxtracelines	| integer   | Maximum tracelines to be drawn
  | cl_traceline_color  | R G B     | Specify tracelines color
  | cl_cross_trace	| 0/1	    | Draw traceline for your crosshair
  | cl_usenewhudstring	| 0/1	    | Use new fonts for DrawHudString()
  | cl_drawmyname	| 0/1	    | Whether or not draw your nickname with new hud style
  | cl_hidehudsininterm | 0/1	    | Hide your HUD if it's intermission?
  | cl_showinterstats	| 0/1	    | Show your shot stats during intermission
  | cl_blackdeathscreen | 0/1	    | Black screen while you're dead
  | cl_deathcam_height  | integer   | Camera offset above the ground when you're dead
  | cl_deathcam_angle	| integer   | Camera angle offset when you're dead
  | cl_clocks		| 0/1	    | Show clocks
  | cl_clocks_pos	| flt flt   | Specify the clocks position in floats (1 = max, 0 = min)
  | cl_clocks_fmt	| 0/1	    | Specify the clocks mode (12 hours/24 hours)
  | cl_clocks_del	| string    | The clocks delimiter
  | hud_timer		| 0/1	    | The AG timer that shows during the match


Other fixes/addins/removes:
	[REMOVED] Scoreboard mouse click fixed
	[ADDED] New HUD
	[ADDED] Playermodels/anymodels light processing control
	[ADDED] Force light colors on models
	[ADDED] Smart crosshair
	[FIXED] New HUD's pain-hud fix
	[ADDED] Weapon models drawing HLTV-like 2 modes
	[FIXED] cl_hudcolor health drawing bug with cl_newhud 0
	[FIXED] Array-size dependent memory corruption bug
	[ADDED] 2 new chrome render modes
	[ADDED] Spectator wallhack with chams, its gonna work ONLY if you are spectator
	[ADDED] Player model flashing
	[ADDED] Weapons & items next-spawn timer
	[ADDED] Rainbow color hud changer
	[ADDED] Spectator HUD with 2 modes, for more details read spechud.txt
	[ADDED] Screen punch-effect when getting damage
	[FIXED] cl_newhud 2 ammo hud's "0" problem fixed
	[ADDED] Weapon position control (offsets)
	[ADDED] New crosshair mode (5 - sprite crosshair)
	[ADDED] Spectator 1st person mode camera offset (height)
	[ADDED] Removable stairs climbing smoothing effect
	[FIXED] Spectator HUD minor bugs
	[ADDED] New scoreboard (very raw now)
	Other minor fixes, e.g. flashplayer time fix
	[ADDED] AG's hud_timer
	[FIXED] Buggy player counting in the scoreboard,
		what was causing some of the crashes
	[FIXED] Game-time management bug after intermission
	[FIXED] Server-title bug in the scoreboard
	[ADDED] Every cvar is now being saved after you exit the game
	[ADDED] Clocks
	[ADDED] Shot stats in intermission
