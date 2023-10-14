//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//  Copyright (C) 2023 Fabian Greffrath
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// DESCRIPTION:  Heads-up displays
//
//-----------------------------------------------------------------------------

// killough 5/3/98: remove unnecessary headers

#include "doomstat.h"
#include "doomkeys.h"
#include "hu_stuff.h"
#include "hu_obituary.h"
#include "hu_lib.h"
#include "st_stuff.h" /* jff 2/16/98 need loc of status bar */
#include "w_wad.h"
#include "s_sound.h"
#include "dstrings.h"
#include "sounds.h"
#include "d_deh.h"   /* Ty 03/27/98 - externalization of mapnamesx arrays */
#include "r_draw.h"
#include "m_input.h"
#include "p_map.h" // crosshair (linetarget)
#include "m_misc2.h"
#include "m_swap.h"
#include "r_main.h"
#include "u_scanner.h"
#include "m_nughud.h" // [Nugget]

// global heads up display controls

int hud_active;       //jff 2/17/98 controls heads-up display mode
int hud_displayed;    //jff 2/23/98 turns heads-up display on/off
secretmessage_t hud_secret_message; // "A secret is revealed!" message
int hud_widget_font;
int hud_draw_bargraphs;
int hud_threelined_widgets;

extern int screenSize; // [Nugget]

//
// Locally used constants, shortcuts.
//
// Ty 03/28/98 -
// These four shortcuts modifed to reflect char ** of mapnamesx[]
#define HU_TITLE  (*mapnames[(gameepisode-1)*9+gamemap-1])
#define HU_TITLE2 (*mapnames2[gamemap-1])
#define HU_TITLEP (*mapnamesp[gamemap-1])
#define HU_TITLET (*mapnamest[gamemap-1])

char *chat_macros[] =    // Ty 03/27/98 - *not* externalized
{
  HUSTR_CHATMACRO0,
  HUSTR_CHATMACRO1,
  HUSTR_CHATMACRO2,
  HUSTR_CHATMACRO3,
  HUSTR_CHATMACRO4,
  HUSTR_CHATMACRO5,
  HUSTR_CHATMACRO6,
  HUSTR_CHATMACRO7,
  HUSTR_CHATMACRO8,
  HUSTR_CHATMACRO9
};

char **player_names[] =
{
  &s_HUSTR_PLRGREEN,
  &s_HUSTR_PLRINDIGO,
  &s_HUSTR_PLRBROWN,
  &s_HUSTR_PLRRED
};

//jff 3/17/98 translate player colmap to text color ranges
int plyrcoltran[MAXPLAYERS]={CR_GREEN,CR_GRAY,CR_BROWN,CR_RED};

static player_t *plr;

// font sets
static hu_font_t big_font = {.space_width = 4, .tab_width = 15, .tab_mask = ~15},
                 sml_font = {.space_width = 5, .tab_width =  7, .tab_mask =  ~7};
static hu_font_t *doom_font = &big_font, *boom_font = &sml_font;
patch_t **hu_font = big_font.patches;

static int CR_BLUE = CR_BLUE1;

// widgets

// [FG] Vanilla widgets point to a boolean variable (*on) to determine
//      if they are enabled, always big_font, mostly left-aligned
static hu_multiline_t w_title;
static hu_multiline_t w_message;
static hu_multiline_t w_chat;
static hu_multiline_t w_secret; // [crispy] secret message widget

// [FG] special pony, per-player chat input buffer
static hu_line_t w_inputbuffer[MAXPLAYERS];

// [FG] Boom widgets are built using builder() functions
static hu_multiline_t w_ammo;   //jff 2/16/98 new ammo widget for hud
static hu_multiline_t w_armor;  //jff 2/16/98 new armor widget for hud
static hu_multiline_t w_health; //jff 2/16/98 new health widget for hud
static hu_multiline_t w_keys;   //jff 2/16/98 new keys widget for hud
static hu_multiline_t w_weapon; //jff 2/16/98 new weapon widget for hud

// [FG] extra Boom widgets, that need to be explicitly enabled
static hu_multiline_t w_monsec; //jff 2/16/98 new kill/secret widget for hud
static hu_multiline_t w_sttime; // time above status bar
static hu_multiline_t w_coord;
static hu_multiline_t w_fps;

// [Nugget] 
static hu_multiline_t w_powers; // Powerup timers

// [Cherry]
static hu_multiline_t w_attempts; // Attempt counter
static hu_multiline_t w_move; // Movement widget
#define MAX_HUDS 3
#define MAX_WIDGETS (12+1) // [Nugget] Support more widgets
#define NUGHUDSLOT 3 // [Nugget] NUGHUD

// [Nugget] Extra slot for NUGHUD
static hu_widget_t doom_widgets[MAX_HUDS+1][MAX_WIDGETS] = {
  {
    {&w_title,   align_left,   align_bottom},
    {&w_message, align_left,   align_top},
    {&w_chat,    align_left,   align_top},
    {&w_secret,  align_center, align_direct, 0, 84},
    {NULL}
  }, {
    {&w_title,   align_left,   align_bottom},
    {&w_message, align_left,   align_top},
    {&w_chat,    align_left,   align_top},
    {&w_secret,  align_center, align_direct, 0, 84},
    {NULL}
  }, {
    {&w_title,   align_left,   align_bottom},
    {&w_message, align_left,   align_top},
    {&w_chat,    align_left,   align_top},
    {&w_secret,  align_center, align_direct, 0, 84},
    {NULL}
  }, { // [Nugget] NUGHUD slot
    {&w_title,   align_left, align_top},
    {&w_message, align_left, align_top},
    {&w_chat,    align_left, align_top},
    {&w_secret,  align_left, align_top},
    {NULL}
  }
};

// [Nugget] Extra slot for NUGHUD
static hu_widget_t boom_widgets[MAX_HUDS+1][MAX_WIDGETS] = {
  {
    {&w_monsec,   align_left,  align_top},
    {&w_sttime,   align_left,  align_top},
    {&w_attempts, align_left,  align_top}, // [Cherry] Attempt counter
    {&w_move,     align_left,  align_top}, // [Cherry] Movement widget
    {&w_powers,   align_right, align_top}, // [Nugget] Powerup timers
    {&w_coord,    align_right, align_top},
    {&w_fps,      align_right, align_top},
    {NULL}
  }, {
    {&w_armor,    align_left,  align_bottom},
    {&w_health,   align_left,  align_bottom},
    {&w_ammo,     align_left,  align_bottom},
    {&w_weapon,   align_left,  align_bottom},
    {&w_keys,     align_left,  align_bottom},

    {&w_monsec,   align_left,  align_bottom},
    {&w_sttime,   align_left,  align_bottom},
    {&w_attempts, align_left,  align_bottom}, // [Cherry] Attempt counter
    {&w_move,     align_left,  align_bottom}, // [Cherry] Movement widget
    {&w_powers,   align_right, align_top}, // [Nugget] Powerup timers
    {&w_coord,    align_right, align_top},
    {&w_fps,      align_right, align_top},
    {NULL}
  }, {
    {&w_health,   align_right, align_top},
    {&w_armor,    align_right, align_top},
    {&w_ammo,     align_right, align_bottom},
    {&w_weapon,   align_right, align_bottom},
    {&w_keys,     align_left,  align_bottom},

    {&w_monsec,   align_left,  align_bottom},
    {&w_sttime,   align_left,  align_bottom},
    {&w_attempts, align_left,  align_bottom}, // [Cherry] Attempt counter
    {&w_move,     align_left,  align_bottom}, // [Cherry] Movement widget
    {&w_powers,   align_right, align_top}, // [Nugget] Powerup timers
    {&w_coord ,   align_right, align_top},
    {&w_fps,      align_right, align_top},
    {NULL}
  }, { // [Nugget] NUGHUD slot
    {&w_monsec,   align_left, align_top},
    {&w_sttime,   align_left, align_top},
    {&w_attempts, align_left, align_top}, // [Cherry] Attempt counter
    {&w_move,     align_left, align_top}, // [Cherry] Movement widget
    {&w_powers,   align_left, align_top}, // [Nugget] Powerup timers
    {&w_coord ,   align_left, align_top},
    {&w_fps,      align_left, align_top},
    {NULL}
  }
};

static hu_widget_t *doom_widget = doom_widgets[0],
                   *boom_widget = boom_widgets[0];

static void HU_ParseHUD (void);

static char       chat_dest[MAXPLAYERS];
boolean           chat_on;
static boolean    message_on;
static boolean    has_message;       // killough 12/98
boolean           message_dontfuckwithme;
static boolean    message_nottobefuckedwith;
static int        message_counter;
static int        message_count;     // killough 11/98
static int        chat_count;        // killough 11/98
static boolean    secret_on;
static int        secret_counter;

boolean           message_centered;
boolean           message_colorized;

extern int        showMessages;
static boolean    headsupactive = false;

//jff 2/16/98 hud supported automap colors added
int hudcolor_titl;  // color range of automap level title
int hudcolor_xyco;  // color range of new coords on automap
//jff 2/16/98 hud text colors, controls added
int hudcolor_mesg;  // color range of scrolling messages
int hudcolor_chat;  // color range of chat lines
int hud_msg_lines;  // number of message lines in window
// [Nugget] Restore message scroll direction toggle
int hud_msg_scrollup;  // killough 11/98: allow messages to scroll upwards
int message_list;      // killough 11/98: made global

int message_timer  = HU_MSGTIMEOUT * (1000/TICRATE);     // killough 11/98
int chat_msg_timer = HU_MSGTIMEOUT * (1000/TICRATE);     // killough 11/98

static void HU_InitCrosshair(void);
void HU_StartCrosshair(void); // [Nugget] Not static anymore
boolean hud_crosshair_on; // [Nugget] Replace with crosshair toggle

//
// Builtin map names.
// The actual names can be found in DStrings.h.
//
// Ty 03/27/98 - externalized map name arrays - now in d_deh.c
// and converted to arrays of pointers to char *
// See modified HUTITLEx macros
extern char **mapnames[];
extern char **mapnames2[];
extern char **mapnamesp[];
extern char **mapnamest[];

// key tables
// jff 5/10/98 french support removed,
// as it was not being used and couldn't be easily tested
//
const char shiftxform[] =
{
  0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
  11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
  21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31,
  ' ', '!', '"', '#', '$', '%', '&',
  '"', // shift-'
  '(', ')', '*', '+',
  '<', // shift-,
  '_', // shift--
  '>', // shift-.
  '?', // shift-/
  ')', // shift-0
  '!', // shift-1
  '@', // shift-2
  '#', // shift-3
  '$', // shift-4
  '%', // shift-5
  '^', // shift-6
  '&', // shift-7
  '*', // shift-8
  '(', // shift-9
  ':',
  ':', // shift-;
  '<',
  '+', // shift-=
  '>', '?', '@',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '[', // shift-[
  '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
  ']', // shift-]
  '"', '_',
  '\'', // shift-`
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '{', '|', '}', '~', 127
};

