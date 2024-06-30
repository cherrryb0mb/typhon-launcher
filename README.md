
# <p align=center>typhon-launcher</p>

Description:
=============
**typhon-launcher**(also known as Typhon) is a slim and themeable OpenGL-based frontend, dashboard or program launcher.
It's meant to be easily used on Linux HTPCs, but also of course also
works on standard desktops or as netbook-launcher.

Features (no special order):
=============================

- emulator/rom launcher mode for up to 21 different emulators
- program launcher for unlimited different programs

- themeable in many aspects:
  - mouse picking (move xyz, resize, block) for every single item on the fly
  - color picking for every single item
  - can be completely configured "inside" - including texteditor
  - optional soundeffects
  - use own ttf font (color,position and size configurable)
  - optional background(s),previews,icons, menuicons (position and size configurable)
  - xrossmediabar mode
    - native ps3 theme p3t support
    - configurable (convert p3t to "normal" typhon theme and adjust it to your needs)
  - commandline import (convert to typhon themes) for ps3, and "custom" themes
  - optional play video as (part of the) background, fullscreen or as preview
  - rotate the whole launcher or only parts from it and save the rotated position
  - display several systeminformations in your theme
  - stencilpicture support to hide parts of the screen (f.e. round corners for previewpicures)
  - support for icons and menuicons - menuicons can be "one at once" or in horizontal/vertical xmb mode
  - support for 3 more additional pictures
  - optional selectionbar, 2 hideboxes, decobox, menubar (color,position and size configurable)
  - tons of other configurable theme details
  - settingsbox can be opened from everywhere
  - initial support for (timed) lightning and "monthcolors"
  - programable slideshow with shaders
  - animated wallpaperswitch, animated icons

- search function
- edit modes for everything
- autoscreenshot launched app in configured time for previewpictures
 
- windowmanager independant
- works even without a windowmanager
- very few dependencies (see below)
- works fine with compiz

- mouse rotation/zoom (not using compiz! :) )
  
- optional videoplayer (also for previews)
  videos can be scaled individually

- optional builtin music player

- can be controlled with mouse,joypad,keyboard (and therefore also with remotes)
  keyboard and joypad mapable
- optional support for mikumikudance (mmd) model and motion files using libmmd

- adjust all programnames, paths etc with the builtin editor
- much more that apparently frostworx forgot to mention...

Requirements:
==============

