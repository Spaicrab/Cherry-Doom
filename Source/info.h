// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: info.h,v 1.10 1998/05/12 12:47:31 phares Exp $
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
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//  02111-1307, USA.
//
// DESCRIPTION:
//      Thing frame/state LUT,
//      generated by multigen utilitiy.
//      This one is the original DOOM version, preserved.
//
//-----------------------------------------------------------------------------

#ifndef __INFO__
#define __INFO__

#include "config.h"

// Needed for action function pointer handling.
#include "d_think.h"

#define MAXSTATEARGS 8

// ********************************************************************
// Sprite name enumeration - must match info.c
// ********************************************************************
typedef enum
{
  // [Nugget] We don't need a whole line for each of these, do we?
  SPR_TROO,SPR_SHTG,SPR_PUNG,SPR_PISG,SPR_PISF,SPR_SHTF,SPR_SHT2,SPR_CHGG,SPR_CHGF,SPR_MISG,
  SPR_MISF,SPR_SAWG,SPR_PLSG,SPR_PLSF,SPR_BFGG,SPR_BFGF,SPR_BLUD,SPR_PUFF,SPR_BAL1,SPR_BAL2,
  SPR_PLSS,SPR_PLSE,SPR_MISL,SPR_BFS1,SPR_BFE1,SPR_BFE2,SPR_TFOG,SPR_IFOG,SPR_PLAY,SPR_POSS,
  SPR_SPOS,SPR_VILE,SPR_FIRE,SPR_FATB,SPR_FBXP,SPR_SKEL,SPR_MANF,SPR_FATT,SPR_CPOS,SPR_SARG,
  SPR_HEAD,SPR_BAL7,SPR_BOSS,SPR_BOS2,SPR_SKUL,SPR_SPID,SPR_BSPI,SPR_APLS,SPR_APBX,SPR_CYBR,
  SPR_PAIN,SPR_SSWV,SPR_KEEN,SPR_BBRN,SPR_BOSF,SPR_ARM1,SPR_ARM2,SPR_BAR1,SPR_BEXP,SPR_FCAN,
  SPR_BON1,SPR_BON2,SPR_BKEY,SPR_RKEY,SPR_YKEY,SPR_BSKU,SPR_RSKU,SPR_YSKU,SPR_STIM,SPR_MEDI,
  SPR_SOUL,SPR_PINV,SPR_PSTR,SPR_PINS,SPR_MEGA,SPR_SUIT,SPR_PMAP,SPR_PVIS,SPR_CLIP,SPR_AMMO,
  SPR_ROCK,SPR_BROK,SPR_CELL,SPR_CELP,SPR_SHEL,SPR_SBOX,SPR_BPAK,SPR_BFUG,SPR_MGUN,SPR_CSAW,
  SPR_LAUN,SPR_PLAS,SPR_SHOT,SPR_SGN2,SPR_COLU,SPR_SMT2,SPR_GOR1,SPR_POL2,SPR_POL5,SPR_POL4,
  SPR_POL3,SPR_POL1,SPR_POL6,SPR_GOR2,SPR_GOR3,SPR_GOR4,SPR_GOR5,SPR_SMIT,SPR_COL1,SPR_COL2,
  SPR_COL3,SPR_COL4,SPR_CAND,SPR_CBRA,SPR_COL6,SPR_TRE1,SPR_TRE2,SPR_ELEC,SPR_CEYE,SPR_FSKU,
  SPR_COL5,SPR_TBLU,SPR_TGRN,SPR_TRED,SPR_SMBT,SPR_SMGT,SPR_SMRT,SPR_HDB1,SPR_HDB2,SPR_HDB3,
  SPR_HDB4,SPR_HDB5,SPR_HDB6,SPR_POB1,SPR_POB2,SPR_BRS1,SPR_TLMP,SPR_TLP2,
  SPR_TNT1,

  SPR_DOGS, // killough 7/19/98: Marine's best friend :)

  SPR_PLS1, // killough 7/19/98: first  of two plasma fireballs in the beta
  SPR_PLS2, // killough 7/19/98: second of two plasma fireballs in the beta
  SPR_BON3, // killough 7/11/98: evil sceptre in beta version
  SPR_BON4, // killough 7/11/98: unholy bible in beta version

  SPR_BLD2, // [FG] blood splats from Doom Retro, unsused in Woof!

  // [FG] 100 extra sprite names to use in dehacked patches
  SPR_SP00, SPR_SP01, SPR_SP02, SPR_SP03, SPR_SP04, SPR_SP05, SPR_SP06, SPR_SP07, SPR_SP08, SPR_SP09,
  SPR_SP10, SPR_SP11, SPR_SP12, SPR_SP13, SPR_SP14, SPR_SP15, SPR_SP16, SPR_SP17, SPR_SP18, SPR_SP19,
  SPR_SP20, SPR_SP21, SPR_SP22, SPR_SP23, SPR_SP24, SPR_SP25, SPR_SP26, SPR_SP27, SPR_SP28, SPR_SP29,
  SPR_SP30, SPR_SP31, SPR_SP32, SPR_SP33, SPR_SP34, SPR_SP35, SPR_SP36, SPR_SP37, SPR_SP38, SPR_SP39,
  SPR_SP40, SPR_SP41, SPR_SP42, SPR_SP43, SPR_SP44, SPR_SP45, SPR_SP46, SPR_SP47, SPR_SP48, SPR_SP49,
  SPR_SP50, SPR_SP51, SPR_SP52, SPR_SP53, SPR_SP54, SPR_SP55, SPR_SP56, SPR_SP57, SPR_SP58, SPR_SP59,
  SPR_SP60, SPR_SP61, SPR_SP62, SPR_SP63, SPR_SP64, SPR_SP65, SPR_SP66, SPR_SP67, SPR_SP68, SPR_SP69,
  SPR_SP70, SPR_SP71, SPR_SP72, SPR_SP73, SPR_SP74, SPR_SP75, SPR_SP76, SPR_SP77, SPR_SP78, SPR_SP79,
  SPR_SP80, SPR_SP81, SPR_SP82, SPR_SP83, SPR_SP84, SPR_SP85, SPR_SP86, SPR_SP87, SPR_SP88, SPR_SP89,
  SPR_SP90, SPR_SP91, SPR_SP92, SPR_SP93, SPR_SP94, SPR_SP95, SPR_SP96, SPR_SP97, SPR_SP98, SPR_SP99,

  NUMSPRITES  // counter of how many there are
} spritenum_t;

