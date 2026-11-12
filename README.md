# Catnap-Gallery
A game about a sleepy cat in a museum.











# FILES

## main.cpp

This where the program starts, we set up the window, registers callbacks, and run the game loop


## sprite_rednderer.h

This file tells the compiler what the SpriteRenderer class looks like , what functions and data it has, without saying how they work yet


## sprite_renderer.cpp
Here we say how it works, one quad, one shader, reused for everything.
 All drawn with the same DrawSprite() call, just with different positions, sizes, rotations, and textures/colors.

## game.h

The Game Class Declaration

## game.cpp
The full implementation