required:
- sfml ( http://www.sfml-dev.org - 2.0 and above required!)
- freetype
- tinyxml
- working opengl 

optional:

- libmmd for mikumikudance model support 
- libp3t (Most likely not on your package manager.Typhon comes with libp3t on its own so if it's not installed it will be linked statically)
- ~~sfemovie ( https://github.com/Yalir/sfeMovie ) for videoplayer. if it is not installed, but sfemovie deps are it will be linked statically~~(sfeMovie is abandoned, I'll most likely end up stripping that out of typhon) 

Compiling:
===========
If you have all dependencies installed (see "Requirements" in README)
just run:
```
cd build
cmake .. 
make
```

To en-/disable optional features just look at the top of CMakeList.txt.
comments are quite self explanatory.

There's also a cmake.sh script, which you might want to use.

Installation:
===============
just run "make install".(Will require root if you're installing to certain paths like /usr !)

Controls:
=========

all buttons can be quickly mapped within typhon.
The default hotkey for opening the keyboard hotkey menu/settings is "F1".
Joypad button mapping menu is toggled with the default hotkey "J".

rotation picking (default F2):
================================
With enabled rotationpicking you can pick an object with left mouse button.
Move the mouse up/down to rotate the y-axis of the picked object
Move the mouse left/right to rotate the x-axis of the picked object.
Toggle z-axis rotation mode with middle mouse button.
With right mouse buton you can reset the coordinates of the selected object to 0.
Mouse wheel up/down changes the depth of the picked object (i.e. cube ot 3d fonts).

Mouse picking (default F3):
============================
mouse picking is en-disabled either within the inputsettings menu or with the hotkey (default F3).
when the mousepointer is in the region of a pickable object (almost all are pickable) this object
turns red, so you know it is active atm. now you can 
- click and hold left mouse button and move the mouse to move the picked object on the fly
- click and hold right mouse button and move the mouse to scale the picked object on the fly
- click middle mousebutton to "block" the active object - see below for explanation
- mouse wheel up/down - in-de/crease the z-position of the active object

mouse blocking:
----------------
sometimes two objects might be in the same area, which makes it quiet hard to drag just one.
although in this case the object with the higher z-position is prefered for picking, you also
have the option to "block" an object. it will change its color from red (picked) to blue (blocked).
blocked objects can't be moved/resized until they are "unblocked" (again middle mouse button).
of course this is also helpful for objects which you already configured 
and don't want to mess up before saving.

color picking (default F4):
============================
With enabled colorpicking you can pick an object with left mouse button
and move the mouse to an area with the color of your choice.
mouse wheel up down changes the transparency of the picked object

settings picking (default F5):
============================
In settingspicking mode you see (almost) every available object.
Those which are disabled in the config are greyed out.
To toggle a picked object press left mousebutton.
Several objects have different datafiles (f.e. textures).
Those can be switched with mouse wheel up/down.


Usage:
=======
below manuals for usage don't cover every single aspect of typhons functions - 
after reading them you will still find many more self-explanatory functions
in the builtin settingsmenus!


Basic Usage:
=============

On an initial launch following default configfiles will be written to 
$HOME/.typhon/: typhonrc, typhon-manualrc, typhon-inputrc, typhon-emurc, themes/typhon and
previews,videopreviews and videos subdirs are created.

using the just created themes/typhon theme, typhon will start up with the settings configured there,
including f.e. the default font $PREFIX/share/typhon/themes/typhon/LiberationSans-Regular.ttf
and the default soundfile $PREFIX/share/typhon/themes/typhon/cursor.wav

in-gui settings are already configurable, including the possibility to edit all programnames and
paths inside typhon.
you probably don't need it as the built in editor works quiet fine,
but if you have leafpad ( http://tarot.freeshell.org/leafpad ) 
installed you can also press F1 to edit
the 2nd configfile typhon-manualrc on the fly 
( - the editor can be replaced in typhon-manualrc or within the editor ).

For basic functionality you should now replace the program-placeholders in the
config with whatever program you want to have in the launcher
( f.e. "program1_3 = firefox" ) -
either with the external texteditor (F1) or with the builtin one (toggle with F11).

When you're done, save the cfg and the launcher is ready for, yes - launching!

If you don't require all menuentries (max 20) of every submenu, just fill in
the value of the programs to be shown for every single submenu.
( f.e. if you just want to see 8 entries in menu3 replace the 20 with an 8
under menu3entries ).

The titles of the submenus can be changed in the "title" sections of the menu
( f.e. title1 = favourites )

That's all for basic usage - also see Chapter "Saving" and Chapter "Config" if you like.

Commandline options:
=====================
see ``typhon --help``

Advanced usage:
================

Picture Support:
-----------------
paths for any kind of image need to be 
configured manually for a new configurationfile.

The names of the previewpictures need to match with the currently selected
menuentry (f.e. if your program1_1 is set to dunan in the configfile,
the matching picture would be expected under $previewpath/dunan.FORMAT - 
where FORMAT is one of png, jpeg, jpg or bmp )

There's also an autoscreenshot option available:
if you want to take a screenshot of the to-be-launched program/rom,
enable the screenshot function and adjust the waitingtime.


p3t-themes:
------------

You can directly use p3t (ps3 xmb) themes and also "convert" them to "real" typhon
themes, so you can save your changes.
The default search path (can be configured in typhon-manualrc) for the p3ts
is ~/.config/typhon/p3ts/

just copy your p3ts there and choose one within the p3t settings menu or via commandline.

previewpictures for browsing through the p3ts are accepted as
"themename.(png|jpg)" in the previewpath.

As all p3ts use the same p3t config and the p3ts don't support fontcolors
(default is copied from {PREFIX}/share/typhon/themes/p3t to ~/.typhon/themes if it is not found)
the default fontcolors might look bad with certain backgrounds.
You have the option though, to copy the currently running p3t to your other "normal"
themes, so the p3t theme will be available with its own theme config,
which you can edit individually.
or you could quickly change the menulist color by pressing the colorpickkey (default F4)
and left-click some color which should be readable. 

Font: 
--------
The default fontpath, which is also written into the
default configfile on initial run, is
{$PREFIX}/usr/share/typhon/themes/typhon/
and the default font is "LiberationSans-Regular.ttf".
Of course you can configure a different font/path in
the typhon configfile.
Without a valid font, typhon doesn't work of course.

The bundeled LiberationSans-Regular.ttf is licensed under the GPL.
license files can be found under data/font-license.

searchmode:
============
when your current menu is an emulator listing you have the option to search for a rom.
just press the search hotkey (default "l") and type in what you are looking for.
Press Return to confirm your search.
It's not case sensitive, so searching for "tur" might also find "Turrican".

the searchbox has the same proportions as the editbox - to configure its size/position
go into the editor-> search/editbox settings (or pick it)

editor:
========
a builtin editor allows you to quickly change programs, paths etc.

changing a program:
--------------------
to change a program 
- enable editmode with the hotkey (default F11) (there should be a red EDITMODE notifier now)
- choose the program which should be changed
- press select key (default Return)
- now type in the new name use backspace to delete characters as usual) 
- press return to activate the changes -> program has changed - editmode is OFF

(when you're done with changing your programs don't forget to save the global themesettings - 
"save settings" in main settingsmenu - else your work will be lost when you quit typhon!)
(or you send me patches)
or I'll completely remove the wiisupport...


Configs:
=========
typhon configs are divided into 5 elements - four global ones and one theme config:
using the default names to explain them (values should be self explanatory):

1)  $HOME/.typhon/typhonrc
		the home for all global config settings which have a number value and don't
		make sense to configure them in the theme configuration

2)  $HOME/.typhon/typhon-manualrc
		here are all global config settings which need to be configured with a texteditor - 
		f.e. programs, screensaverpath and others.
		you can open this file within typhon pressing F1 
		(default used editor is leafpad, which can be configured (irony!) in typhon-manualrc)
		or edit the entries with the builtin editor (toggle F11 and/or go into the editor settingsmenu)
	
3)  $HOME/.typhon/themes/ is the path where all themefiles are expected
		the initially created theme is called typhon.
		as you can see most values are in the themefiles.

4) $HOME/.typhon/typhon-emurc (name is configureable in typhon-manualrc)
		here you can configure the emulators, rompaths, emulator arguments, romsuffixes
		and rom preview picture paths (again built in or external editor) 

