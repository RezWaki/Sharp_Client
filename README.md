# Sharp_Client
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
  | cl_crosshair	| 0/1/2/3/4 | Switch between custom crosshairs, 0 - disable
  | cl_crosscolor	| R G B	    | Control the custom crosshair color
  | cl_crossdotsize	| integer   | Control the "dot" custom crosshair size
  | cl_autobhop		| 1/0	    | Autobunnyhop
  | cl_slide		| 0/1/2	    | DuckSpam modes (slide/doubleduck emulation)
  | cl_slidefactor	| Float	    | DuckSpam frequency for slidemode (0.9 recommended)
  | cl_hudcolorforname  | 1/0	    | Use last-used color for nickname in scoreboard
  | cl_hudcolor		| R G B	    | Control the whole HUD color
  | cl_vote_defaultpos  | 1/0	    | Use default position for AG vote HUDs
  | cl_gaussbeam_color  | R G B	    | Control the Gauss beam color
  | r_fakedrawentities  | 0/1/2/3/4 | Control the "r_drawentities" cvar without limits
  | cl_hudweapon	| 1/0	    | Draw current weapon in HUD

Other fixes/addins/removes:
	[REMOVED] Scoreboard mouse click fixed
