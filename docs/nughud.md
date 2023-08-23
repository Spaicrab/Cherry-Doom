# Nugget Doom's NUGHUD guide

As a Nugget Doom fork, Cherry Doom supports the NUGHUD lump, that is used to customize the Crispy HUD.

**The `NUGHUD` lump** is a variant of MBF's `OPTIONS` lump, **used specifically by Nugget Doom to customize the Crispy HUD**, which we will refer to as _Nugget HUD_ from now on.

As implied, `NUGHUD` uses the same format as `OPTIONS`. Excerpt from `mbfedit.txt`:

```
The OPTIONS lump has the same format as mbf.cfg: A text file listing option
names and values, optionally separated by blank or comment lines.
```

## Loading NUGHUD lumps

**`NUGHUD` lumps can be loaded just like `OPTIONS` lumps**:

- By loading WADs which contain a lump explicitly named `NUGHUD`;
- By including a file explicitly named `nughud.lmp` (case-insensitive) in the corresponding autoload folder;
- By using the `-file` command-line parameter to load a file explicitly named `nughud.#`, where `#` can be any extension name (`.lmp` is recommended).

## NUGHUD elements

In practice, **there are three types of elements in the Nugget HUD**. The following variables are shared across all types:

- `_x`: **An integer for X position**, which can be any number between 0 and 320 (inclusive), and also -1 in some cases.
- `_y`: **An integer for Y position**, which can be any number between 0 and 200 (inclusive).
- `_wide`: **An integer to shift the element's X position based on widescreen**, with the following possible values:
  - A value of -2 will shift the element left forcefully;
  - A value of -1 will shift the element left only when using the widescreen Nugget HUD;
  - A value of 0 will keep the element in place regardless of widescreen;
  - A value of 1 will shift the element right only when using the widescreen Nugget HUD;
  - A value of 2 will shift the element right forcefully.

### Widgets

The following widgets are available:

```
nughud_ammo ----- Ammo count for the currently-equipped weapon
nughud_health --- Health count
nughud_arms# ---- Arms (weapon) number, where # is a number between 1 and 9 (inclusive)
nughud_frags ---- Frags count, only shown during Deathmatch games
nughud_face ----- Face (Mugshot)
nughud_armor ---- Armor count
nughud_key# ----- Key display, where # is a number between 0 and 2 (in order: Blue Key; Yellow Key; Red Key)
nughud_ammo# ---- Ammo count for each type, where # is a number between 0 and 3 (in order: Bullets; Shells; Cells; Rockets)
nughud_maxammo# - Same as the above, but for Max Ammo
```

**Widgets support an X position value of -1 to disable the widget.**

Aside from the shared variables, there is an additional boolean variable (value of 0 or 1), `nughud_face_bg`, that toggles the _Face_ background, whose position is linked to that of the _Face_ itself.

**Widgets example:**

```
; This is a comment!
; Loading a NUGHUD lump with these contents will restore the Face widget.

nughud_face_x 143

; Move the Frags widget elsewhere, since in the default NUGHUD distribution,
; it is drawn right where the Face is drawn in the traditional Status Bar.
nughud_frags_x    314
nughud_frags_y    155
nughud_frags_wide 1
```

**`NUGHUD` supports custom fonts for most widgets.**
Graphics for all characters of a given font must be provided for the font to be used, else the Vanilla font will be used instead.

The following fonts are available:

```
Tall Numbers, used for the Health, Armor, current-weapon Ammo and Frags counts:

  NHTNUM# -- Number, where # is a number between 0 and 9 (inclusive)
  NHTMINUS - Minus sign
  NHTPRCNT - Percent sign

Ammo Numbers, used for the Ammo and Max Ammo counts:

  NHAMNUM# - Number, where # is a number between 0 and 9 (inclusive)

Arms Numbers, used for the weapon numbers:

  NHW0NUM# - Weapon unavailable, where # is a number between 1 and 9 (inclusive)
  NHW1NUM# - Weapon available, where # is a number between 1 and 9 (inclusive)

Keys:

  NHKEYS# -- Key, where # is a number between 0 and 8 (inclusive)

Berserk, drawn in place of the Ammo count when using the Berserk Fist:

  NHBERSRK - Berserk graphic
```

