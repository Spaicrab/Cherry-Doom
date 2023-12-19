//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
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
// DESCRIPTION:
//      Mission begin melt/wipe screen special effect.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "i_video.h"
#include "v_video.h"
#include "m_random.h"
#include "f_wipe.h"
#include "doomstat.h" // [Nugget]

//
// SCREEN WIPE PACKAGE
//

static byte *wipe_scr_start;
static byte *wipe_scr_end;
static byte *wipe_scr;

static void wipe_shittyColMajorXform(short *array, int width, int height)
{
  short *dest = Z_Malloc(width*height*sizeof(short), PU_STATIC, 0);
  int x, y;

  for(y=0;y<height;y++)
    for(x=0;x<width;x++)
      dest[x*height+y] = array[y*width+x];
  memcpy(array, dest, width*height*sizeof(short));
  Z_Free(dest);
}

static int wipe_initColorXForm(int width, int height, int ticks)
{
  memcpy(wipe_scr, wipe_scr_start, width*height);
  return 0;
}

// killough 3/5/98: reformatted and cleaned up
static int wipe_doColorXForm(int width, int height, int ticks)
{
  boolean unchanged = true;
  byte *w   = wipe_scr;
  byte *e   = wipe_scr_end;
  byte *end = wipe_scr+width*height;

  ticks *= 8; // [Nugget] Speed it up, to match "Melt" wipe speed

  // [Nugget] Screen Wipe speed
  if (!strictmode && wipe_speed_percentage != 100)
  { ticks = MAX(1, ticks * wipe_speed_percentage / 100); }

  for (;w != end; w++, e++)
    if (*w != *e)
      {
        int newval;
        unchanged = false;
        *w = *w > *e ?
          (newval = *w - ticks) < *e ? *e : newval :
          (newval = *w + ticks) > *e ? *e : newval ;
      }
  return unchanged;
}

static int wipe_exitColorXForm(int width, int height, int ticks)
{
  return 0;
}

static int *y;

static int wipe_initMelt(int width, int height, int ticks)
{
  int i;

  // copy start screen to main screen
  memcpy(wipe_scr, wipe_scr_start, width*height);

  // makes this wipe faster (in theory)
  // to have stuff in column-major format
  wipe_shittyColMajorXform((short*)wipe_scr_start, width/2, height);
  wipe_shittyColMajorXform((short*)wipe_scr_end, width/2, height);

  // setup initial column positions (y<0 => not ready to scroll yet)
  y = (int *) Z_Malloc(width*sizeof(int), PU_STATIC, 0);
  y[0] = -(M_Random()%16) * hires;
  for (i=1;i<width;i++)
    {
      int r = ((M_Random()%3) - 1) * hires;
      y[i] = y[i-1] + r;
      if (y[i] > 0)
        y[i] = 0;
      else
        if (y[i] == -16 * hires)
          y[i] = -15 * hires;
    }
  return 0;
}

static int wipe_doMelt(int width, int height, int ticks)
{
  boolean done = true;
  int i;

  width /= 2;

  ticks *= hires; // [Nugget] Hires support: brought from `wipe_ScreenWipe()`

  // [Nugget] Screen Wipe speed
  if (!strictmode && wipe_speed_percentage != 100)
  { ticks = MAX(1, ticks * wipe_speed_percentage / 100); }

  while (ticks--)
    for (i=0;i<width;i++)
      if (y[i]<0)
        {
          y[i]++;
          done = false;
        }
      else
        if (y[i] < height)
          {
            short *s, *d;
            int j, dy, idx;

            dy = (y[i] < 16) ? y[i]+1 : 8;
            if (y[i]+dy >= height)
              dy = height - y[i];
            s = &((short *)wipe_scr_end)[i*height+y[i]];
            d = &((short *)wipe_scr)[y[i]*width+i];
            idx = 0;
            for (j=dy;j;j--)
              {
                d[idx] = *(s++);
                idx += width;
              }
            y[i] += dy;
            s = &((short *)wipe_scr_start)[i*height];
            d = &((short *)wipe_scr)[y[i]*width+i];
            idx = 0;
            for (j=height-y[i];j;j--)
              {
                d[idx] = *(s++);
                idx += width;
              }
            done = false;
          }
  return done;
}

static int wipe_exitMelt(int width, int height, int ticks)
{
  Z_Free(y);
  return 0;
}

// [Nugget] "Fade" wipe /-----------------------------------------------------

static boolean fadeIn;

static int wipe_initFade(int width, int height, int ticks)
{
  fadeIn = false;
  return 0;
}

static int wipe_doFade(int width, int height, int ticks)
{
  int y;
  static int screenshade = 1;
  static const int targshade = 31;
  
  ticks *= 2; // Speed it up, to match "Melt" wipe speed

  // [Nugget] Screen Wipe speed
  if (!strictmode && wipe_speed_percentage != 100)
  { ticks = MAX(1, ticks * wipe_speed_percentage / 100); }

  memcpy(wipe_scr, fadeIn ? wipe_scr_end : wipe_scr_start, width * height);

  for (y = 0;  y < width * height;  y++)
  { wipe_scr[y] = colormaps[0][(screenshade * 256) + wipe_scr[y]]; }

  if (!fadeIn)
  {
    // Fade out to black

    screenshade += ticks;

    if (screenshade > targshade) {
      screenshade = targshade;
      fadeIn = true;
    }
  }
  else
  {
    // Fade in from black

    screenshade -= ticks;

    if (screenshade < 1) {
      screenshade = 1;
      return true;
    }
  }
  
  return false;
}

static int wipe_exitFade(int width, int height, int ticks)
{
  return 0;
}

// [Nugget] -----------------------------------------------------------------/

int wipe_StartScreen(int x, int y, int width, int height)
{
  I_ReadScreen(wipe_scr_start = screens[2]);
  return 0;
}

int wipe_EndScreen(int x, int y, int width, int height)
{
  I_ReadScreen(wipe_scr_end = screens[3]);
  V_DrawBlock(x, y, 0, width, height, wipe_scr_start); // restore start scr.
  return 0;
}

// [Nugget] Rearranged for convenience
static int (*const wipes[])(int, int, int) = {
  0, 0, 0, // [Nugget] Dummies for "no wipe"
  wipe_initMelt,
  wipe_doMelt,
  wipe_exitMelt,
  wipe_initColorXForm,
  wipe_doColorXForm,
  wipe_exitColorXForm,
  // [Nugget] "Fade" wipe /------------
  wipe_initFade,
  wipe_doFade,
  wipe_exitFade
  // [Nugget] ------------------------/
};

// killough 3/5/98: reformatted and cleaned up
int wipe_ScreenWipe(int wipeno, int x, int y, int width, int height, int ticks)
{
  static boolean go;                               // when zero, stop the wipe

  // killough 11/98: hires support
  // [Nugget] `ticks` is now calculated in `wipe_doMelt()`,
  // since that's the only wipe style where it matters
  width *= hires, height *= hires;

  if (!go)                                         // initial stuff
    {
      go = 1;
      wipe_scr = screens[0];
      wipes[wipeno*3](width, height, ticks);
    }
  if (wipes[wipeno*3+1](width, height, ticks))     // final stuff
    {
      wipes[wipeno*3+2](width, height, ticks);
      go = 0;
    }
  return !go;
}

//----------------------------------------------------------------------------
//
// $Log: f_wipe.c,v $
// Revision 1.3  1998/05/03  22:11:24  killough
// beautification
//
// Revision 1.2  1998/01/26  19:23:16  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:54  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
