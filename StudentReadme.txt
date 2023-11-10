# University of Leeds 2023-24
# COMP 5823M Assignment 1 Flight Simulator

Ahmad Shahbaaz Hussain
Student ID: 201802541

A makefile is provided which is generated following the instructions provided with the assignment. The makefile was generated on Mac. A new makefile might need to be generated for when the program is being tested on a different machine using the same methods described in the readme.txt file provided in the assignment. I have listed the instructions here again
for convenience. 

I recommend following these instructions to generate a new makefile and compiling the code
Using the new makefile. This is what worked for me.

UNIVERSITY LINUX:
=================

The university machines have QT 5.15.2 installed.

To compile on the University Linux machines, you will need to do the following:

[userid@machine A1_handout]$ module add legacy-eng
[userid@machine A1_handout]$ module add qt/5.15.2
[userid@machine A1_handout]$ qmake -project QT+=opengl LIBS+=-lGLU
[userid@machine A1_handout]$ qmake
[userid@machine A1_handout]$ make

You should see a compiler warning about an unused parameter, which can be ignored.

To execute the programe:

[userid@machine A1_handout]$ ./A1_handout 

CONTROLS
========

WD: Controls yaw
AS: Controls pitch 
QE: Controls roll
V: Switches to follow camera
-: Decreases speed
+: Increases speed (this might require you to press shift and + keys depending on keyboard)

Player collisions:
=================
* Ground collision
* Lava bomb collision
* Collision with other planes in the scene

World collisions:
=================
* AI planes collide into each other; when they do, they change colour (this is to make sure you always have a chance to see the collision instead of destroying them and having to restart game to see it)
* Lava bombs collide with terrain, damaging it.
* Lava bombs collide with each other, pushing each other, causing change of colour