### Text Lines

The following text lines are available:

```
nughud_time ----- Time display, only shown if enabled by the user
nughud_sts ------ Stats (Kills/Items/Secrets) display, only shown if enabled by the user
nughud_title ---- Level Name display, only shown on the Automap
nughud_powers --- Powerup timers, only shown if enabled by the user
nughud_attempts - Attempt counter, only shown if enabled by the user
nughud_coord ---- Coordinates display, only shown if enabled by the user
nughud_fps ------ FPS display, only shown when the FPS cheat is activated
nughud_message -- Message display
nughud_secret --- Secret Message display
```

Aside from the shared variables, **text lines have an additional integer variable, `_align`, that sets the text line's alignment**: -1 will align it left, 0 will center it and 1 will align it right.
There are also additional boolean variables: 
```
nughud_time_sts - Make Time display be relocated to the position of the Stats display, in case the latter is inactive.
nughud_attempts_time - Make Attempt counter be relocated to the position of the Time display, in case:
  A) the Time display is inactive and the Stats display is active
  B) the Time display is active and is relocated to the position of the inactive Stats display
nughud_attempts_sts - Make Attempt counter be relocated to the position of the Stats display, in case
                      the Stats display is inactive and the Time display is either also inactive or active
                      but with nughud_time_sts set to 0
```

The _Message_ display supports an X position value of -1 to forcefully draw it at its original X position, where it'll be affected by the Centered Messages setting.

**Text lines example:**

```
; Loading a NUGHUD lump with these contents will draw the Level Name display centered.

nughud_title_x     160
nughud_title_wide  0
nughud_title_align 0
```

### Patches

**Patches are static graphics that can be drawn anywhere on the screen**, behind the widgets.
Up to 8 patches can be drawn. They are drawn in increasing order (i.e. `patch1` is drawn below `patch2`, which is drawn below `patch3`, and so on).

Aside from the shared variables, **patches have an additional string variable, `_name`, that determines the name of the graphic lump to be used**, which can either be a sprite (i.e. a lump between `S_START` and `S_END` markers, like `MEDIA0`) or a graphic (like `STBAR`).
The names used in the `NUGHUD` lump MUST be enclosed between quotation marks.

Patches do NOT support an X position value of -1. They can be disabled simply by not providing any graphic.

**Patches example:**

```
; Loading a NUGHUD lump with these contents will draw
; the Status Bar graphic in its traditional position.
; It doesn't look right, but it's just an example.

nughud_patch1_x 0
nughud_patch1_y 168
nughud_patch1_wide 0
nughud_patch1_name "STBAR"

; Also draw the Arms box, which is actually a separate graphic.
nughud_patch2_x 104
nughud_patch2_y 168
nughud_patch2_wide 0
nughud_patch2_name "STARMS"
```

**Custom lumps CAN be used** (for example, a graphic called `NEWPATCH`).

### Miscellaneous

**There's an additional fixed-point variable, `nughud_weapheight`, to increase the height at which weapon sprites are drawn**.
It can be any value between 0 and 200 (inclusive).

---

By default and compared to the original Crispy HUD, the Nugget HUD hides the face widget and shows Arms numbers 2-9 instead of 2-7.

**The default distribution for the Nugget HUD**, as defined in the executable, **is available in text format as `nughud.lmp`**, found in the `docs/` folder. Comments were added to it for clarity. Feel free to use it as a base to make new distributions.

It is advised that you do not include values for variables that you do not wish to modify, as to avoid issues if the handling of any of them is altered in the future.