static boolean VANILLAMAP(int e, int m)
{
  if (gamemode == commercial)
    return (e == 1 && m > 0 && m <=32);
  else
    return (e > 0 && e <= 4 && m > 0 && m <= 9);
}

struct {
  char **str;
  const int cr;
  const char *col;
} static const colorize_strings[] = {
  // [Woof!] colorize keycard and skull key messages
  {&s_GOTBLUECARD, CR_BLUE2, " blue "},
  {&s_GOTBLUESKUL, CR_BLUE2, " blue "},
  {&s_GOTREDCARD,  CR_RED,  " red "},
  {&s_GOTREDSKULL, CR_RED,  " red "},
  {&s_GOTYELWCARD, CR_GOLD, " yellow "},
  {&s_GOTYELWSKUL, CR_GOLD, " yellow "},
  {&s_PD_BLUEC,    CR_BLUE2, " blue "},
  {&s_PD_BLUEK,    CR_BLUE2, " blue "},
  {&s_PD_BLUEO,    CR_BLUE2, " blue "},
  {&s_PD_BLUES,    CR_BLUE2, " blue "},
  {&s_PD_REDC,     CR_RED,  " red "},
  {&s_PD_REDK,     CR_RED,  " red "},
  {&s_PD_REDO,     CR_RED,  " red "},
  {&s_PD_REDS,     CR_RED,  " red "},
  {&s_PD_YELLOWC,  CR_GOLD, " yellow "},
  {&s_PD_YELLOWK,  CR_GOLD, " yellow "},
  {&s_PD_YELLOWO,  CR_GOLD, " yellow "},
  {&s_PD_YELLOWS,  CR_GOLD, " yellow "},

  // [Woof!] colorize multi-player messages
  {&s_HUSTR_PLRGREEN,  CR_GREEN, "Green: "},
  {&s_HUSTR_PLRINDIGO, CR_GRAY,  "Indigo: "},
  {&s_HUSTR_PLRBROWN,  CR_BROWN, "Brown: "},
  {&s_HUSTR_PLRRED,    CR_RED,   "Red: "},
};

static char* PrepareColor(const char *str, const char *col)
{
    char *str_replace, col_replace[16];

    M_snprintf(col_replace, sizeof(col_replace),
               "\x1b%c%s\x1b%c", '0'+CR_ORIG, col, '0'+CR_ORIG);
    str_replace = M_StringReplace(str, col, col_replace);

    return str_replace;
}

static void UpdateColor(char *str, int cr)
{
    int i;
    int len = strlen(str);

    if (!message_colorized)
    {
        cr = CR_ORIG;
    }

    for (i = 0; i < len; ++i)
    {
        if (str[i] == '\x1b' && i + 1 < len)
        {
          str[i + 1] = '0'+cr;
          break;
        }
    }
}

void HU_ResetMessageColors(void)
{
    int i;

    for (i = 0; i < arrlen(colorize_strings); i++)
    {
        UpdateColor(*colorize_strings[i].str, colorize_strings[i].cr);
    }
}

extern boolean st_invul;

static char* ColorByHealth(int health, int maxhealth, boolean invul)
{
  if (invul)
    return colrngs[CR_GRAY];

  health = 100 * health / maxhealth;

  if (health < health_red)
    return colrngs[hudcolor_th_low];
  else if (health < health_yellow)
    return colrngs[hudcolor_th_ok];
  else if (health <= health_green)
    return colrngs[hudcolor_th_good];
  else
    return colrngs[hudcolor_th_extra];
}

static int lightest_color, darkest_color;

static void HU_InitDemoProgressBar (void)
{
  byte *playpal = W_CacheLumpName("PLAYPAL", PU_STATIC);

  lightest_color = I_GetPaletteIndex(playpal, 0xFF, 0xFF, 0xFF);
  darkest_color  = I_GetPaletteIndex(playpal, 0x00, 0x00, 0x00);
}