// ********************************************************************
// States (frames) enumeration -- must match info.c
// ********************************************************************
typedef enum
{
  S_NULL,
  S_LIGHTDONE,
  S_PUNCH,
  S_PUNCHDOWN,
  S_PUNCHUP,
  S_PUNCH1,
  S_PUNCH2,
  S_PUNCH3,
  S_PUNCH4,
  S_PUNCH5,
  S_PISTOL,
  S_PISTOLDOWN,
  S_PISTOLUP,
  S_PISTOL1,
  S_PISTOL2,
  S_PISTOL3,
  S_PISTOL4,
  S_PISTOLFLASH,
  S_SGUN,
  S_SGUNDOWN,
  S_SGUNUP,
  S_SGUN1,
  S_SGUN2,
  S_SGUN3,
  S_SGUN4,
  S_SGUN5,
  S_SGUN6,
  S_SGUN7,
  S_SGUN8,
  S_SGUN9,
  S_SGUNFLASH1,
  S_SGUNFLASH2,
  S_DSGUN,
  S_DSGUNDOWN,
  S_DSGUNUP,
  S_DSGUN1,
  S_DSGUN2,
  S_DSGUN3,
  S_DSGUN4,
  S_DSGUN5,
  S_DSGUN6,
  S_DSGUN7,
  S_DSGUN8,
  S_DSGUN9,
  S_DSGUN10,
  S_DSNR1,
  S_DSNR2,
  S_DSGUNFLASH1,
  S_DSGUNFLASH2,
  S_CHAIN,
  S_CHAINDOWN,
  S_CHAINUP,
  S_CHAIN1,
  S_CHAIN2,
  S_CHAIN3,
  S_CHAINFLASH1,
  S_CHAINFLASH2,
  S_MISSILE,
  S_MISSILEDOWN,
  S_MISSILEUP,
  S_MISSILE1,
  S_MISSILE2,
  S_MISSILE3,
  S_MISSILEFLASH1,
  S_MISSILEFLASH2,
  S_MISSILEFLASH3,
  S_MISSILEFLASH4,
  S_SAW,
  S_SAWB,
  S_SAWDOWN,
  S_SAWUP,
  S_SAW1,
  S_SAW2,
  S_SAW3,
  S_PLASMA,
  S_PLASMADOWN,
  S_PLASMAUP,
  S_PLASMA1,
  S_PLASMA2,
  S_PLASMAFLASH1,
  S_PLASMAFLASH2,
  S_BFG,
  S_BFGDOWN,
  S_BFGUP,
  S_BFG1,
  S_BFG2,
  S_BFG3,
  S_BFG4,
  S_BFGFLASH1,
  S_BFGFLASH2,
  S_BLOOD1,
  S_BLOOD2,
  S_BLOOD3,
  S_PUFF1,
  S_PUFF2,
  S_PUFF3,
  S_PUFF4,
  S_TBALL1,
  S_TBALL2,
  S_TBALLX1,
  S_TBALLX2,
  S_TBALLX3,
  S_RBALL1,
  S_RBALL2,
  S_RBALLX1,
  S_RBALLX2,
  S_RBALLX3,
  S_PLASBALL,
  S_PLASBALL2,
  S_PLASEXP,
  S_PLASEXP2,
  S_PLASEXP3,
  S_PLASEXP4,
  S_PLASEXP5,
  S_ROCKET,
  S_BFGSHOT,
  S_BFGSHOT2,
  S_BFGLAND,
  S_BFGLAND2,
  S_BFGLAND3,
  S_BFGLAND4,
  S_BFGLAND5,
  S_BFGLAND6,
  S_BFGEXP,
  S_BFGEXP2,
  S_BFGEXP3,
  S_BFGEXP4,
  S_EXPLODE1,
  S_EXPLODE2,
  S_EXPLODE3,
  S_TFOG,
  S_TFOG01,
  S_TFOG02,
  S_TFOG2,
  S_TFOG3,
  S_TFOG4,
  S_TFOG5,
  S_TFOG6,
  S_TFOG7,
  S_TFOG8,
  S_TFOG9,
  S_TFOG10,
  S_IFOG,
  S_IFOG01,
  S_IFOG02,
  S_IFOG2,
  S_IFOG3,
  S_IFOG4,
  S_IFOG5,
  S_PLAY,
  S_PLAY_RUN1,
  S_PLAY_RUN2,
  S_PLAY_RUN3,
  S_PLAY_RUN4,
  S_PLAY_ATK1,
  S_PLAY_ATK2,
  S_PLAY_PAIN,
  S_PLAY_PAIN2,
  S_PLAY_DIE1,
  S_PLAY_DIE2,
  S_PLAY_DIE3,
  S_PLAY_DIE4,
  S_PLAY_DIE5,
  S_PLAY_DIE6,
  S_PLAY_DIE7,
  S_PLAY_XDIE1,
  S_PLAY_XDIE2,
  S_PLAY_XDIE3,
  S_PLAY_XDIE4,
  S_PLAY_XDIE5,
  S_PLAY_XDIE6,
  S_PLAY_XDIE7,
  S_PLAY_XDIE8,
  S_PLAY_XDIE9,
  S_POSS_STND,
  S_POSS_STND2,
  S_POSS_RUN1,
  S_POSS_RUN2,
  S_POSS_RUN3,
  S_POSS_RUN4,
  S_POSS_RUN5,
  S_POSS_RUN6,
  S_POSS_RUN7,
  S_POSS_RUN8,
  S_POSS_ATK1,
  S_POSS_ATK2,
  S_POSS_ATK3,
  S_POSS_PAIN,
  S_POSS_PAIN2,
  S_POSS_DIE1,
  S_POSS_DIE2,
  S_POSS_DIE3,
  S_POSS_DIE4,
  S_POSS_DIE5,
  S_POSS_XDIE1,
  S_POSS_XDIE2,
  S_POSS_XDIE3,
  S_POSS_XDIE4,
  S_POSS_XDIE5,
  S_POSS_XDIE6,
  S_POSS_XDIE7,
  S_POSS_XDIE8,
  S_POSS_XDIE9,
  S_POSS_RAISE1,
  S_POSS_RAISE2,
  S_POSS_RAISE3,
  S_POSS_RAISE4,
  S_SPOS_STND,
  S_SPOS_STND2,
  S_SPOS_RUN1,
  S_SPOS_RUN2,
  S_SPOS_RUN3,
  S_SPOS_RUN4,
  S_SPOS_RUN5,
  S_SPOS_RUN6,
  S_SPOS_RUN7,
  S_SPOS_RUN8,
  S_SPOS_ATK1,
  S_SPOS_ATK2,
  S_SPOS_ATK3,
  S_SPOS_PAIN,
  S_SPOS_PAIN2,
  S_SPOS_DIE1,
  S_SPOS_DIE2,
  S_SPOS_DIE3,
  S_SPOS_DIE4,
  S_SPOS_DIE5,
  S_SPOS_XDIE1,
  S_SPOS_XDIE2,
  S_SPOS_XDIE3,
  S_SPOS_XDIE4,
  S_SPOS_XDIE5,
  S_SPOS_XDIE6,
  S_SPOS_XDIE7,
  S_SPOS_XDIE8,
  S_SPOS_XDIE9,
  S_SPOS_RAISE1,
  S_SPOS_RAISE2,
  S_SPOS_RAISE3,
  S_SPOS_RAISE4,
  S_SPOS_RAISE5,
  S_VILE_STND,
  S_VILE_STND2,
  S_VILE_RUN1,
  S_VILE_RUN2,
  S_VILE_RUN3,
  S_VILE_RUN4,
  S_VILE_RUN5,
  S_VILE_RUN6,
  S_VILE_RUN7,
  S_VILE_RUN8,
  S_VILE_RUN9,
  S_VILE_RUN10,
  S_VILE_RUN11,
  S_VILE_RUN12,
  S_VILE_ATK1,
  S_VILE_ATK2,
  S_VILE_ATK3,
  S_VILE_ATK4,
  S_VILE_ATK5,
  S_VILE_ATK6,
  S_VILE_ATK7,
  S_VILE_ATK8,
  S_VILE_ATK9,
  S_VILE_ATK10,
  S_VILE_ATK11,
  S_VILE_HEAL1,
  S_VILE_HEAL2,
  S_VILE_HEAL3,
  S_VILE_PAIN,
  S_VILE_PAIN2,
  S_VILE_DIE1,
  S_VILE_DIE2,
  S_VILE_DIE3,
  S_VILE_DIE4,
  S_VILE_DIE5,
  S_VILE_DIE6,
  S_VILE_DIE7,
  S_VILE_DIE8,
  S_VILE_DIE9,
  S_VILE_DIE10,
  S_FIRE1,
  S_FIRE2,
  S_FIRE3,
  S_FIRE4,
  S_FIRE5,
  S_FIRE6,
  S_FIRE7,
  S_FIRE8,
  S_FIRE9,
  S_FIRE10,
  S_FIRE11,
  S_FIRE12,
  S_FIRE13,
  S_FIRE14,
  S_FIRE15,
  S_FIRE16,
  S_FIRE17,
  S_FIRE18,
  S_FIRE19,
  S_FIRE20,
  S_FIRE21,
  S_FIRE22,
  S_FIRE23,
  S_FIRE24,
  S_FIRE25,
  S_FIRE26,
  S_FIRE27,
  S_FIRE28,
  S_FIRE29,
  S_FIRE30,
  S_SMOKE1,
  S_SMOKE2,
  S_SMOKE3,
  S_SMOKE4,
  S_SMOKE5,
  S_TRACER,
  S_TRACER2,
  S_TRACEEXP1,
  S_TRACEEXP2,
  S_TRACEEXP3,
  S_SKEL_STND,
  S_SKEL_STND2,
  S_SKEL_RUN1,
  S_SKEL_RUN2,
  S_SKEL_RUN3,
  S_SKEL_RUN4,
  S_SKEL_RUN5,
  S_SKEL_RUN6,
  S_SKEL_RUN7,
  S_SKEL_RUN8,
  S_SKEL_RUN9,
  S_SKEL_RUN10,
  S_SKEL_RUN11,
  S_SKEL_RUN12,
  S_SKEL_FIST1,
  S_SKEL_FIST2,
  S_SKEL_FIST3,
  S_SKEL_FIST4,
  S_SKEL_MISS1,
  S_SKEL_MISS2,
  S_SKEL_MISS3,
  S_SKEL_MISS4,
  S_SKEL_PAIN,
  S_SKEL_PAIN2,
  S_SKEL_DIE1,
  S_SKEL_DIE2,
  S_SKEL_DIE3,
  S_SKEL_DIE4,
  S_SKEL_DIE5,
  S_SKEL_DIE6,
  S_SKEL_RAISE1,
  S_SKEL_RAISE2,
  S_SKEL_RAISE3,
  S_SKEL_RAISE4,
  S_SKEL_RAISE5,
  S_SKEL_RAISE6,
  S_FATSHOT1,
  S_FATSHOT2,
  S_FATSHOTX1,
  S_FATSHOTX2,
  S_FATSHOTX3,
  S_FATT_STND,
  S_FATT_STND2,
  S_FATT_RUN1,
  S_FATT_RUN2,
  S_FATT_RUN3,
  S_FATT_RUN4,
  S_FATT_RUN5,
  S_FATT_RUN6,
  S_FATT_RUN7,
  S_FATT_RUN8,
  S_FATT_RUN9,
  S_FATT_RUN10,
  S_FATT_RUN11,
  S_FATT_RUN12,
  S_FATT_ATK1,
  S_FATT_ATK2,
  S_FATT_ATK3,
  S_FATT_ATK4,
  S_FATT_ATK5,
  S_FATT_ATK6,
  S_FATT_ATK7,
  S_FATT_ATK8,
  S_FATT_ATK9,
  S_FATT_ATK10,
  S_FATT_PAIN,
  S_FATT_PAIN2,
  S_FATT_DIE1,
  S_FATT_DIE2,
  S_FATT_DIE3,
  S_FATT_DIE4,
  S_FATT_DIE5,
  S_FATT_DIE6,
  S_FATT_DIE7,
  S_FATT_DIE8,
  S_FATT_DIE9,
  S_FATT_DIE10,
  S_FATT_RAISE1,
  S_FATT_RAISE2,
  S_FATT_RAISE3,
  S_FATT_RAISE4,
  S_FATT_RAISE5,
  S_FATT_RAISE6,
  S_FATT_RAISE7,
  S_FATT_RAISE8,
  S_CPOS_STND,
  S_CPOS_STND2,
  S_CPOS_RUN1,
  S_CPOS_RUN2,
  S_CPOS_RUN3,
  S_CPOS_RUN4,
  S_CPOS_RUN5,
  S_CPOS_RUN6,
  S_CPOS_RUN7,
  S_CPOS_RUN8,
  S_CPOS_ATK1,
  S_CPOS_ATK2,
  S_CPOS_ATK3,
  S_CPOS_ATK4,
  S_CPOS_PAIN,
  S_CPOS_PAIN2,
  S_CPOS_DIE1,
  S_CPOS_DIE2,
  S_CPOS_DIE3,
  S_CPOS_DIE4,
  S_CPOS_DIE5,
  S_CPOS_DIE6,
  S_CPOS_DIE7,
  S_CPOS_XDIE1,
  S_CPOS_XDIE2,
  S_CPOS_XDIE3,
  S_CPOS_XDIE4,
  S_CPOS_XDIE5,
  S_CPOS_XDIE6,
  S_CPOS_RAISE1,
  S_CPOS_RAISE2,
  S_CPOS_RAISE3,
  S_CPOS_RAISE4,
  S_CPOS_RAISE5,
  S_CPOS_RAISE6,
  S_CPOS_RAISE7,
  S_TROO_STND,
  S_TROO_STND2,
  S_TROO_RUN1,
  S_TROO_RUN2,
  S_TROO_RUN3,
  S_TROO_RUN4,
  S_TROO_RUN5,
  S_TROO_RUN6,
  S_TROO_RUN7,
  S_TROO_RUN8,
  S_TROO_ATK1,
  S_TROO_ATK2,
  S_TROO_ATK3,
  S_TROO_PAIN,
  S_TROO_PAIN2,
  S_TROO_DIE1,
  S_TROO_DIE2,
  S_TROO_DIE3,
  S_TROO_DIE4,
  S_TROO_DIE5,
  S_TROO_XDIE1,
  S_TROO_XDIE2,
  S_TROO_XDIE3,
  S_TROO_XDIE4,
  S_TROO_XDIE5,
  S_TROO_XDIE6,
  S_TROO_XDIE7,
  S_TROO_XDIE8,
  S_TROO_RAISE1,
  S_TROO_RAISE2,
  S_TROO_RAISE3,
  S_TROO_RAISE4,
  S_TROO_RAISE5,
  S_SARG_STND,
  S_SARG_STND2,
  S_SARG_RUN1,
  S_SARG_RUN2,
  S_SARG_RUN3,
  S_SARG_RUN4,
  S_SARG_RUN5,
  S_SARG_RUN6,
  S_SARG_RUN7,
  S_SARG_RUN8,
  S_SARG_ATK1,
  S_SARG_ATK2,
  S_SARG_ATK3,
  S_SARG_PAIN,
  S_SARG_PAIN2,
  S_SARG_DIE1,
  S_SARG_DIE2,
  S_SARG_DIE3,
  S_SARG_DIE4,
  S_SARG_DIE5,
  S_SARG_DIE6,
  S_SARG_RAISE1,
  S_SARG_RAISE2,
  S_SARG_RAISE3,
  S_SARG_RAISE4,
  S_SARG_RAISE5,
  S_SARG_RAISE6,
  S_HEAD_STND,
  S_HEAD_RUN1,
  S_HEAD_ATK1,
  S_HEAD_ATK2,
  S_HEAD_ATK3,
  S_HEAD_PAIN,
  S_HEAD_PAIN2,
  S_HEAD_PAIN3,
  S_HEAD_DIE1,
  S_HEAD_DIE2,
  S_HEAD_DIE3,
  S_HEAD_DIE4,
  S_HEAD_DIE5,
  S_HEAD_DIE6,
  S_HEAD_RAISE1,
  S_HEAD_RAISE2,
  S_HEAD_RAISE3,
  S_HEAD_RAISE4,
  S_HEAD_RAISE5,
  S_HEAD_RAISE6,
  S_BRBALL1,
  S_BRBALL2,
  S_BRBALLX1,
  S_BRBALLX2,
  S_BRBALLX3,
  S_BOSS_STND,
  S_BOSS_STND2,
  S_BOSS_RUN1,
  S_BOSS_RUN2,
  S_BOSS_RUN3,
  S_BOSS_RUN4,
  S_BOSS_RUN5,
  S_BOSS_RUN6,
  S_BOSS_RUN7,
  S_BOSS_RUN8,
  S_BOSS_ATK1,
  S_BOSS_ATK2,
  S_BOSS_ATK3,
  S_BOSS_PAIN,
  S_BOSS_PAIN2,
  S_BOSS_DIE1,
  S_BOSS_DIE2,
  S_BOSS_DIE3,
  S_BOSS_DIE4,
  S_BOSS_DIE5,
  S_BOSS_DIE6,
  S_BOSS_DIE7,
  S_BOSS_RAISE1,
  S_BOSS_RAISE2,
  S_BOSS_RAISE3,
  S_BOSS_RAISE4,
  S_BOSS_RAISE5,
  S_BOSS_RAISE6,
  S_BOSS_RAISE7,
  S_BOS2_STND,
  S_BOS2_STND2,
  S_BOS2_RUN1,
  S_BOS2_RUN2,
  S_BOS2_RUN3,
  S_BOS2_RUN4,
  S_BOS2_RUN5,
  S_BOS2_RUN6,
  S_BOS2_RUN7,
  S_BOS2_RUN8,
  S_BOS2_ATK1,
  S_BOS2_ATK2,
  S_BOS2_ATK3,
  S_BOS2_PAIN,
  S_BOS2_PAIN2,
  S_BOS2_DIE1,
  S_BOS2_DIE2,
  S_BOS2_DIE3,
  S_BOS2_DIE4,
  S_BOS2_DIE5,
  S_BOS2_DIE6,
  S_BOS2_DIE7,
  S_BOS2_RAISE1,
  S_BOS2_RAISE2,
  S_BOS2_RAISE3,
  S_BOS2_RAISE4,
  S_BOS2_RAISE5,
  S_BOS2_RAISE6,
  S_BOS2_RAISE7,
  S_SKULL_STND,
  S_SKULL_STND2,
  S_SKULL_RUN1,
  S_SKULL_RUN2,
  S_SKULL_ATK1,
  S_SKULL_ATK2,
  S_SKULL_ATK3,
  S_SKULL_ATK4,
  S_SKULL_PAIN,
  S_SKULL_PAIN2,
  S_SKULL_DIE1,
  S_SKULL_DIE2,
  S_SKULL_DIE3,
  S_SKULL_DIE4,
  S_SKULL_DIE5,
  S_SKULL_DIE6,
  S_SPID_STND,
  S_SPID_STND2,
  S_SPID_RUN1,
  S_SPID_RUN2,
  S_SPID_RUN3,
  S_SPID_RUN4,
  S_SPID_RUN5,
  S_SPID_RUN6,
  S_SPID_RUN7,
  S_SPID_RUN8,
  S_SPID_RUN9,
  S_SPID_RUN10,
  S_SPID_RUN11,
  S_SPID_RUN12,
  S_SPID_ATK1,
  S_SPID_ATK2,
  S_SPID_ATK3,
  S_SPID_ATK4,
  S_SPID_PAIN,
  S_SPID_PAIN2,
  S_SPID_DIE1,
  S_SPID_DIE2,
  S_SPID_DIE3,
  S_SPID_DIE4,
  S_SPID_DIE5,
  S_SPID_DIE6,
  S_SPID_DIE7,
  S_SPID_DIE8,
  S_SPID_DIE9,
  S_SPID_DIE10,
  S_SPID_DIE11,
  S_BSPI_STND,
  S_BSPI_STND2,
  S_BSPI_SIGHT,
  S_BSPI_RUN1,
  S_BSPI_RUN2,
  S_BSPI_RUN3,
  S_BSPI_RUN4,
  S_BSPI_RUN5,
  S_BSPI_RUN6,
  S_BSPI_RUN7,
  S_BSPI_RUN8,
  S_BSPI_RUN9,
  S_BSPI_RUN10,
  S_BSPI_RUN11,
  S_BSPI_RUN12,
  S_BSPI_ATK1,
  S_BSPI_ATK2,
  S_BSPI_ATK3,
  S_BSPI_ATK4,
  S_BSPI_PAIN,
  S_BSPI_PAIN2,
  S_BSPI_DIE1,
  S_BSPI_DIE2,
  S_BSPI_DIE3,
  S_BSPI_DIE4,
  S_BSPI_DIE5,
  S_BSPI_DIE6,
  S_BSPI_DIE7,
  S_BSPI_RAISE1,
  S_BSPI_RAISE2,
  S_BSPI_RAISE3,
  S_BSPI_RAISE4,
  S_BSPI_RAISE5,
  S_BSPI_RAISE6,
  S_BSPI_RAISE7,
  S_ARACH_PLAZ,
  S_ARACH_PLAZ2,
  S_ARACH_PLEX,
  S_ARACH_PLEX2,
  S_ARACH_PLEX3,
  S_ARACH_PLEX4,
  S_ARACH_PLEX5,
  S_CYBER_STND,
  S_CYBER_STND2,
  S_CYBER_RUN1,
  S_CYBER_RUN2,
  S_CYBER_RUN3,
  S_CYBER_RUN4,
  S_CYBER_RUN5,
  S_CYBER_RUN6,
  S_CYBER_RUN7,
  S_CYBER_RUN8,
  S_CYBER_ATK1,
  S_CYBER_ATK2,
  S_CYBER_ATK3,
  S_CYBER_ATK4,
  S_CYBER_ATK5,
  S_CYBER_ATK6,
  S_CYBER_PAIN,
  S_CYBER_DIE1,
  S_CYBER_DIE2,
  S_CYBER_DIE3,
  S_CYBER_DIE4,
  S_CYBER_DIE5,
  S_CYBER_DIE6,
  S_CYBER_DIE7,
  S_CYBER_DIE8,
  S_CYBER_DIE9,
  S_CYBER_DIE10,
  S_PAIN_STND,
  S_PAIN_RUN1,
  S_PAIN_RUN2,
  S_PAIN_RUN3,
  S_PAIN_RUN4,
  S_PAIN_RUN5,
  S_PAIN_RUN6,
  S_PAIN_ATK1,
  S_PAIN_ATK2,
  S_PAIN_ATK3,
  S_PAIN_ATK4,
  S_PAIN_PAIN,
  S_PAIN_PAIN2,
  S_PAIN_DIE1,
  S_PAIN_DIE2,
  S_PAIN_DIE3,
  S_PAIN_DIE4,
  S_PAIN_DIE5,
  S_PAIN_DIE6,
  S_PAIN_RAISE1,
  S_PAIN_RAISE2,
  S_PAIN_RAISE3,
  S_PAIN_RAISE4,
  S_PAIN_RAISE5,
  S_PAIN_RAISE6,
  S_SSWV_STND,
  S_SSWV_STND2,
  S_SSWV_RUN1,
  S_SSWV_RUN2,
  S_SSWV_RUN3,
  S_SSWV_RUN4,
  S_SSWV_RUN5,
  S_SSWV_RUN6,
  S_SSWV_RUN7,
  S_SSWV_RUN8,
  S_SSWV_ATK1,
  S_SSWV_ATK2,
  S_SSWV_ATK3,
  S_SSWV_ATK4,
  S_SSWV_ATK5,
  S_SSWV_ATK6,
  S_SSWV_PAIN,
  S_SSWV_PAIN2,
  S_SSWV_DIE1,
  S_SSWV_DIE2,
  S_SSWV_DIE3,
  S_SSWV_DIE4,
  S_SSWV_DIE5,
  S_SSWV_XDIE1,
  S_SSWV_XDIE2,
  S_SSWV_XDIE3,
  S_SSWV_XDIE4,
  S_SSWV_XDIE5,
  S_SSWV_XDIE6,
  S_SSWV_XDIE7,
  S_SSWV_XDIE8,
  S_SSWV_XDIE9,
  S_SSWV_RAISE1,
  S_SSWV_RAISE2,
  S_SSWV_RAISE3,
  S_SSWV_RAISE4,
  S_SSWV_RAISE5,
  S_KEENSTND,
  S_COMMKEEN,
  S_COMMKEEN2,
  S_COMMKEEN3,
  S_COMMKEEN4,
  S_COMMKEEN5,
  S_COMMKEEN6,
  S_COMMKEEN7,
  S_COMMKEEN8,
  S_COMMKEEN9,
  S_COMMKEEN10,
  S_COMMKEEN11,
  S_COMMKEEN12,
  S_KEENPAIN,
  S_KEENPAIN2,
  S_BRAIN,
  S_BRAIN_PAIN,
  S_BRAIN_DIE1,
  S_BRAIN_DIE2,
  S_BRAIN_DIE3,
  S_BRAIN_DIE4,
  S_BRAINEYE,
  S_BRAINEYESEE,
  S_BRAINEYE1,
  S_SPAWN1,
  S_SPAWN2,
  S_SPAWN3,
  S_SPAWN4,
  S_SPAWNFIRE1,
  S_SPAWNFIRE2,
  S_SPAWNFIRE3,
  S_SPAWNFIRE4,
  S_SPAWNFIRE5,
  S_SPAWNFIRE6,
  S_SPAWNFIRE7,
  S_SPAWNFIRE8,
  S_BRAINEXPLODE1,
  S_BRAINEXPLODE2,
  S_BRAINEXPLODE3,
  S_ARM1,
  S_ARM1A,
  S_ARM2,
  S_ARM2A,
  S_BAR1,
  S_BAR2,
  S_BEXP,
  S_BEXP2,
  S_BEXP3,
  S_BEXP4,
  S_BEXP5,
  S_BBAR1,
  S_BBAR2,
  S_BBAR3,
  S_BON1,
  S_BON1A,
  S_BON1B,
  S_BON1C,
  S_BON1D,
  S_BON1E,
  S_BON2,
  S_BON2A,
  S_BON2B,
  S_BON2C,
  S_BON2D,
  S_BON2E,
  S_BKEY,
  S_BKEY2,
  S_RKEY,
  S_RKEY2,
  S_YKEY,
  S_YKEY2,
  S_BSKULL,
  S_BSKULL2,
  S_RSKULL,
  S_RSKULL2,
  S_YSKULL,
  S_YSKULL2,
  S_STIM,
  S_MEDI,
  S_SOUL,
  S_SOUL2,
  S_SOUL3,
  S_SOUL4,
  S_SOUL5,
  S_SOUL6,
  S_PINV,
  S_PINV2,
  S_PINV3,
  S_PINV4,
  S_PSTR,
  S_PINS,
  S_PINS2,
  S_PINS3,
  S_PINS4,
  S_MEGA,
  S_MEGA2,
  S_MEGA3,
  S_MEGA4,
  S_SUIT,
  S_PMAP,
  S_PMAP2,
  S_PMAP3,
  S_PMAP4,
  S_PMAP5,
  S_PMAP6,
  S_PVIS,
  S_PVIS2,
  S_CLIP,
  S_AMMO,
  S_ROCK,
  S_BROK,
  S_CELL,
  S_CELP,
  S_SHEL,
  S_SBOX,
  S_BPAK,
  S_BFUG,
  S_MGUN,
  S_CSAW,
  S_LAUN,
  S_PLAS,
  S_SHOT,
  S_SHOT2,
  S_COLU,
  S_STALAG,
  S_BLOODYTWITCH,
  S_BLOODYTWITCH2,
  S_BLOODYTWITCH3,
  S_BLOODYTWITCH4,
  S_DEADTORSO,
  S_DEADBOTTOM,
  S_HEADSONSTICK,
  S_GIBS,
  S_HEADONASTICK,
  S_HEADCANDLES,
  S_HEADCANDLES2,
  S_DEADSTICK,
  S_LIVESTICK,
  S_LIVESTICK2,
  S_MEAT2,
  S_MEAT3,
  S_MEAT4,
  S_MEAT5,
  S_STALAGTITE,
  S_TALLGRNCOL,
  S_SHRTGRNCOL,
  S_TALLREDCOL,
  S_SHRTREDCOL,
  S_CANDLESTIK,
  S_CANDELABRA,
  S_SKULLCOL,
  S_TORCHTREE,
  S_BIGTREE,
  S_TECHPILLAR,
  S_EVILEYE,
  S_EVILEYE2,
  S_EVILEYE3,
  S_EVILEYE4,
  S_FLOATSKULL,
  S_FLOATSKULL2,
  S_FLOATSKULL3,
  S_HEARTCOL,
  S_HEARTCOL2,
  S_BLUETORCH,
  S_BLUETORCH2,
  S_BLUETORCH3,
  S_BLUETORCH4,
  S_GREENTORCH,
  S_GREENTORCH2,
  S_GREENTORCH3,
  S_GREENTORCH4,
  S_REDTORCH,
  S_REDTORCH2,
  S_REDTORCH3,
  S_REDTORCH4,
  S_BTORCHSHRT,
  S_BTORCHSHRT2,
  S_BTORCHSHRT3,
  S_BTORCHSHRT4,
  S_GTORCHSHRT,
  S_GTORCHSHRT2,
  S_GTORCHSHRT3,
  S_GTORCHSHRT4,
  S_RTORCHSHRT,
  S_RTORCHSHRT2,
  S_RTORCHSHRT3,
  S_RTORCHSHRT4,
  S_HANGNOGUTS,
  S_HANGBNOBRAIN,
  S_HANGTLOOKDN,
  S_HANGTSKULL,
  S_HANGTLOOKUP,
  S_HANGTNOBRAIN,
  S_COLONGIBS,
  S_SMALLPOOL,
  S_BRAINSTEM,
  S_TECHLAMP,
  S_TECHLAMP2,
  S_TECHLAMP3,
  S_TECHLAMP4,
  S_TECH2LAMP,
  S_TECH2LAMP2,
  S_TECH2LAMP3,
  S_TECH2LAMP4,
  S_TNT1, // add state for invisible sprite         // phares 3/8/98

  S_GRENADE,   // killough 8/9/98: grenade launcher
  S_DETONATE,  // killough 8/9/98: detonation of objects
  S_DETONATE2,
  S_DETONATE3,

  S_DOGS_STND,      // killough 7/19/98: Marine's best friend :)
  S_DOGS_STND2,
  S_DOGS_RUN1,
  S_DOGS_RUN2,
  S_DOGS_RUN3,
  S_DOGS_RUN4,
  S_DOGS_RUN5,
  S_DOGS_RUN6,
  S_DOGS_RUN7,
  S_DOGS_RUN8,
  S_DOGS_ATK1,
  S_DOGS_ATK2,
  S_DOGS_ATK3,
  S_DOGS_PAIN,
  S_DOGS_PAIN2,
  S_DOGS_DIE1,
  S_DOGS_DIE2,
  S_DOGS_DIE3,
  S_DOGS_DIE4,
  S_DOGS_DIE5,
  S_DOGS_DIE6,
  S_DOGS_RAISE1,
  S_DOGS_RAISE2,
  S_DOGS_RAISE3,
  S_DOGS_RAISE4,
  S_DOGS_RAISE5,
  S_DOGS_RAISE6,

  S_OLDBFG1,  // killough 7/11/98: the old BFG's 43 firing frames
  S_OLDBFG42 = S_OLDBFG1+41,
  S_OLDBFG43,

  S_PLS1BALL,      // killough 7/19/98: first plasma fireball in the beta
  S_PLS1BALL2,
  S_PLS1EXP,
  S_PLS1EXP2,
  S_PLS1EXP3,
  S_PLS1EXP4,
  S_PLS1EXP5,

  S_PLS2BALL,     // killough 7/19/98: second plasma fireball in the beta
  S_PLS2BALL2,
  S_PLS2BALLX1,
  S_PLS2BALLX2,
  S_PLS2BALLX3,
  S_BON3, // killough 7/11/98: evil sceptre in beta version
  S_BON4, // killough 7/11/98: unholy bible in beta version

  // killough 10/98: beta lost souls were different from their modern cousins
  S_BSKUL_STND,
  S_BSKUL_RUN1,
  S_BSKUL_RUN2,
  S_BSKUL_RUN3,
  S_BSKUL_RUN4,
  S_BSKUL_ATK1,
  S_BSKUL_ATK2,
  S_BSKUL_ATK3,
  S_BSKUL_PAIN1,
  S_BSKUL_PAIN2,
  S_BSKUL_PAIN3,
  S_BSKUL_DIE1,
  S_BSKUL_DIE2,
  S_BSKUL_DIE3,
  S_BSKUL_DIE4,
  S_BSKUL_DIE5,
  S_BSKUL_DIE6,
  S_BSKUL_DIE7,
  S_BSKUL_DIE8,

  S_MUSHROOM,  // killough 10/98: mushroom explosion effect

  NUMSTATES  // Counter of how many there are
} statenum_t;

