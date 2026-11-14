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









# STORY

a sleepy museum cat living inside the Royal Grand Gallery of Art. Inside the restoration workshop sits the C.A.T.Scan (Canvas Artifact Teleportation Scanner), an experimental device used to analyze classical pigments.

One night, a mysterious anomaly tears through the gallery's prized historical canvas, "The Battlefield of Knights". The painting's golden keystone puzzle piece is lost inside the painted world, leaving behind an empty grey canvas with a missing slot on the museum wall.

While napping near the scanner, Barnaby is pulled through the portal and enters the painting! Transformed into a warrior cat with a boomerang blade, Barnaby must jump onto the enemy commander's steed, retrieve the lost golden puzzle artifact, and bring it back through the scanner to restore the museum masterpiece.

In the upcoming 3D project, Barnaby will explore fully 3D wonderland worlds inspired by master painters such as Van Gogh, Monet, etc.

## Controls
- A/D : walk left/right flipping cat's facing direction
- SPACE : JUMP (gravity used, can jump from ground or off the boss ' s head)
- Right Mouse CLick : Throw boomerang sword
- E: Near the C.A.T SCan in the MUSEUM SCENE jump intto PAINTING SCENE

## Technical requirements
1. COMPLEX ANIMATIONS:  
    - Bommerang sword flight and spin: linear flisght translation, rotation around it on center continiously (swordRotationAngle += 1080.0f * dt)
    -FLoatin Puzzle Piece: SMooth vertical levitation(harmonic by using (in(puzzleTimer * 2.5f) * 0.06f))
    - Camera FOllow & OnGoing background: linear interpolation camera follow (cameraX += (target - cameraX) * 5.0f * dt) with continuous background wrapping via fmod

2. Original Algorithms
 
 - Platformer Jump & Dynamic Boss Platforming
    - Euler gravity integration: velocityY += gravity * dt; catPos.y += velocityY * dt;
    - Dynamic platform landing on the moving boss: if falling (velocityY <= 0), the cat rides the boss (catPos.x += bossSpeed * bossDir * dt), takes 0 damage while on top, and can jump again from his head.
- Health, Immunity & HUD System:
    - Knockback and temporary invulnerability timer (catHurtTimer) on hit.
    - Discrete boss health formula: (int)ceil((bossHealth / bossMaxHealth) * 3.0f) dynamically updating health segments.

3. User input Handling:
- keyboard control A,D,E, SPACE and E for interation with single-press debouncing
- Mouse controls (Roght click sword attack)

4. Multiple Complex Shapes Built from Primitives
Built procedurally using modern OpenGL primitives (whiteTexture quads):
1.  The Museum FRamed Painting (DrawFramedPainting - 15 primitives)
    - torn dark grey canvas with a cutout slot and 4 alignment marks.
    - Gilded baroque frame with 4 border bars and 4 ornate corner studs.
    - Engraved brass museum plaque with mounting screws.
    - Dynamic gameplay link: Once the piece is collected in battle, the missing slot automatically lights up with the restored golden piece!

2. The Jigsaw Puzzle Artifact (DrawPuzzlePiece - 14 primitives):
    - Central golden square with 2D beveled highlights and shadows.
    - Interlocking tabs with narrow necks and wide heads (top & right).
    - Cutout sockets with inner depth shadows (left & bottom).
    - Central magical glowing diamond gem rotated at 45°.