5) $HOME/.typhon/typhon-inputrc
		here several input options are saved


read next chapter "Saving" to find out how to save configs/themes

Saving:
========

Apart from simply editing the config file(s) with an editor,
there are also save options available within the launcher directly:

- on the bottom of the mainsettings menu
  is a "save settings" option, where you can save the currently
  used global configfiles (defaults to $HOME/.typhon/typhonrc ,
  $HOME/.typhon/typhon-manualrc, $HOME/.typhon/typhon-emurc and also $HOME/.typhon/typhon-inputrc
  if no other configfile is set with the commandline)
  theme configurations are not saved here!

- one entry above is the "save theme" option, where the themes can be saved

  The themefilepath is set to $HOME/.typhon/themes/ and the to-be-saved
  themename is the one which is currently selected under "theme" (one entry above "save theme).

  ONLY if you set some themename with the commandline, this name will be used
  when saving, no matter which one is currently set under "theme"!
  Take care to not overwrite your commandlinetheme with values of an other one then!

Create new theme:
------------------

  Providing a non-existing themename via commandline, you can create a new one!
  (f.e. "typhon -t testtheme" will create a new testtheme (using default "typhon" values),
  which you can directly edit within the settings menu. 
  "save theme" when done, and the theme is ready).
  alternatively you can of course also simply duplicate one of your themes under a different name
  and use the new one for configuring.

native typhon themes:
-------------------------

import:
--------
archives packed as tgz/tar.gz/tar.bz2 are detected as native typhon themes.
assuming the archive is a valid typhon theme just import it with
typhon -i /path/to/$archive

create:
--------
allowed extensions for the packed typhon theme are tgz/tar.gz/tar.bz2
the themename has to be the archive name (minus the extension suffix)
top level directory in the archive has to be the themename as well
inside this top level directory there have to be
themedata/$themename/$themedatafiles
themes/$themename
and also an INSTALL and a README should be included


Importing external themes:
---------------------------
There's native support for ps3 p3t themes.
You can either launch them via commandline or switch to them in the settings.
The settings also have an option to convert the external theme to a typhon theme.
You can also import an external theme directly via the commandline with:
typhon /path/to/some/<theme>

where <theme> accepts 
- a p3t ps3 theme
- a jpg
- some other zip/rar for non-supported themes

non-supported themes:
----------------------
when installing a non-supported theme useful datafiles are copied to the new themedatadir
and a themeconfig pointing to this themedatadir and having default values will be created.
With just a few clicks the unsupported theme is typhonified. :)
the new theme (and its datadir) will be called like the archivename without the suffix.



Credits:
=========
- Frostworx themselves for the original typhon code
- Frostworx's wife for creating the default typhon theme
- all authors of the used 3rdparty cmake modules (look inside for source url)
- the authors of gource ( http://code.google.com/p/gource ) for their cool freetype fontclasses
- Lucas Soltic for sfeMovie ( https://github.com/Yalir/sfeMovie )
- ehamberg (http://hamberg.no/erlend) for his xrender screenshot code found on https://gist.github.com/767824
- Harleaquin aka superdau for contributions to the original typhon code
- An "anonymous contributor" for the dutch translation
- Kathrin Schaumburg for the default theme's gfx
- The authors of libp3t
- Me (cherrryb0mb) for picking this project up and maintaining it