// ********************************************************************
// Definition of the state (frames) structure
// ********************************************************************
typedef struct
{
  spritenum_t sprite;       // sprite number to show
  long        frame;        // which frame/subframe of the sprite is shown
  long        tics;         // number of gametics this frame should last
  void        (*action)();  // code pointer to function for action if any
  statenum_t  nextstate;    // linked list pointer to next state or zero
  long        misc1, misc2; // used for psprite positioning
  long        args[MAXSTATEARGS]; // [XA] mbf21 args
  int         flags;
} state_t;

// state flags
#define STATEF_SKILL5FAST 0x00000001 // tics halve on nightmare skill

// these are in info.c
extern state_t  original_states[NUMSTATES];
extern char *original_sprnames[];             // 1/17/98 killough

// ********************************************************************
// Thing enumeration -- must match info.c
// ********************************************************************
// Note that many of these are generically named for the ornamentals
//
typedef enum {
  MT_NULL = -1, // null/invalid mobj (zero is reserved for MT_PLAYER)
  MT_PLAYER,
  MT_POSSESSED,
  MT_SHOTGUY,
  MT_VILE,
  MT_FIRE,
  MT_UNDEAD,
  MT_TRACER,
  MT_SMOKE,
  MT_FATSO,
  MT_FATSHOT,
  MT_CHAINGUY,
  MT_TROOP,
  MT_SERGEANT,
  MT_SHADOWS,
  MT_HEAD,
  MT_BRUISER,
  MT_BRUISERSHOT,
  MT_KNIGHT,
  MT_SKULL,
  MT_SPIDER,
  MT_BABY,
  MT_CYBORG,
  MT_PAIN,
  MT_WOLFSS,
  MT_KEEN,
  MT_BOSSBRAIN,
  MT_BOSSSPIT,
  MT_BOSSTARGET,
  MT_SPAWNSHOT,
  MT_SPAWNFIRE,
  MT_BARREL,
  MT_TROOPSHOT,
  MT_HEADSHOT,
  MT_ROCKET,
  MT_PLASMA,
  MT_BFG,
  MT_ARACHPLAZ,
  MT_PUFF,
  MT_BLOOD,
  MT_TFOG,
  MT_IFOG,
  MT_TELEPORTMAN,
  MT_EXTRABFG,
  MT_MISC0,
  MT_MISC1,
  MT_MISC2,
  MT_MISC3,
  MT_MISC4,
  MT_MISC5,
  MT_MISC6,
  MT_MISC7,
  MT_MISC8,
  MT_MISC9,
  MT_MISC10,
  MT_MISC11,
  MT_MISC12,
  MT_INV,
  MT_MISC13,
  MT_INS,
  MT_MISC14,
  MT_MISC15,
  MT_MISC16,
  MT_MEGA,
  MT_CLIP,
  MT_MISC17,
  MT_MISC18,
  MT_MISC19,
  MT_MISC20,
  MT_MISC21,
  MT_MISC22,
  MT_MISC23,
  MT_MISC24,
  MT_MISC25,
  MT_CHAINGUN,
  MT_MISC26,
  MT_MISC27,
  MT_MISC28,
  MT_SHOTGUN,
  MT_SUPERSHOTGUN,
  MT_MISC29,
  MT_MISC30,
  MT_MISC31,
  MT_MISC32,
  MT_MISC33,
  MT_MISC34,
  MT_MISC35,
  MT_MISC36,
  MT_MISC37,
  MT_MISC38,
  MT_MISC39,
  MT_MISC40,
  MT_MISC41,
  MT_MISC42,
  MT_MISC43,
  MT_MISC44,
  MT_MISC45,
  MT_MISC46,
  MT_MISC47,
  MT_MISC48,
  MT_MISC49,
  MT_MISC50,
  MT_MISC51,
  MT_MISC52,
  MT_MISC53,
  MT_MISC54,
  MT_MISC55,
  MT_MISC56,
  MT_MISC57,
  MT_MISC58,
  MT_MISC59,
  MT_MISC60,
  MT_MISC61,
  MT_MISC62,
  MT_MISC63,
  MT_MISC64,
  MT_MISC65,
  MT_MISC66,
  MT_MISC67,
  MT_MISC68,
  MT_MISC69,
  MT_MISC70,
  MT_MISC71,
  MT_MISC72,
  MT_MISC73,
  MT_MISC74,
  MT_MISC75,
  MT_MISC76,
  MT_MISC77,
  MT_MISC78,
  MT_MISC79,
  MT_MISC80,
  MT_MISC81,
  MT_MISC82,
  MT_MISC83,
  MT_MISC84,
  MT_MISC85,
  MT_MISC86,
  MT_PUSH,    // controls push source                     // phares
  MT_PULL,    // controls pull source                     // phares 3/20/98

  MT_DOGS,    // killough 7/19/98: Marine's best friend

  MT_PLASMA1, // killough 7/11/98: first  of alternating beta plasma fireballs
  MT_PLASMA2, // killough 7/11/98: second of alternating beta plasma fireballs
  MT_SCEPTRE, // killough 7/11/98: evil sceptre in beta version
  MT_BIBLE,   // killough 7/11/98: unholy bible in beta version

  MT_MUSICSOURCE, // [crispy] support MUSINFO lump (dynamic music changing)

  NUMMOBJTYPES  // Counter of how many there are

} mobjtype_t;