//
// HU_Init()
//
// Initialize the heads-up display, text that overwrites the primary display
//
// Passed nothing, returns nothing
//
void HU_Init(void)
{
  int i, j;
  char buffer[9];

  // load the heads-up font
  for (i = 0, j = HU_FONTSTART; i < HU_FONTSIZE; i++, j++)
  {
    sprintf(buffer, "STCFN%.3d", j);
    if (W_CheckNumForName(buffer) != -1)
      big_font.patches[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);

    if ('0' <= j && j <= '9')
    {
      sprintf(buffer, "DIG%.1d", j - 48);
      sml_font.patches[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
    }
    else if ('A' <= j && j <= 'Z')
    {
      sprintf(buffer, "DIG%c", j);
      sml_font.patches[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
    }
    else if (j > 122)
    {
      sprintf(buffer, "STBR%.3d", j);
      sml_font.patches[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
    }
    else
    {
      sprintf(buffer, "DIG%.2d", j);
      if (W_CheckNumForName(buffer) != -1)
        sml_font.patches[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
    }

    // [FG] small font available, big font unavailable
    if (big_font.patches[i] == NULL && sml_font.patches[i] != NULL)
    {
      big_font.patches[i] = sml_font.patches[i];
    }
    // [FG] big font available, small font unavailable
    else if (big_font.patches[i] != NULL && sml_font.patches[i] == NULL)
    {
      sml_font.patches[i] = big_font.patches[i];
    }
    // [FG] both fonts unavailable, fall back to '!'
    else if (big_font.patches[i] == NULL && sml_font.patches[i] == NULL)
    {
      sml_font.patches[i] =
      big_font.patches[i] = big_font.patches[0];
    }
  }

  //jff 2/26/98 load patches for keys and double keys
  for (i = HU_FONTSIZE, j = 0; j < 6; i++, j++)
  {
    sprintf(buffer, "STKEYS%.1d", j);
    sml_font.patches[i] =
    big_font.patches[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
  }

  // [FG] calculate font height once right here
  sml_font.line_height = SHORT(sml_font.patches['A'-HU_FONTSTART]->height) + 1;
  big_font.line_height = SHORT(big_font.patches['A'-HU_FONTSTART]->height) + 1;

  // [FG] support crosshair patches from extras.wad
  HU_InitCrosshair();

  HU_InitDemoProgressBar();

  HU_InitObituaries();
  HU_ParseHUD();

  // [Woof!] prepare player messages for colorization
  for (i = 0; i < arrlen(colorize_strings); i++)
  {
    *colorize_strings[i].str = PrepareColor(*colorize_strings[i].str, colorize_strings[i].col);
  }

  HU_ResetMessageColors();
}

// [FG] support centered player messages

static void HU_set_centered_message()
{
  int i, j;

  for (i = 0; i < MAX_HUDS+1; i++) // [Nugget] Extra slot for NUGHUD
  {
    hu_widget_t *const d_w = doom_widgets[i];

    for (j = 0; d_w[j].multiline; j++)
    {
      if (d_w[j].multiline == &w_message)
      {
        if (d_w[j].h_align == align_direct)
          continue;

        // [FG] save original alignment in the unused x coordinate
        if (d_w[j].x == 0)
          d_w[j].x = d_w[j].h_align;

        d_w[j].h_align = message_centered ? align_center : d_w[j].x;
      }
    }
  }
}

static inline void HU_cond_build_widget (hu_multiline_t *const multiline, boolean cond)
{
  if (cond)
  {
    multiline->builder();
    multiline->built = true;
  }
}

void HU_disable_all_widgets (void)
{
  hu_widget_t *w = boom_widget;

  while (w->multiline)
  {
    w->multiline->built = false;
    w++;
  }
}

//
// HU_Stop()
//
// Make the heads-up displays inactive
//
// Passed nothing, returns nothing
//
void HU_Stop(void)
{
  headsupactive = false;
}

//
// HU_Start(void)
//
// Create and initialize the heads-up widgets, software machines to
// maintain, update, and display information over the primary display
//
// This routine must be called after any change to the heads up configuration
// in order for the changes to take effect in the actual displays
//
// Passed nothing, returns nothing
//

static void HU_widget_build_ammo (void);
static void HU_widget_build_armor (void);
static void HU_widget_build_powers(void); // [Nugget] Powerup timers
static void HU_widget_build_coord (void);
static void HU_widget_build_fps (void);
static void HU_widget_build_health (void);
static void HU_widget_build_keys (void);
static void HU_widget_build_frag (void);
static void HU_widget_build_monsec(void);
static void HU_widget_build_sttime(void);
static void HU_widget_build_title (void);
static void HU_widget_build_weapon (void);
static void HU_widget_build_attempts(void); // [Cherry] Attempt counter
static void HU_widget_build_move(void); // [Cherry] Movement widget

// [Nugget] /-----------------------------------------------------------------

static void NughudAlignWidget(nughud_alignable_t *aligner, hu_widget_t *alignee)
{
  alignee->y = aligner->y;

  // Messages hack
  if (aligner->x == -1) {
    alignee->h_align = message_centered ? align_center : align_left;
    alignee->x = 1994;
    return;
  }
  
  switch (aligner->align) {
    case -1:  alignee->h_align = align_left;    break;
    case  0:  alignee->h_align = align_center;  break;
    case  1:  alignee->h_align = align_right;   break;
    default:                                    break;
  }

  alignee->x = aligner->x + NUGHUDWIDESHIFT(aligner->wide);
}

void HU_NughudAlignTime(void)
{
  hu_widget_t *w = boom_widgets[NUGHUDSLOT];

  while (w->multiline) {
    if (w->multiline == &w_sttime)
    {
      if (nughud.time_sts && !hud_level_stats && (!automapactive || !map_level_stats))
      {
        // Relocate Time text line to position of Stats text line
        NughudAlignWidget(&nughud.sts, w);
      }
      else
      { NughudAlignWidget(&nughud.time, w); }

      break;
    }
    else { w++; }
  }
}

// [Nugget] -----------------------------------------------------------------/

void HU_Start(void)
{
  const boolean inter = gamestate == GS_INTERMISSION; // [Cherry]
  int i;

  if (headsupactive)                    // stop before starting
    HU_Stop();

  plr = &players[displayplayer];        // killough 3/7/98
  message_on = false;
  message_dontfuckwithme = false;
  message_nottobefuckedwith = false;
  chat_on = false;
  secret_on = false;

  // killough 11/98:
  message_counter = 0;
  message_count = (message_timer  * TICRATE) / 1000 + 1;
  chat_count    = (chat_msg_timer * TICRATE) / 1000 + 1;

  // [crispy] re-calculate WIDESCREENDELTA
  I_GetScreenDimensions();

  // create the message widget
  HUlib_init_multiline(&w_message, message_list ? hud_msg_lines : 1,
                       &doom_font, colrngs[hudcolor_mesg],
                       &message_on, NULL);

  // create the secret message widget
  HUlib_init_multiline(&w_secret, 1,
                       &doom_font, colrngs[CR_GOLD],
                       &secret_on, NULL);

  // create the chat widget
  HUlib_init_multiline(&w_chat, 1,
                       &doom_font, colrngs[hudcolor_chat],
                       &chat_on, NULL);
  // [FG] only the chat widget draws a cursor
  w_chat.drawcursor = true;

  // create the inputbuffer widgets, one per player
  for (i = 0; i < MAXPLAYERS; i++)
  {
    HUlib_clear_line(&w_inputbuffer[i]);
  }

  //jff 2/16/98 added some HUD widgets
  // create the map title widget
  HUlib_init_multiline(&w_title, 1,
                       &doom_font, colrngs[hudcolor_titl],
                       &automapactive, HU_widget_build_title);
  // [FG] built only once right here
  w_title.builder();

  // create the hud health widget
  HUlib_init_multiline(&w_health, 1,
                       &boom_font, colrngs[CR_GREEN],
                       NULL, HU_widget_build_health);

  // create the hud armor widget
  HUlib_init_multiline(&w_armor, 1,
                       &boom_font, colrngs[CR_GREEN],
                       NULL, HU_widget_build_armor);

  // create the hud ammo widget
  HUlib_init_multiline(&w_ammo, 1,
                       &boom_font, colrngs[CR_GOLD],
                       NULL, HU_widget_build_ammo);

  // create the hud weapons widget
  HUlib_init_multiline(&w_weapon, inter ? 3 : 1,
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, HU_widget_build_weapon);

  // create the hud keys widget
  HUlib_init_multiline(&w_keys, 1,
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, deathmatch ? HU_widget_build_frag : HU_widget_build_keys);

  // create the hud monster/secret widget
  HUlib_init_multiline(&w_monsec,
                       // [Nugget] NUGHUD
                       ((hud_threelined_widgets && !st_crispyhud)
                        || (st_crispyhud && nughud.sts_ml))
                       ? 3 : 1,
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, HU_widget_build_monsec);

  HUlib_init_multiline(&w_sttime, 1,
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, HU_widget_build_sttime);

  // [Nugget] Powerup timers
  HUlib_init_multiline(&w_powers, 1, // TO-DO: Multi-lined?
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, HU_widget_build_powers);

  // [Cherry] Attempt counter
  HUlib_init_multiline(&w_attempts, 1,
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, HU_widget_build_attempts);

  // [Cherry] Movement widget
  HUlib_init_multiline(&w_move, 1,
                       &boom_font, colrngs[hudcolor_wg_name],
                       NULL, HU_widget_build_move);

  // create the automaps coordinate widget
  HUlib_init_multiline(&w_coord,
                       // [Nugget] NUGHUD
                       ((hud_threelined_widgets && !st_crispyhud)
                        || (st_crispyhud && nughud.coord_ml))
                       ? 3 : 1,
                       &boom_font, colrngs[hudcolor_xyco],
                       NULL, HU_widget_build_coord);

  HUlib_init_multiline(&w_fps, 1,
                       &boom_font, colrngs[hudcolor_xyco],
                       NULL, HU_widget_build_fps);

  HU_set_centered_message();

  // [Nugget] NUGHUD
  if (st_crispyhud) {
    hu_widget_t    *w;
    hu_multiline_t *m;
    nughud_alignable_t *na;

    w = doom_widgets[NUGHUDSLOT];
    while ((m = w->multiline)) {
      na = NULL;
      if      (m == &w_title)   { na = &nughud.title;   } 
      else if (m == &w_message) { na = &nughud.message; }
      else if (m == &w_chat)    { na = &nughud.message; }
      else if (m == &w_secret)  { na = &nughud.secret;  }

      if (na) {
        if (m == &w_chat) {
          w->x = 2 - WIDESCREENDELTA;
          w->y = na->y + ((*m->font)->line_height * (message_list ? hud_msg_lines : 1));
        }
        else
        { NughudAlignWidget(na, w); }
      }

      w++;
    }

    w = boom_widgets[NUGHUDSLOT];
    while ((m = w->multiline)) {
      na = NULL;
      if      (m == &w_sttime)   { na = &nughud.time;     }
      else if (m == &w_monsec)   { na = &nughud.sts;      }
      else if (m == &w_powers)   { na = &nughud.powers;   }
      else if (m == &w_coord)    { na = &nughud.coord;    }
      else if (m == &w_fps)      { na = &nughud.fps;      }
      else if (m == &w_attempts) { na = &nughud.attempts; }
      else if (m == &w_move)     { na = &nughud.movement; }

      if (na) {
        if (na == &nughud.time) { HU_NughudAlignTime(); }
        else                    { NughudAlignWidget(na, w); }
      }

      w++;
    }
  }

  HU_disable_all_widgets();
  HUlib_set_margins();

  // init crosshair
  if (hud_crosshair_on) // [Nugget] Use crosshair toggle
    HU_StartCrosshair();

  // now allow the heads-up display to run
  headsupactive = true;
}

static void HU_widget_build_title (void)
{
  char hud_titlestr[HU_MAXLINELENGTH] = "";
  char *s, *n;

  if (gamemapinfo && gamemapinfo->levelname)
  {
    if (gamemapinfo->label)
      s = gamemapinfo->label;
    else
      s = gamemapinfo->mapname;

    if (s == gamemapinfo->mapname || U_CheckField(s))
    {
      M_snprintf(hud_titlestr, sizeof(hud_titlestr), "%s: ", s);
    }
    s = gamemapinfo->levelname;
  }
  else if (gamestate == GS_LEVEL)
  {
    if (VANILLAMAP(gameepisode, gamemap))
    {
      s = (gamemode != commercial) ? HU_TITLE :
          (gamemission == pack_tnt) ? HU_TITLET :
          (gamemission == pack_plut) ? HU_TITLEP :
          HU_TITLE2;
    }
    // WADs like pl2.wad have a MAP33, and rely on the layout in the
    // Vanilla executable, where it is possible to overflow the end of one
    // array into the next.
    else if (gamemode == commercial && gamemap >= 33 && gamemap <= 35)
    {
      s = (gamemission == doom2) ? (*mapnamesp[gamemap-33]) :
          (gamemission == pack_plut) ? (*mapnamest[gamemap-33]) : "";
    }
    else
    {
      // initialize the map title widget with the generic map lump name
      s = MAPNAME(gameepisode, gamemap);
    }
  }
  else
  {
    s = "";
  }

  // [FG] cap at line break
  if ((n = strchr(s, '\n')))
  {
    *n = '\0';
  }

  M_StringConcat(hud_titlestr, s, sizeof(hud_titlestr));

  HUlib_add_string_to_cur_line(&w_title, hud_titlestr);
}

// do the hud ammo display
static void HU_widget_build_ammo (void)
{
  char hud_ammostr[HU_MAXLINELENGTH] = "AMM ";
  int fullammo = plr->maxammo[weaponinfo[plr->readyweapon].ammo];
  int i = 4;

  // special case for weapon with no ammo selected - blank bargraph + N/A
  if (weaponinfo[plr->readyweapon].ammo == am_noammo || fullammo == 0)
  {
    if (hud_draw_bargraphs)
    {
      strcat(hud_ammostr, "\x7f\x7f\x7f\x7f\x7f\x7f\x7f");
    }
    strcat(hud_ammostr, "N/A");
    w_ammo.cr = colrngs[CR_GRAY];
  }
  else
  {
    int ammo = plr->ammo[weaponinfo[plr->readyweapon].ammo];
    int ammopct = (100 * ammo) / fullammo;
    int ammobars = ammopct / 4;

    // build the bargraph string
    if (hud_draw_bargraphs)
    {
      // full bargraph chars
      for (i = 4; i < 4 + ammobars / 4;)
        hud_ammostr[i++] = 123;

      // plus one last character with 0, 1, 2, 3 bars
      switch (ammobars % 4)
      {
        case 0:
          break;
        case 1:
          hud_ammostr[i++] = 126;
          break;
        case 2:
          hud_ammostr[i++] = 125;
          break;
        case 3:
          hud_ammostr[i++] = 124;
          break;
      }

      // pad string with blank bar characters
      while (i < 4 + 7)
        hud_ammostr[i++] = 127;
      hud_ammostr[i] = '\0';
    }

    // build the numeric amount init string
    sprintf(hud_ammostr + i, "%3d/%3d", ammo, fullammo);

    // backpack changes thresholds (ammo widget)
    if (plr->backpack && !hud_backpack_thresholds && fullammo)
      ammopct = (100 * ammo) / (fullammo / 2);

    // set the display color from the percentage of total ammo held
    if (plr->cheats & CF_INFAMMO) // [Nugget] Make it gray if the player has infinite ammo
      w_ammo.cr = colrngs[CR_GRAY];
    else
    if (ammopct < ammo_red)
      w_ammo.cr = colrngs[hudcolor_th_low];
    else if (ammopct < ammo_yellow)
      w_ammo.cr = colrngs[hudcolor_th_ok];
    else if (ammopct > 100) // more than max threshold w/o backpack
      w_ammo.cr = colrngs[hudcolor_th_extra];
    else
      w_ammo.cr = colrngs[hudcolor_th_good];
  }

  // transfer the init string to the widget
  HUlib_add_string_to_cur_line(&w_ammo, hud_ammostr);
}

// do the hud health display
static void HU_widget_build_health (void)
{
  const boolean inter = gamestate == GS_INTERMISSION; // [Cherry]
  char hud_healthstr[HU_MAXLINELENGTH] = "HEL ";
  int i = 4;
  int healthbars = (st_health > 100) ? 25 : (st_health / 4);

  // build the bargraph string
  if (!inter && hud_draw_bargraphs)
  {
    // full bargraph chars
    for (i = 4; i < 4 + healthbars / 4;)
      hud_healthstr[i++] = 123;

    // plus one last character with 0, 1, 2, 3 bars
    switch (healthbars % 4)
    {
      case 0:
        break;
      case 1:
        hud_healthstr[i++] = 126;
        break;
      case 2:
        hud_healthstr[i++] = 125;
        break;
      case 3:
        hud_healthstr[i++] = 124;
        break;
    }

    // pad string with blank bar characters
    while (i < 4 + 7)
      hud_healthstr[i++] = 127;
    hud_healthstr[i] = '\0';
  }

  // [Cherry] don't pad with spaces on intermission screen
  // build the numeric amount init string
  sprintf(hud_healthstr + i, inter ? "%d%%" : "%3d%%", st_health);

  // set the display color from the amount of health posessed
  w_health.cr = ColorByHealth(plr->health, 100, st_invul);

  // transfer the init string to the widget
  HUlib_add_string_to_cur_line(&w_health, hud_healthstr);
}

// do the hud armor display
static void HU_widget_build_armor (void)
{
  const boolean inter = gamestate == GS_INTERMISSION; // [Cherry]
  char hud_armorstr[HU_MAXLINELENGTH] = "ARM ";
  int i = 4;
  int armorbars = (st_armor > 100) ? 25 : (st_armor / 4);

  // build the bargraph string
  if (!inter && hud_draw_bargraphs)
  {
    // full bargraph chars
    for (i = 4; i < 4 + armorbars / 4;)
      hud_armorstr[i++] = 123;

    // plus one last character with 0, 1, 2, 3 bars
    switch (armorbars % 4)
    {
      case 0:
        break;
      case 1:
        hud_armorstr[i++] = 126;
        break;
      case 2:
        hud_armorstr[i++] = 125;
        break;
      case 3:
        hud_armorstr[i++] = 124;
        break;
    }

    // pad string with blank bar characters
    while (i < 4 + 7)
      hud_armorstr[i++] = 127;
    hud_armorstr[i] = '\0';
  }

  // build the numeric amount init string
  // [Cherry] don't pad with spaces on intermission screen
  sprintf(hud_armorstr + i, inter ? "%d%%" : "%3d%%", st_armor);

  // color of armor depends on type
  if (hud_armor_type)
  {
    w_armor.cr =
      // [Nugget] Make it gray ONLY if the player is in God Mode
      (plr->cheats & CF_GODMODE) ? colrngs[CR_GRAY] :
      (plr->armortype == 0) ? colrngs[CR_RED] :
      (plr->armortype == 1) ? colrngs[CR_GREEN] :
      colrngs[CR_BLUE];
  }
  else
  {
    int armor = plr->armorpoints;
    
    // set the display color from the amount of armor posessed
    w_armor.cr =
      // [Nugget] Make it gray ONLY if the player is in God Mode
      (plr->cheats & CF_GODMODE) ? colrngs[CR_GRAY] :
      (armor < armor_red) ? colrngs[hudcolor_th_low] :
      (armor < armor_yellow) ? colrngs[hudcolor_th_ok] :
      (armor <= armor_green) ? colrngs[hudcolor_th_good] :
      colrngs[hudcolor_th_extra];
  }

  // transfer the init string to the widget
  HUlib_add_string_to_cur_line(&w_armor, hud_armorstr);
}

// do the hud weapon display
static void HU_widget_build_weapon (void)
{
  const boolean inter = gamestate == GS_INTERMISSION; // [Cherry]
  char hud_weapstr[HU_MAXLINELENGTH] = "WEA ";
  int i = inter ? 0 : 4, w, ammo, fullammo, ammopct;
  boolean newlined = false;

  // do each weapon that exists in current gamemode
  for (w = 0; w <= wp_supershotgun; w++) //jff 3/4/98 show fists too, why not?
  {
    int ok = 1;

    //jff avoid executing for weapons that do not exist
    switch (gamemode)
    {
      case shareware:
        if (w >= wp_plasma && w != wp_chainsaw)
          ok = 0;
        break;
      case retail:
      case registered:
        if (w >= wp_supershotgun && !have_ssg)
          ok = 0;
        break;
      default:
      case commercial:
        break;
    }
    if (!ok)
      continue;

    ammo = plr->ammo[weaponinfo[w].ammo];
    fullammo = plr->maxammo[weaponinfo[w].ammo];
    ammopct = 0;

    // skip weapons not currently posessed
    if (!plr->weaponowned[w])
      continue;

    // backpack changes thresholds (weapon widget)
    if (plr->backpack && !hud_backpack_thresholds)
      fullammo /= 2;

    ammopct = fullammo ? (100 * ammo) / fullammo : 100;

    // display each weapon number in a color related to the ammo for it
    hud_weapstr[i++] = '\x1b'; //jff 3/26/98 use ESC not '\' for paths
    if (weaponinfo[w].ammo == am_noammo) //jff 3/14/98 show berserk on HUD
      hud_weapstr[i++] = (w == wp_fist && !plr->powers[pw_strength]) ? '0'+CR_GRAY : '0'+hudcolor_th_good;
    else if (ammopct < ammo_red)
      hud_weapstr[i++] = '0'+hudcolor_th_low;
    else if (ammopct < ammo_yellow)
      hud_weapstr[i++] = '0'+hudcolor_th_ok;
    else if (ammopct > 100) // more than max threshold w/o backpack
      hud_weapstr[i++] = '0'+hudcolor_th_extra;
    else
      hud_weapstr[i++] = '0'+hudcolor_th_good;

    hud_weapstr[i++] = '0'+w+1;
    // [Cherry] new line each 3 weapons if on intermission screen
    if (inter && !((i + 1) % 12)) {
      hud_weapstr[i] = '\0';
      HUlib_add_string_to_cur_line(&w_weapon, hud_weapstr);
      i = 0;
      newlined = true;
    }
    else {
      hud_weapstr[i++] = ' ';
      hud_weapstr[i] = '\0';
      newlined = false;
    }
  }

  // transfer the init string to the widget
  if (!newlined)
    HUlib_add_string_to_cur_line(&w_weapon, hud_weapstr);
}

static void HU_widget_build_keys (void)
{
  char hud_keysstr[HU_MAXLINELENGTH] = { 'K', 'E', 'Y', '\x1b', '0'+CR_NONE, ' ', '\0' };
  int i = 6, k;

  // build text string whose characters call out graphic keys
  for (k = 0; k < 6; k++)
  {
    // skip keys not possessed
    if (!plr->cards[k])
      continue;

    hud_keysstr[i++] = HU_FONTEND + k + 1; // key number plus HU_FONTEND is char for key
    hud_keysstr[i++] = ' ';   // spacing
    hud_keysstr[i++] = ' ';
  }
  hud_keysstr[i] = '\0';

  // transfer the built string (frags or key title) to the widget
  HUlib_add_string_to_cur_line(&w_keys, hud_keysstr);
}

static inline int HU_top (char *const fragstr, int i, const int idx1, const int top1)
{
  if (idx1 > -1)
  {
    char numbuf[32], *s;

    sprintf(numbuf, "%5d", top1);
    // make frag count in player's color via escape code

    fragstr[i++] = '\x1b'; //jff 3/26/98 use ESC not '\' for paths
    fragstr[i++] = '0' + plyrcoltran[idx1 & 3];
    s = numbuf;
    while (*s)
      fragstr[i++] = *s++;
  }
  return i;
}

static void HU_widget_build_frag (void)
{
  char hud_fragstr[HU_MAXLINELENGTH] = { 'F', 'R', 'G', '\x1b', '0'+CR_ORIG, ' ', '\0' };
  int i = 6, k;

  int top1 = -999, top2 = -999, top3 = -999, top4 = -999;
  int idx1 = -1, idx2 = -1, idx3 = -1, idx4 = -1;
  int fragcount, m;

  // scan thru players
  for (k = 0; k < MAXPLAYERS; k++)
  {
    // skip players not in game
    if (!playeringame[k])
      continue;

    fragcount = 0;

    // compute number of times they've fragged each player
    // minus number of times they've been fragged by them
    for (m = 0; m < MAXPLAYERS; m++)
    {
      if (!playeringame[m])
        continue;
      fragcount += (m != k) ? players[k].frags[m] : -players[k].frags[m];
    }

    // very primitive sort of frags to find top four
    if (fragcount > top1)
    {
      top4 = top3; top3 = top2; top2 = top1; top1 = fragcount;
      idx4 = idx3; idx3 = idx2; idx2 = idx1; idx1 = k;
    }
    else if (fragcount > top2)
    {
      top4 = top3; top3 = top2; top2 = fragcount;
      idx4 = idx3; idx3 = idx2; idx2 = k;
    }
    else if (fragcount > top3)
    {
      top4 = top3; top3 = fragcount;
      idx4 = idx3; idx3 = k;
    }
    else if (fragcount > top4)
    {
      top4 = fragcount;
      idx4 = k;
    }
  }

  // killough 11/98: replaced cut-and-pasted code with function

  // if the biggest number exists,
  // put it in the init string
  i = HU_top(hud_fragstr, i, idx1, top1);

  // if the second biggest number exists,
  // put it in the init string
  i = HU_top(hud_fragstr, i, idx2, top2);

  // if the third biggest number exists,
  // put it in the init string
  i = HU_top(hud_fragstr, i, idx3, top3);

  // if the fourth biggest number exists,
  // put it in the init string
  i = HU_top(hud_fragstr, i, idx4, top4);

  hud_fragstr[i] = '\0';

  // transfer the built string (frags or key title) to the widget
  HUlib_add_string_to_cur_line(&w_keys, hud_fragstr);
}

static void HU_widget_build_monsec(void)
{
  char hud_monsecstr[HU_MAXLINELENGTH];
  int i, playerscount;
  char kills_str[60];
  int offset = 0;

  int kills = 0, kills_color, kills_percent, kills_percent_color;
  int items = 0, items_color;
  int secrets = 0, secrets_color;

  for (i = 0, playerscount = 0; i < MAXPLAYERS; ++i)
  {
    int color = (i == displayplayer) ? '0'+CR_GRAY : '0'+CR_GREEN;
    if (playeringame[i])
    {
      if (playerscount == 0)
      {
        offset = sprintf(kills_str,
          "\x1b%c%d", color, players[i].killcount);
      }
      else
      {
        offset += sprintf(kills_str + offset,
          "\x1b%c+\x1b%c%d", '0'+CR_GREEN, color, players[i].killcount);
      }

      kills += players[i].killcount;
      items += players[i].itemcount;
      secrets += players[i].secretcount;
      ++playerscount;
    }
  }

  // [Nugget] Customizable stat colors

  // [Nugget] Smart Totals from So Doom
  kills_color = (kills - (smarttotals ? extrakills : extraspawns) >= totalkills) ? '0'+hudcolor_ms_comp : '0'+hudcolor_ms_incomp;
  kills_percent_color = (kills - (smarttotals ? extrakills : 0) >= totalkills) ? '0'+hudcolor_ms_comp : '0'+hudcolor_ms_incomp;
  kills_percent = (totalkills == 0) ? 100 : ((kills - (smarttotals ? extrakills : 0)) * 100 / totalkills);
  
  items_color = (items >= totalitems) ? '0'+hudcolor_ms_comp : '0'+hudcolor_ms_incomp;
  secrets_color = (secrets >= totalsecret) ? '0'+hudcolor_ms_comp : '0'+hudcolor_ms_incomp;

  if (playerscount > 1)
  {
    offset = sprintf(hud_monsecstr,
      "\x1b%cK %s \x1b%c%d/%d",
      '0'+CR_RED, kills_str, kills_color,
      kills - (smarttotals ? extrakills : 0), // [Nugget] Smart Totals from So Doom
      totalkills);
  }
  else
  {
    offset = sprintf(hud_monsecstr,
      "\x1b%cK \x1b%c%d/%d",
      '0'+CR_RED, kills_color,
      plr->killcount - (smarttotals ? extrakills : 0), // [Nugget] Smart Totals from So Doom
      totalkills);
  }

  if (extraspawns && !smarttotals) // [Nugget] Smart Totals from So Doom
  {
    offset += sprintf(hud_monsecstr + offset, "+%d", extraspawns);
  }

  if ((hud_threelined_widgets && !st_crispyhud) || (st_crispyhud && nughud.sts_ml)) // [Nugget] NUGHUD
  {
    sprintf(hud_monsecstr + offset, " \x1b%c%d%%", kills_percent_color, kills_percent);
    HUlib_add_string_to_cur_line(&w_monsec, hud_monsecstr);

    sprintf(hud_monsecstr, "\x1b%cI \x1b%c%d/%d", ('0'+CR_RED), items_color, items, totalitems);
    HUlib_add_string_to_cur_line(&w_monsec, hud_monsecstr);

    sprintf(hud_monsecstr, "\x1b%cS \x1b%c%d/%d", ('0'+CR_RED), secrets_color, secrets, totalsecret);
    HUlib_add_string_to_cur_line(&w_monsec, hud_monsecstr);
  }
  else
  {
    sprintf(hud_monsecstr + offset,
      " \x1b%c%d%% \x1b%cI \x1b%c%d/%d \x1b%cS \x1b%c%d/%d",
      kills_percent_color, kills_percent,
      '0'+CR_RED, items_color, items, totalitems,
      '0'+CR_RED, secrets_color, secrets, totalsecret);

    HUlib_add_string_to_cur_line(&w_monsec, hud_monsecstr);
  }
}

static void HU_widget_build_sttime(void)
{
  const boolean inter = gamestate == GS_INTERMISSION; // [Cherry]
  char hud_timestr[HU_MAXLINELENGTH/2];
  int offset = 0;
  extern int time_scale;

  // [Nugget] Add conditions; function may've been called due to event timers
  if (hud_level_time || (map_level_time && automapactive))
  {
    if (time_scale != 100)
    {
      offset += sprintf(hud_timestr + offset, "\x1b%c%d%% ",
              '0'+CR_BLUE, time_scale);
    }

    // [Cherry] don't print the total time if only one level was completed
    //          (for intermission screen)
    if (totalleveltimes && (!inter || totalleveltimes != (leveltime - leveltime % TICRATE)))
    {
      const int time = (totalleveltimes + (inter ? 0 : leveltime)) / TICRATE;

      offset += sprintf(hud_timestr + offset, "\x1b%c%d:%02d ",
              '0'+hudcolor_totaltime, time/60, time%60);
    }
    // [Nugget] Add to `offset`; delete tab, it'll be added later
    offset += sprintf(hud_timestr + offset, "\x1b%c%d:%05.2f",
      '0'+hudcolor_plain, leveltime/TICRATE/60, (float)(leveltime%(60*TICRATE))/TICRATE);

    // [Nugget] If event timers are enabled, add a space
    if (plr->eventtics) { offset += sprintf(hud_timestr + offset, " "); }
  }

  // [Nugget] Event timer
  if (plr->eventtics) {
    const int   type = plr->eventtype;
    const int   mins = plr->eventtime / (60 * TICRATE);
    const float secs = (float)(plr->eventtime % (60 * TICRATE)) / TICRATE;

    if (!plr->eventtics || gameaction == ga_completed)
    { plr->eventtics = plr->eventtype = plr->eventtime = 0; }

    offset += sprintf(hud_timestr + offset, "\x1b%c%c %02i:%05.02f",
                      '0'+CR_GOLD,
                      type == TIMER_KEYPICKUP ? 'K' : type == TIMER_TELEPORT ? 'T' : 'U',
                      mins, secs);
  }

  offset += sprintf(hud_timestr + offset, "\t"); // [Nugget] Add tab here

  HUlib_add_string_to_cur_line(&w_sttime, hud_timestr);
}

// [Nugget]
void HU_widget_rebuild_sttime(void)
{
  HU_widget_build_sttime();
}

// [Nugget] Powerup timers
static void HU_widget_build_powers(void)
{
  char hud_powerstr[48];
  int offset = 0;

  // TO-DO: Multi-lined?

  if (plr->powers[pw_invisibility] > 0) {
    offset += sprintf(hud_powerstr, "\x1b%cINVIS %i\" ",
                      '0' + ((plr->powers[pw_invisibility] > 4*32 || plr->powers[pw_invisibility] & 8) ? CR_RED : CR_BLACK),
                      MIN(INVISTICS/TICRATE, 1 + (plr->powers[pw_invisibility] / TICRATE)));
  }

  if (plr->powers[pw_invulnerability] > 0) {
    offset += sprintf(hud_powerstr + offset, "\x1b%cINVUL %i\" ",
                      '0' + ((plr->powers[pw_invulnerability] > 4*32 || plr->powers[pw_invulnerability] & 8) ? CR_GREEN : CR_BLACK),
                      MIN(INVULNTICS/TICRATE, 1 + (plr->powers[pw_invulnerability] / TICRATE)));
  }

  if (plr->powers[pw_infrared] > 0) {
    offset += sprintf(hud_powerstr + offset, "\x1b%cLIGHT %i\" ",
                      '0' + ((plr->powers[pw_infrared] > 4*32 || plr->powers[pw_infrared] & 8) ? CR_BRICK : CR_BLACK),
                      MIN(INFRATICS/TICRATE, 1 + (plr->powers[pw_infrared] / TICRATE)));
  }

  if (plr->powers[pw_ironfeet] > 0) {
    offset += sprintf(hud_powerstr + offset, "\x1b%cSUIT %i\"",
                      '0' + ((plr->powers[pw_ironfeet] > 4*32 || plr->powers[pw_ironfeet] & 8) ? CR_GRAY : CR_BLACK),
                      MIN(IRONTICS/TICRATE, 1 + (plr->powers[pw_ironfeet] / TICRATE)));
  }

  
  if (offset) { HUlib_add_string_to_cur_line(&w_powers, hud_powerstr); }
  else        { HUlib_clear_cur_line(&w_powers); }
}

// [Cherry] Attempt counter
static void HU_widget_build_attempts(void)
{
  const boolean inter = gamestate == GS_INTERMISSION;
  char hud_attemptstr[HU_MAXLINELENGTH] = "ATT ";
  int offset = 4;

  if (sessionattempts != -1)
  {
    offset += sprintf(hud_attemptstr + offset, "\x1b%c%d", '0'+hudcolor_plain, sessionattempts);
    if (!inter)
    {
      offset += sprintf(hud_attemptstr + offset, "/%d", totalattempts);
    }
  }

  if (offset > 4)
  {
    HUlib_add_string_to_cur_line(&w_attempts, hud_attemptstr);
  }
}

// [Cherry] Movement widget
static void HU_widget_build_move(void)
{
  char hud_movestr[HU_MAXLINELENGTH] = "MOVE ";
  int offset = 5;

  sprintf(hud_movestr + offset, "\x1b%c0", '0'+CR_RED);

  if (plr->cmd.forwardmove)
  {
    const int move = plr->cmd.forwardmove;
    offset += sprintf(hud_movestr + offset, "\x1b%c",
                      '0'+(abs(move) < 26 ? hudcolor_th_ok :
                           abs(move) < 50 ? hudcolor_th_good : hudcolor_th_extra));

    hud_movestr[offset++] = 'M';
    hud_movestr[offset++] = move > 0 ? 'F' : 'B';
    offset += sprintf(hud_movestr + offset, "%d ", abs(move));
  }

  if (plr->cmd.sidemove)
  {
    const int move = plr->cmd.sidemove;
    offset += sprintf(hud_movestr + offset, "\x1b%c",
                      '0'+(abs(move) < 25 ? hudcolor_th_ok :
                           abs(move) < 50 ? hudcolor_th_good : hudcolor_th_extra));

    hud_movestr[offset++] = 'S';
    hud_movestr[offset++] = move > 0 ? 'R' : 'L';
    offset += sprintf(hud_movestr + offset, "%d", abs(move));
  }

  HUlib_add_string_to_cur_line(&w_move, hud_movestr);
}

static void HU_widget_build_coord (void)
{
  char hud_coordstr[HU_MAXLINELENGTH];
  fixed_t x,y,z; // killough 10/98:
  void AM_Coordinates(const mobj_t *, fixed_t *, fixed_t *, fixed_t *);

  // killough 10/98: allow coordinates to display non-following pointer
  AM_Coordinates(plr->mo, &x, &y, &z);

  //jff 2/16/98 output new coord display
  if ((hud_threelined_widgets && !st_crispyhud) || (st_crispyhud && nughud.coord_ml)) // [Nugget] NUGHUD
  {
    sprintf(hud_coordstr, "X \x1b%c%d", '0'+CR_GRAY, x >> FRACBITS);
    HUlib_add_string_to_cur_line(&w_coord, hud_coordstr);

    sprintf(hud_coordstr, "Y \x1b%c%d", '0'+CR_GRAY, y >> FRACBITS);
    HUlib_add_string_to_cur_line(&w_coord, hud_coordstr);

    sprintf(hud_coordstr, "Z \x1b%c%d", '0'+CR_GRAY, z >> FRACBITS);
    HUlib_add_string_to_cur_line(&w_coord, hud_coordstr);
  }
  else
  {
    sprintf(hud_coordstr, "X \x1b%c%d \x1b%cY \x1b%c%d \x1b%cZ \x1b%c%d",
            '0'+hudcolor_plain, x >> FRACBITS, '0'+hudcolor_xyco,
            '0'+hudcolor_plain, y >> FRACBITS, '0'+hudcolor_xyco,
            '0'+hudcolor_plain, z >> FRACBITS);

    HUlib_add_string_to_cur_line(&w_coord, hud_coordstr);
  }
}

static void HU_widget_build_fps (void)
{
  char hud_fpsstr[HU_MAXLINELENGTH/4];
  extern int fps;

  sprintf(hud_fpsstr,"\x1b%c%d \x1b%cFPS", '0'+CR_GRAY, fps, '0'+CR_ORIG);
  HUlib_add_string_to_cur_line(&w_fps, hud_fpsstr);
}

// Crosshair

int hud_crosshair; // [Nugget] Crosshair type to be used
boolean hud_crosshair_health;
crosstarget_t hud_crosshair_target;
crosslockon_t hud_crosshair_lockon;
boolean hud_crosshair_indicators; // [Nugget] Horizontal autoaim indicators
boolean hud_crosshair_fuzzy; // [Nugget] Account for fuzzy targets
int hud_crosshair_color;
int hud_crosshair_target_color;

typedef struct
{
  patch_t *patch;
  patch_t *patchl, *patchr; // [Nugget] Horizontal autoaim indicators
  int lw, lh, rw, rh;
  int side; // [Nugget] Horizontal autoaim indicators
  int w, h, x, y;
  char *cr;
} crosshair_t;

static crosshair_t crosshair;

const char *crosshair_nam[HU_CROSSHAIRS] =
  { NULL,
    "CROSS00", "CROSS01", "CROSS02", "CROSS03",
    "CROSS04", "CROSS05", "CROSS06", "CROSS07",
    "CROSS08" };
const char *crosshair_str[HU_CROSSHAIRS+1] =
  { "none",
    "Cross", "Angle", "Dot", "Big Cross",
    "Circle", "Big Circle", "Chevron", "Chevrons",
    "Arcs", NULL };

static void HU_InitCrosshair(void)
{
  int i, j;

  for (i = 1; i < HU_CROSSHAIRS; i++)
  {
    j = W_CheckNumForName(crosshair_nam[i]);
    if (j >= num_predefined_lumps)
    {
      if (R_IsPatchLump(j))
        crosshair_str[i] = crosshair_nam[i];
      else
        crosshair_nam[i] = NULL;
    }
  }
}

void HU_StartCrosshair(void) // [Nugget] Not static anymore
{
  if (crosshair.patch)
    Z_ChangeTag(crosshair.patch, PU_CACHE);

  // [Nugget] Horizontal autoaim indicators
  if (crosshair.patchl) { Z_ChangeTag(crosshair.patchl, PU_CACHE); }
  if (crosshair.patchr) { Z_ChangeTag(crosshair.patchr, PU_CACHE); }

  if (crosshair_nam[hud_crosshair])
  {
    crosshair.patch = W_CacheLumpName(crosshair_nam[hud_crosshair], PU_STATIC);

    crosshair.w = SHORT(crosshair.patch->width)/2;
    crosshair.h = SHORT(crosshair.patch->height)/2;
  }
  else
    crosshair.patch = NULL;

  // [Nugget] Horizontal autoaim indicators
  crosshair.patchl = W_CacheLumpName("CROSSIL", PU_STATIC);
  crosshair.lw = SHORT(crosshair.patchl->width);
  crosshair.lh = SHORT(crosshair.patchl->height)/2;
  crosshair.patchr = W_CacheLumpName("CROSSIR", PU_STATIC);
  crosshair.rw = SHORT(crosshair.patchr->width);
  crosshair.rh = SHORT(crosshair.patchr->height)/2;
}

mobj_t *crosshair_target; // [Alaux] Lock crosshair on target

static void HU_UpdateCrosshair(void)
{
  crosshair.x = ORIGWIDTH/2;
  crosshair.y = (screenblocks <= 10) ? (ORIGHEIGHT-ST_HEIGHT)/2 : ORIGHEIGHT/2;
  crosshair.side = 0;

  if (STRICTMODE(hud_crosshair_target || hud_crosshair_lockon))
  {
    angle_t an = plr->mo->angle;
    const ammotype_t ammo = weaponinfo[plr->readyweapon].ammo;
    const fixed_t range = (ammo == am_noammo
                           && !(plr->readyweapon == wp_fist && plr->cheats & CF_SAITAMA))
                          ? MELEERANGE : 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1);
    boolean intercepts_overflow_enabled = overflow[emu_intercepts].enabled;

    crosshair_target = linetarget = NULL;

    overflow[emu_intercepts].enabled = false;
    P_AimLineAttack(plr->mo, an, range, 0);
    if ((ammo == am_misl || ammo == am_cell)
        && (!no_hor_autoaim || !casual_play)) // [Nugget]
    {
      if (!linetarget) {
        P_AimLineAttack(plr->mo, an += 1<<26, range, 0);
        if (linetarget && hud_crosshair_indicators) { crosshair.side = -1; } // [Nugget]
      }
      if (!linetarget) {
        P_AimLineAttack(plr->mo, an -= 2<<26, range, 0);
        if (linetarget && hud_crosshair_indicators) { crosshair.side = 1; } // [Nugget]
      }
    }
    overflow[emu_intercepts].enabled = intercepts_overflow_enabled;
    
    crosshair_target = linetarget;
  }

  // [Nugget] Begin checking, in order of priority

  if (hud_crosshair_target && crosshair_target
      && (!(crosshair_target->flags & MF_SHADOW) || hud_crosshair_fuzzy)) // [Nugget]
  {
    // [Alaux] Color crosshair by target health
    if (hud_crosshair_target == crosstarget_health)
    {
      const int spawnhealth = linetarget->info->spawnhealth;
      const int health = linetarget->health;

      if      (health < spawnhealth*0.25) { crosshair.cr = colrngs[CR_RED];   }
      else if (health < spawnhealth*0.50) { crosshair.cr = colrngs[CR_BRICK]; }
      else if (health < spawnhealth*0.75) { crosshair.cr = colrngs[CR_GOLD];  }
      else                                { crosshair.cr = colrngs[CR_GREEN]; }
    }
    else
    {
      crosshair.cr = colrngs[hud_crosshair_target_color];
    }
  }
  else
  if (hud_crosshair_health)
    crosshair.cr = ColorByHealth(plr->health, 100, st_invul);
  else
    crosshair.cr = colrngs[hud_crosshair_color];
}

void HU_UpdateCrosshairLock(int x, int y)
{
  int w = (crosshair.w << hires);
  int h = (crosshair.h << hires);

  x = viewwindowx + BETWEEN(w, viewwidth  - w - 1, x);
  y = viewwindowy + BETWEEN(h, viewheight - h - 1, y);

  if (hud_crosshair_lockon == crosslockon_full) // [Nugget]
  { crosshair.x = (x >> hires) - WIDESCREENDELTA; }
  crosshair.y = (y >> hires);
}

void HU_DrawCrosshair(void)
{
  // [Nugget] Change conditions
  if (plr->playerstate != PST_LIVE
      || automapactive
  /* || menuactive
      || paused
      || secret_on */
      // [Nugget]
      || !crosshair.cr
      || (chasecam_mode && !chasecam_crosshair)
     )
  {
    return;
  }

  if (crosshair.patch)
    V_DrawPatchTranslated(crosshair.x - crosshair.w,
                          crosshair.y - crosshair.h,
                          0, crosshair.patch, crosshair.cr);

  // [Nugget] Horizontal autoaim indicators
  if (crosshair.side == -1)
    V_DrawPatchTranslated(crosshair.x - crosshair.w - crosshair.lw,
                          crosshair.y - crosshair.lh,
                          0, crosshair.patchl, crosshair.cr);
  else if (crosshair.side == 1)
    V_DrawPatchTranslated(crosshair.x + crosshair.w,
                          crosshair.y - crosshair.rh,
                          0, crosshair.patchr, crosshair.cr);
}

// [crispy] print a bar indicating demo progress at the bottom of the screen
boolean HU_DemoProgressBar(boolean force)
{
  const int progress = SCREENWIDTH * playback_tic / playback_totaltics;
  static int old_progress = 0;

  if (old_progress < progress)
  {
    old_progress = progress;
  }
  else if (!force)
  {
    return false;
  }

  V_DrawHorizLine(0, SCREENHEIGHT - 2, 0, progress, darkest_color);
  V_DrawHorizLine(0, SCREENHEIGHT - 1, 0, progress, lightest_color);

  return true;
}

// [FG] level stats and level time widgets
int map_player_coords, map_level_stats, map_level_time;
int hud_level_stats, hud_level_time;
int map_power_timers, hud_power_timers; // [Nugget] Powerup timers
int hud_attempt_counter, map_attempt_counter; // [Cherry] Attempt counter
int hud_movement, map_movement; // [Cherry] Movement widget

// [Cherry]
// HU_UpdateWidgetFont
//
void HU_UpdateWidgetFont(void)
{
  if ((automapactive && hud_widget_font == 1) || hud_widget_font == 2)
  {
    boom_font = &big_font;
    CR_BLUE = CR_BLUE2;
  }
  else
  {
    boom_font = &sml_font;
    CR_BLUE = CR_BLUE1;
  }
}

//
// HU_Drawer()
//
// Draw all the pieces of the heads-up display
//
// Passed nothing, returns nothing
//
void HU_Drawer(void)
{
  hu_widget_t *w;

  HUlib_reset_align_offsets();

  // jff 4/24/98 Erase current lines before drawing current
  // needed when screen not fullsize
  // killough 11/98: only do it when not fullsize
  // moved here to properly update the w_sttime and w_monsec widgets
  if (scaledviewheight < 200)
  {
    HU_Erase();
  }

  w = doom_widget;
  while (w->multiline)
  {
    if (*w->multiline->on)
    {
      HUlib_draw_widget(w);
    }
    w++;
  }

  // [Nugget] Removed "draw_crispy_hud" check

  w = boom_widget;
  while (w->multiline)
  {
    if (w->multiline->built)
    {
      HUlib_draw_widget(w);
    }
    w++;
  }
}

// [FG] draw Time widget on intermission screen
void WI_DrawTimeWidget(void)
{
  hu_widget_t w = {&w_sttime, align_left, align_top};

  if (hud_level_time)
  {
    HUlib_reset_align_offsets();
    // [Cherry] allowed the font to change during intermission so an update is necessary
    HU_widget_build_sttime();
    HUlib_draw_widget(&w);
  }
}

int wi_more_widgets;

// [Cherry] Draw attempts, health, armor and weapons on intermission screen
void WI_DrawMoreWidgets(void)
{
  hu_widget_t w1 = { &w_attempts, align_left, align_top };
  hu_widget_t w2 = { &w_health, align_left, align_top };
  hu_widget_t w3 = { &w_armor, align_left, align_top };
  hu_widget_t w4 = { &w_weapon, align_left, align_top };

  if (wi_more_widgets)
  {
    HUlib_draw_widget(&w1);
    HUlib_draw_widget(&w2);
    HUlib_draw_widget(&w3);
    HUlib_draw_widget(&w4);
  }
}

// [Cherry]
void WI_BuildMoreWidgets(void)
{
  HU_widget_build_attempts();
  HU_widget_build_health();
  HU_widget_build_armor();
  HU_widget_build_weapon();
}

//
// HU_Erase()
//
// Erase hud display lines that can be trashed by small screen display
//
// Passed nothing, returns nothing
//

void HU_Erase(void)
{
  // [FG] TODO optimize!
  if (!automapactive && viewwindowx)
    R_DrawViewBorder();
}

//
// HU_Ticker()
//
// Update the hud displays once per frame
//
// Passed nothing, returns nothing
//

static boolean bsdown; // Is backspace down?
static int bscounter;

int M_StringWidth(char *string);

void HU_Ticker(void)
{
  doom_widget = doom_widgets[st_crispyhud ? NUGHUDSLOT : hud_active]; // [Nugget] NUGHUD
  boom_widget = boom_widgets[st_crispyhud ? NUGHUDSLOT : hud_active]; // [Nugget] NUGHUD
  plr = &players[displayplayer];         // killough 3/7/98

  HU_disable_all_widgets();
  // [Nugget] Removed "draw_crispy_hud" code

  HU_UpdateWidgetFont();

  // wait a few tics before sending a backspace character
  if (bsdown && bscounter++ > 9)
  {
    HUlib_add_key_to_cur_line(&w_chat, KEY_BACKSPACE);
    bscounter = 8;
  }

  // tick down message counter if message is up
  if (message_counter && !--message_counter)
    message_on = message_nottobefuckedwith = false;

  if (secret_counter && !--secret_counter)
    secret_on = false;

  // [Woof!] "A secret is revealed!" message
  if (plr->secretmessage)
  {
    HUlib_add_string_to_cur_line(&w_secret, plr->secretmessage);
    plr->secretmessage = NULL;
    secret_on = true;
    secret_counter = 5*TICRATE/2; // [crispy] 2.5 seconds
  }

  // if messages on, or "Messages Off" is being displayed
  // this allows the notification of turning messages off to be seen
  // display message if necessary

  if ((showMessages || message_dontfuckwithme) && plr->message &&
      (!message_nottobefuckedwith || message_dontfuckwithme))
  {
    //post the message to the message widget
    HUlib_add_string_to_cur_line(&w_message, plr->message);

    // [FG] empty messages clear the whole widget
    if (plr->message[0] == '\0')
      HUlib_clear_all_lines(&w_message);

    // clear the message to avoid posting multiple times
    plr->message = 0;

    message_on = true;       // note a message is displayed
    // start the message persistence counter	      
    message_counter = message_count;

    has_message = true;        // killough 12/98

    // transfer "Messages Off" exception to the "being displayed" variable
    message_nottobefuckedwith = message_dontfuckwithme;

    // clear the flag that "Messages Off" is being posted
    message_dontfuckwithme = 0;
  }

  // check for incoming chat characters
  if (netgame)
  {
    int i, rc;
    char c;

    for (i = 0; i < MAXPLAYERS; i++)
    {
      if (!playeringame[i])
        continue;

      if (i != consoleplayer &&
          (c = players[i].cmd.chatchar))
      {
        if (c <= HU_BROADCAST)
          chat_dest[i] = c;
        else
        {
          if (c >= 'a' && c <= 'z')
            c = (char) shiftxform[(unsigned char) c];

          rc = HUlib_add_key_to_line(&w_inputbuffer[i], c);
          if (rc && c == KEY_ENTER)
          {
            if (w_inputbuffer[i].len &&
                (chat_dest[i] == consoleplayer + 1 ||
                chat_dest[i] == HU_BROADCAST))
            {
              HUlib_add_strings_to_cur_line(&w_message,
                                            *player_names[i],
                                            w_inputbuffer[i].line);

              has_message = true; // killough 12/98
              message_nottobefuckedwith = true;
              message_on = true;
              message_counter = chat_count; // killough 11/98
              S_StartSound(0, gamemode == commercial ?
                              sfx_radio : sfx_tink);
            }
            HUlib_clear_line(&w_inputbuffer[i]);
          }
        }
        players[i].cmd.chatchar = 0;
      }
    }
  }

  // draw the automap widgets if automap is displayed

  if (automapactive)
  {
    HU_cond_build_widget(&w_monsec, map_level_stats);
    HU_cond_build_widget(&w_attempts, map_attempt_counter); // [Cherry] Attempt counter
    HU_cond_build_widget(&w_move, map_movement); // [Cherry] Movement widget
    HU_cond_build_widget(&w_sttime, map_level_time || plr->eventtics); // [Nugget] Event timers
    HU_cond_build_widget(&w_powers, STRICTMODE(map_power_timers)); // [Nugget] Powerup timers
    HU_cond_build_widget(&w_coord, STRICTMODE(map_player_coords));
  }
  else
  {
    HU_cond_build_widget(&w_coord, STRICTMODE(map_player_coords) == 2);
  }

  HU_cond_build_widget(&w_fps, plr->cheats & CF_SHOWFPS);

  if (hud_displayed &&
      scaledviewheight == SCREENHEIGHT &&
      automap_off)
  {
    // [Nugget] Removed crispy_hud code
    {
      HU_cond_build_widget(&w_weapon, true);
      HU_cond_build_widget(&w_armor, true);
      HU_cond_build_widget(&w_health, true);
      HU_cond_build_widget(&w_ammo, true);
      HU_cond_build_widget(&w_keys, true);
    }

    HU_cond_build_widget(&w_monsec, hud_level_stats);
    HU_cond_build_widget(&w_attempts, hud_attempt_counter); // [Cherry] Attempt counter
    HU_cond_build_widget(&w_move, hud_movement); // [Cherry] Movement widget
    HU_cond_build_widget(&w_sttime, hud_level_time || plr->eventtics); // [Nugget] Event timers
    HU_cond_build_widget(&w_powers, STRICTMODE(hud_power_timers)); // [Nugget] Powerup timers
  }
  else if (scaledviewheight &&
           (scaledviewheight < SCREENHEIGHT || screenblocks < 13) && // [Nugget] Allow in NUGHUD
           automap_off)
  {
    HU_cond_build_widget(&w_monsec, hud_level_stats);
    HU_cond_build_widget(&w_attempts, hud_attempt_counter); // [Cherry] Attempt counter
    HU_cond_build_widget(&w_move, hud_movement); // [Cherry] Movement widget
    HU_cond_build_widget(&w_sttime, hud_level_time || plr->eventtics); // [Nugget] Event timers
    HU_cond_build_widget(&w_powers, hud_power_timers); // [Nugget] Powerup timers
  }

  // [Nugget] Event timers
  if (plr->eventtics) { plr->eventtics--; }

  // update crosshair properties
  if (hud_crosshair)
    HU_UpdateCrosshair();
}

#define QUEUESIZE   128

static char chatchars[QUEUESIZE];
static int  head = 0;
static int  tail = 0;

//
// HU_queueChatChar()
//
// Add an incoming character to the circular chat queue
//
// Passed the character to queue, returns nothing
//
void HU_queueChatChar(char c)
{
  if (((head + 1) & (QUEUESIZE-1)) == tail)
    displaymsg("%s", HUSTR_MSGU);
  else
    {
      chatchars[head++] = c;
      head &= QUEUESIZE-1;
    }
}

//
// HU_dequeueChatChar()
//
// Remove the earliest added character from the circular chat queue
//
// Passed nothing, returns the character dequeued
//
char HU_dequeueChatChar(void)
{
  char c;

  if (head != tail)
    {
      c = chatchars[tail++];
      tail &= QUEUESIZE-1;
    }
  else
    c = 0;
  return c;
}

//
// HU_Responder()
//
// Responds to input events that affect the heads up displays
//
// Passed the event to respond to, returns true if the event was handled
//

boolean HU_Responder(event_t *ev)
{
  static char   lastmessage[HU_MAXLINELENGTH+1];
  char*   macromessage;
  boolean   eatkey = false;
  static boolean  shiftdown = false;
  static boolean  altdown = false;
  int     c;
  int     i;
  int     numplayers;

  static int    num_nobrainers = 0;

  c = (ev->type == ev_keydown) ? ev->data1 : 0;

  numplayers = 0;
  for (i=0 ; i<MAXPLAYERS ; i++)
    numplayers += playeringame[i];

  if (ev->data1 == KEY_RSHIFT)
    {
      shiftdown = ev->type == ev_keydown;
      return false;
    }

  if (ev->data1 == KEY_RALT)
    {
      altdown = ev->type == ev_keydown;
      return false;
    }

  if (M_InputActivated(input_chat_backspace))
  {
    bsdown = true;
    bscounter = 0;
    c = KEY_BACKSPACE;
  }
  else if (M_InputDeactivated(input_chat_backspace))
  {
    bsdown = false;
    bscounter = 0;
  }

  if (ev->type == ev_keyup)
    return false;

  if (!chat_on)
    {
      if (M_InputActivated(input_chat_enter))                         // phares
        {
	  //jff 2/26/98 toggle list of messages

	  if (has_message)
	    {
		  message_counter = message_count;
		  message_on = true;
	    }
          eatkey = true;
        }  //jff 2/26/98 no chat if message review is displayed
      else // killough 10/02/98: no chat if demo playback
        if (!demoplayback)
          {
	    if ((netgame || STRICTMODE(sp_chat)) // [Nugget] Singleplayer chat
		&& M_InputActivated(input_chat))
	      {
		eatkey = chat_on = true;
		HUlib_clear_cur_line(&w_chat);
		HU_queueChatChar(HU_BROADCAST);
	      }//jff 2/26/98
	    else    // killough 11/98: simplify
	      if (netgame && numplayers > 2)
		for (i=0; i<MAXPLAYERS ; i++)
		  if (M_InputActivated(input_chat_dest0 + i))
		  {
		    if (i == consoleplayer)
		      displaymsg("%s", 
			++num_nobrainers <  3 ? HUSTR_TALKTOSELF1 :
	                  num_nobrainers <  6 ? HUSTR_TALKTOSELF2 :
	                  num_nobrainers <  9 ? HUSTR_TALKTOSELF3 :
	                  num_nobrainers < 32 ? HUSTR_TALKTOSELF4 :
                                                HUSTR_TALKTOSELF5 );
                  else
                    if (playeringame[i])
                      {
                        eatkey = chat_on = true;
                        HUlib_clear_cur_line(&w_chat);
                        HU_queueChatChar((char)(i+1));
                        break;
                      }
		  }
          }
    }//jff 2/26/98 no chat functions if message review is displayed
  else
      {
        if (M_InputActivated(input_chat_enter))
        {
          c = KEY_ENTER;
        }

        // send a macro
        if (altdown)
          {
            c = c - '0';
            if (c < 0 || c > 9)
              return false;
            // fprintf(stderr, "got here\n");
            macromessage = chat_macros[c];

            // kill last message with a '\n'
            HU_queueChatChar(KEY_ENTER); // DEBUG!!!                // phares

            // send the macro message
            while (*macromessage)
              HU_queueChatChar(*macromessage++);
            HU_queueChatChar(KEY_ENTER);                            // phares

            // leave chat mode and notify that it was sent
            chat_on = false;
            strcpy(lastmessage, chat_macros[c]);
            displaymsg("%s", lastmessage);
            eatkey = true;
          }
        else
          {
            if (shiftdown || (c >= 'a' && c <= 'z'))
              c = shiftxform[c];
            eatkey = HUlib_add_key_to_cur_line(&w_chat, c);
            if (eatkey)
              HU_queueChatChar(c);

            if (c == KEY_ENTER)                                     // phares
              {
                chat_on = false;
                if (w_chat.lines[0]->len)
                  {
                    strcpy(lastmessage, w_chat.lines[0]->line);
                    displaymsg("%s", lastmessage);
                  }
              }
            else
              if (c == KEY_ESCAPE)                               // phares
                chat_on = false;
          }
      }
  return eatkey;
}

// [FG] dynamic HUD alignment

typedef struct {
  const char *name, *altname;
  hu_multiline_t *const multiline;
} multiline_names_t;

static const multiline_names_t
  doom_names[] = {
    {"title",   NULL,     &w_title},
    {"message", NULL,     &w_message},
// [FG] TODO due to its variable width and the trailing cursor,
//      the w_chat widget *must* currently remain left-aligned
//  {"chat",    NULL,     &w_chat},
    {"secret",  NULL,     &w_secret},
    {NULL},
  },
  boom_names[] = {
    {"ammo",      NULL,     &w_ammo},
    {"armor",     NULL,     &w_armor},
    {"health",    NULL,     &w_health},
    {"keys",      NULL,     &w_keys},
    {"weapon",   "weapons", &w_weapon},

    {"monsec",   "stats",   &w_monsec},
    {"sttime",   "time",    &w_sttime},
    {"attempts",  NULL,     &w_attempts}, // [Cherry] Attempt counter
    {"movement", "move",    &w_move}, // [Cherry] Movement widget
    {"powers",    NULL,     &w_powers}, // [Nugget] Powerup Timers
    {"coord",    "coords",  &w_coord},
    {"fps",      "rate",    &w_fps},
    {NULL},
};

static boolean HU_ReplaceInDoomWidgets (hu_multiline_t *multiline, int hud, align_t h_align, align_t v_align, int x, int y)
{
  int i;

  if (hud < 0 || hud >= MAX_HUDS)
  {
    return false;
  }

  for (i = 0; i < MAX_WIDGETS - 1; i++)
  {
    if (doom_widgets[hud][i].multiline == multiline)
    {
      doom_widgets[hud][i].h_align = h_align;
      doom_widgets[hud][i].v_align = v_align;
      doom_widgets[hud][i].x = x;
      doom_widgets[hud][i].y = y;

      return true;
    }
  }

  return false;
}

static boolean HU_AddToBoomWidgets (hu_multiline_t *multiline, int hud, align_t h_align, align_t v_align, int x, int y)
{
  int i;

  if (hud < 0 || hud >= MAX_HUDS)
  {
    return false;
  }

  for (i = 0; i < MAX_WIDGETS - 1; i++)
  {
    if (boom_widgets[hud][i].multiline == NULL)
    {
      break;
    }
  }

  if (i + 1 >= MAX_WIDGETS)
  {
    return false;
  }

  boom_widgets[hud][i].multiline = multiline;
  boom_widgets[hud][i].h_align = h_align;
  boom_widgets[hud][i].v_align = v_align;
  boom_widgets[hud][i].x = x;
  boom_widgets[hud][i].y = y;

  boom_widgets[hud][i + 1].multiline = NULL;

  return true;
}

static boolean HU_AddToWidgets (hu_multiline_t *multiline, const multiline_names_t *names, int hud, align_t h_align, align_t v_align, int x, int y)
{
  if (names == doom_names)
  {
    return HU_ReplaceInDoomWidgets(multiline, hud, h_align, v_align, x, y);
  }
  else if (names == boom_names)
  {
    return HU_AddToBoomWidgets(multiline, hud, h_align, v_align, x, y);
  }

  return false;
}

static hu_multiline_t *HU_MultilineByName (const char *name, const multiline_names_t *names)
{
  int i;

  for (i = 0; names[i].name; i++)
  {
    if (strcasecmp(name, names[i].name) == 0 ||
       (names[i].altname && strcasecmp(name, names[i].altname) == 0))
    {
      return names[i].multiline;
    }
  }

  return NULL;
}

static boolean HU_AddHUDCoords (char *name, int hud, int x, int y)
{
  const multiline_names_t *names = doom_names;
  hu_multiline_t *multiline = HU_MultilineByName(name, names);

  if (multiline == NULL)
  {
    names = boom_names;
    multiline = HU_MultilineByName(name, names);
  }

  if (multiline == NULL)
  {
    return false;
  }

  // [FG] relative alignment to the edges
  if (x < 0)
  {
    x += ORIGWIDTH;
  }
  if (y < 0)
  {
    y += ORIGHEIGHT;
  }

  if (x < 0 || x >= ORIGWIDTH || y < 0 || y >= ORIGHEIGHT)
  {
    return false;
  }

  return HU_AddToWidgets(multiline, names, hud, align_direct, align_direct, x, y);
}

static boolean HU_AddHUDAlignment (char *name, int hud, char *alignstr)
{
  const multiline_names_t *names = doom_names;
  hu_multiline_t *multiline = HU_MultilineByName(name, names);

  if (multiline == NULL)
  {
    names = boom_names;
    multiline = HU_MultilineByName(name, names);
  }

  if (multiline == NULL)
  {
    return false;
  }

  if (!strcasecmp(alignstr, "topleft")          || !strcasecmp(alignstr, "upperleft"))
  {
    return HU_AddToWidgets(multiline, names, hud, align_left, align_top, 0, 0);
  }
  else if (!strcasecmp(alignstr, "topright")    || !strcasecmp(alignstr, "upperright"))
  {
    return HU_AddToWidgets(multiline, names, hud, align_right, align_top, 0, 0);
  }
  else if (!strcasecmp(alignstr, "topcenter")   || !strcasecmp(alignstr, "uppercenter"))
  {
    return HU_AddToWidgets(multiline, names, hud, align_center, align_top, 0, 0);
  }
  else if (!strcasecmp(alignstr, "bottomleft")  || !strcasecmp(alignstr, "lowerleft"))
  {
    return HU_AddToWidgets(multiline, names, hud, align_left, align_bottom, 0, 0);
  }
  else if (!strcasecmp(alignstr, "bottomright") || !strcasecmp(alignstr, "lowerright"))
  {
    return HU_AddToWidgets(multiline, names, hud, align_right, align_bottom, 0, 0);
  }
  else if (!strcasecmp(alignstr, "bottomcenter")|| !strcasecmp(alignstr, "lowercenter"))
  {
    return HU_AddToWidgets(multiline, names, hud, align_center, align_bottom, 0, 0);
  }

  return false;
}

static void HU_ParseHUD (void)
{
  u_scanner_t scanner, *s;
  int hud = -1;
  int lumpnum;
  const char *data;
  int length;

  if ((lumpnum = W_CheckNumForName("WOOFHUD")) == -1)
  {
    return;
  }

  data = W_CacheLumpNum(lumpnum, PU_CACHE);
  length = W_LumpLength(lumpnum);

  scanner = U_ScanOpen(data, length, "WOOFHUD");
  s = &scanner;

  while (U_HasTokensLeft(s))
  {
    char *name;

    if (!U_CheckToken(s, TK_Identifier))
    {
      U_GetNextToken(s, true);
      continue;
    }

    if (!strcasecmp("HUD", s->string))
    {
      U_MustGetInteger(s);
      hud = s->number;

      if (hud < 0 || hud >= MAX_HUDS)
      {
        U_Error(s, "HUD (%d) must be between 0 and %d", hud, MAX_HUDS - 1);
      }

      memset(boom_widgets[hud], 0, sizeof(boom_widgets[hud]));
      continue;
    }

    name = M_StringDuplicate(s->string);

    if (U_CheckToken(s, TK_IntConst))
    {
      int x, y;

      x = s->number;
      U_MustGetInteger(s);
      y = s->number;

      if (!HU_AddHUDCoords(name, hud, x, y))
      {
        U_Error(s, "Cannot set coordinates for widget (%s)", name);
      }
    }
    else
    {
      char *align;

      U_MustGetToken(s, TK_Identifier);
      align = M_StringDuplicate(s->string);

      if (!HU_AddHUDAlignment(name, hud, align))
      {
        U_Error(s, "Cannot set alignment for widget (%s)", name);
      }

      free(align);
    }

    free(name);
  }

  U_ScanClose(s);
}

//----------------------------------------------------------------------------
//
// $Log: hu_stuff.c,v $
// Revision 1.27  1998/05/10  19:03:41  jim
// formatted/documented hu_stuff
//
// Revision 1.26  1998/05/03  22:25:24  killough
// Provide minimal headers at top; nothing else
//
// Revision 1.25  1998/04/28  15:53:58  jim
// Fix message list bug in small screen mode
//
// Revision 1.24  1998/04/22  12:50:14  jim
// Fix lockout from dynamic message change
//
// Revision 1.23  1998/04/05  10:09:51  jim
// added STCFN096 lump
//
// Revision 1.22  1998/03/28  05:32:12  jim
// Text enabling changes for DEH
//
// Revision 1.19  1998/03/17  20:45:23  jim
// added frags to HUD
//
// Revision 1.18  1998/03/15  14:42:16  jim
// added green fist/chainsaw in HUD when berserk
//
// Revision 1.17  1998/03/10  07:07:15  jim
// Fixed display glitch in HUD cycle
//
// Revision 1.16  1998/03/09  11:01:48  jim
// fixed string overflow for DEH, added graphic keys
//
// Revision 1.15  1998/03/09  07:10:09  killough
// Use displayplayer instead of consoleplayer
//
// Revision 1.14  1998/03/05  00:57:37  jim
// Scattered HUD
//
// Revision 1.13  1998/03/04  11:50:48  jim
// Change automap coord display
//
// Revision 1.12  1998/02/26  22:58:26  jim
// Added message review display to HUD
//
// Revision 1.11  1998/02/23  14:20:51  jim
// Merged HUD stuff, fixed p_plats.c to support elevators again
//
// Revision 1.10  1998/02/23  04:26:07  killough
// really allow new hud stuff to be turned off COMPLETELY
//
// Revision 1.9  1998/02/22  12:51:26  jim
// HUD control on F5, z coord, spacing change
//
// Revision 1.7  1998/02/20  18:46:51  jim
// cleanup of HUD control
//
// Revision 1.6  1998/02/19  16:54:53  jim
// Optimized HUD and made more configurable
//
// Revision 1.5  1998/02/18  11:55:55  jim
// Fixed issues with HUD and reduced screen size
//
// Revision 1.3  1998/02/15  02:47:47  phares
// User-defined keys
//
// Revision 1.2  1998/01/26  19:23:22  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:55  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
