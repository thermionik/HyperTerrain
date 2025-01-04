
NeHe's great OpenGL framework ported to SDL
--------------------------------------------
porting by Fabio 'SnowDruid' Franchello (snowdruid@libero.it),
13/8/2001

Revision 1: 29/10/2001

1) WHY?! (We were all living peacefully, why you've done this?!)

Why rewrite NeHe's work using SDL?  First  of all,  it makes it PORTABLE!
As  SDL is portable! What  does  it mean?  It  means that you can code an 
Application using this  framework  and SDL under Windows, and  compile it
without changes under Linux and the other OSses SDL supports!

Second, it semplify a lot.  All the  Creation/Destruction/Syncing windows
operations are made by the SDL video layer, using the powerful and unique
SDL_OPENGLBLIT mode. This means that you  have a workspace where  you can
use all standard OpenGL calls as well as all the powerful  SDL 2D graphic
functions! This in a way  add a whole new  dimension to the opportunities
the framework offers.

*** YES!  THIS IS A DOCUMENTED WORKING EXAMPLE OF THE SDL_OPENGLBLIT USE!


2) HOW?! (How do I use it?)

This code has been tested under Windows 98 (compiled with MSVC6)and Linux
(Red Hat 7.0, XFV3.3.3, KDE) only for now.You need SDL library  installed
to compile properly. SDL comes from Sam Lantinga's work and can  be found
at http://www.libsdl.org. You also need an OpenGL implementation for your
OS. If you plan to use Window and have  MS Visual Studio, you  don't have
to search, you already got the OpenGL libraries. You can found the latest
version of OpenGL at  http://www.opengl.org, anyway. If  you want  to use
Linux, you can  look at  http://www.mesa3d.org to find a free open-source
implementation of OpenGL,or take a look at http://www.opengl.org as well.
I'm still  waiting to test it on Solaris 8 but the silly responsable that
works in  my University's  computer lab still  promises  it  will install
the OpenGl libs on the file server soon(he keeps promising since 6 months
by now).

Building tips:

Windows (MS Visual Studio 6):

- Open the VisualC.zip and extract the files into the same  directory  of
  this file you're reading.
- Double-click on the ConsoleFrame.dsw file. Visual Studio will  open and
  all the source files will be loaded.
- Go to Project->Settings: the project settings window will open.
- On the right,  you can  change the  settings for Debug, Release or Both
  Configurations
- Hit the "C/C++" Tab and choose the "Preprocessor" Category.
- Under "Additional include directories:" change the  entry with  the dir
  where you have SDL includes installed.
- You can add "ASK_FULLSCREEN" to the list in "Preprocessor definitions:"
  to make the program ask you if you want to use fullscreen or not at run
  time. You can add "FULLSCREEN" to  the  same list if you always want to
  start in fullscreen mode.  If  you don't add anything, you'll end up in
  windowed mode.
- Now go in the "Code Generation" Category: under "Use run-time  library"
  you have to choose either  "Multithreaded DLL"  (if your  SDL was built
  as a "Release" application) or  "Debug Multithreaded DLL"  (if your SDL
  was build as a "Debug" application).
- Now hit the "Link" Tab on the top and go in the "Input" category.
- Under  "Additional library path:" change  the entry  with the directory
  where you have SDL .lib files installed.
- You're done, hit "OK".
- Hit F7 to build!
- Once you've the application build, remember to copy SDL.dll in the same
  directory for the application to work (or  you can  copy it to Windows\
  System, the default place when looking for a .dll)
- Watch the Demo. Yah! It sucks! Who made it? You can do alot better!
  Prove it. :) Edit app.cpp and create your own application! Have fun! :)


Linux (gcc + X11):

- Unzip the archive and cd into the created directory.
- Edit "Makefile", at the line beginning with "CPPFLAGS="
  this should look like:

  CPPFlags = -Wall -g -DLINUX `sdl-config --cflags`

  you can add -DFULLSCREEN in the line to always start in fullscreen mode

  (IE: like this:
   CPPFLAGS = -Wall -g -DLINUX -DFULLSCREEN `sdl-config --cflags`
  )

  you can skip this point if you want to start in windowed mode.
- Type "make depend".
- Type "make".
- That's it. Run the executable.
- Watch the Demo. Oh. I see. You can code a better application of of that
  framework (Linux coders are in a way  more calm).  Prove it, anyway. :)
  Fill in app.cpp and create your application. Have fun! :)


Other systems will follow as soon as I can test them. :)

  
3) WHAT?! (There's something new this version offers?)

Yes. I've added a pretty basic set of functions for error logging.You can
call the Log() function anytime in your application. The log  output goes
in a file  named by the  LOG_FILE  symbol  in main.h. You  can  obviously
change the name to fit your needs.You can even add another set of logging
functions to have different log who work over different types of messages
(IE: one for the graphic errors, one for the application errors,  one for
everything  you want).  I'm  planning to  expand  this  feature in future
releases, anyway, as the project is not dead (see the TODO.txt file).


4) HUH?! (What's left behind?)

Do  this version offer all the features the original  code offers?  Well, 
almost. All the holes will be closed hopefully. For now, what feature the
code do NOT offer?

- For  Win32 users,  the F1 key to toggle between fullscreen and windowed
  mode do not work, as SDL does not support the feature under Windows. If
  I'll find a workaround, I'll add it.
- For Linux users, the ability to choose the starting mode (fullscreen or
  windowed)  at runtime  can't be used. You have  to change this thing at
  compile time. (see above).


5) WHO?! (Who's to blame for this?)

The  porting was  made by me, Fabio Franchello (snowdruid@libero.it).

The original WIN32 source comes from the hard work of Jeff Molofee,  with
help from Maxwell Sayles & Peter Puck (praise to them all!).

The  awesome Simple Directmedia Library (SDL) comes from  the efforts  of
Sam Lantinga (praise to him!).

Can you help?  YES YOU CAN! :) If you have ideas or advices, feel free to
email me. :)


6) EH! (A little note on SDL_OPENGLBLIT)

As a matter if fact SDL_OPENGLBLIT  mode is not fast. Is not  the fastest
mode you can use. But have another great advantage IMHO:it's easy to use.
If you use the glOrtho() GL call  to set up a proper ortho viewport  (and
so use  IE GL Quads to  display "sprites")  you will  obtain a  much more
smooth running code. So which mode to use?  It depends  on what you need.

If   you  need  an  OpenGL  scene  with  some  (maybe fixed) 2D elements,
like a IE a GUI. SDL_OPENGLBLIT  is the way to go.  If  you're needing  a
3D scenery for a mainly 2D action  game,  maybe you  should try glOrtho()
technique.  In  the next release  of  this framework I'll add both modes.

Have a nice day!

-------------------------------------
(c) Fabio 'SnowDruid' Franchello 2001