typedef enum {
  IG_DEFAULT,
  IG_END
} infighting_group_t;

typedef enum {
  PG_GROUPLESS = -1,
  PG_DEFAULT,
  PG_BARON,
  PG_END
} projectile_group_t;

typedef enum {
  SG_DEFAULT,
  SG_END
} splash_group_t;

// ********************************************************************
// Definition of the Thing structure
// ********************************************************************
// Note that these are only indices to the state, sound, etc. arrays
// and not actual pointers.  Most can be set to zero if the action or
// sound doesn't apply (like lamps generally don't attack or whistle).

typedef struct
{
    int doomednum;    // Thing number used in id's editor, and now
                      //  probably by every other editor too
    int spawnstate;   // The state (frame) index when this Thing is
                      //  first created
    int spawnhealth;  // The initial hit points for this Thing
    int seestate;     // The state when it sees you or wakes up
    int seesound;     // The sound it makes when waking
    int reactiontime; // How many tics it waits after it wakes up
                      //  before it will start to attack, in normal
                      //  skills (halved for nightmare)
    int attacksound;  // The sound it makes when it attacks
    int painstate;    // The state to indicate pain
    int painchance;   // A number that is checked against a random
                      //  number 0-255 to see if the Thing is supposed
                      //  to go to its painstate or not.  Note this
                      //  has absolutely nothing to do with the chance
                      //  it will get hurt, just the chance of it
                      //  reacting visibly.
    int painsound;    // The sound it emits when it feels pain
    int meleestate;   // Melee==close attack
    int missilestate; // What states to use when it's in the air, if
                      //  in fact it is ever used as a missile
    int deathstate;   // What state begins the death sequence
    int xdeathstate;  // What state begins the horrible death sequence
                      //  like when a rocket takes out a trooper
    int deathsound;   // The death sound.  See also A_Scream() in
                      //  p_enemy.c for some tweaking that goes on
                      //  for certain monsters
    int speed;        // How fast it moves.  Too fast and it can miss
                      //  collision logic.
    int radius;       // An often incorrect radius
    int height;       // An often incorrect height, used only to see
                      //  if a monster can enter a sector
    int mass;         // How much an impact will move it.  Cacodemons
                      //  seem to retreat when shot because they have
                      //  very little mass and are moved by impact
    int damage;       // If this is a missile, how much does it hurt?
    int activesound;  // What sound it makes wandering around, once
                      //  in a while.  Chance is 3/256 it will.
    int flags;        // Bit masks for lots of things.  See p_mobj.h
    int raisestate;   // The first state for an Archvile or respawn
                      //  resurrection.  Zero means it won't come
                      //  back to life.

    // mbf21
    int flags2;
    int infighting_group;
    int projectile_group;
    int splash_group;
    int ripsound;
    int altspeed;
    int meleerange;

    // [Woof!]
    int bloodcolor;   // [FG] colored blood and gibs
    // DEHEXTRA
    mobjtype_t droppeditem; // mobj to drop after death
} mobjinfo_t;

#define NO_ALTSPEED -1

// See p_mobj_h for addition more technical info
extern mobjinfo_t original_mobjinfo[NUMMOBJTYPES];

// DSDHacked
extern state_t* states;
extern int num_states;
extern char** sprnames;
extern int num_sprites;
extern mobjinfo_t* mobjinfo;
extern int num_mobj_types;

#endif

//----------------------------------------------------------------------------
//
// $Log: info.h,v $
// Revision 1.10  1998/05/12  12:47:31  phares
// Removed OVER_UNDER code
//
// Revision 1.9  1998/05/06  11:31:53  jim
// Moved predefined lump writer info->w_wad
//
// Revision 1.8  1998/05/04  21:35:54  thldrmn
// commenting and reformatting
//
// Revision 1.7  1998/04/22  06:33:58  killough
// Add const to WritePredefinedLumpWad() parm
//
// Revision 1.6  1998/04/21  23:47:10  jim
// Predefined lump dumper option
//
// Revision 1.5  1998/03/23  15:24:09  phares
// Changed pushers to linedef control
//
// Revision 1.4  1998/03/09  18:30:43  phares
// Added invisible sprite for MT_PUSH
//
// Revision 1.3  1998/02/24  08:45:53  phares
// Pushers, recoil, new friction, and over/under work
//
// Revision 1.2  1998/01/26  19:27:02  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:57  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
