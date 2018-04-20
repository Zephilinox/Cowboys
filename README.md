Low-Level Programming
==================

\
Setup
------

If playing locally, simply open 2 instances of the client.exe AFTER building the solution in visual studio. One user must select server, the other, client. Next, users choose their warband/gang members. Only the user that is running the server can begin the match. Chat is available in the lobby.

If playing online, edit the settings.json file, changing the "Server IP" : "localhost" to "Server IP" : "<*insert server player's IP here". Then follow the above, opening 1 instance per PC.

\
Gameplay
------
This game is a turn-based showdown to death!

Users control a warband of cowboys! Left mouse button press on a cowboy to select him. If the cowboy is on your team, it is highlighted in the units panel at the bottom of your screen. If the cowboy is an enemy, a small panel will slide in from the top right, showing the available stats of that enemy. 

To act, use the right mouse button with a cowboy selected. The context senstive actions available are:
- Plan your move - A path is displayed, showing the movement cost in time units and the path your unit will take. Clicking on a different tile will re-calculate the path.
- Make your move - Clicking on the same tile again will make your unit move there.
- Attack! - right clicking on an enemy cowboy will cause your cowboy to attack! But only if he has enough time units. But be careful, if an enemy has time units to spare, they may well shoot back!

Scroll the screen with WASD or by moving the mouse to the edge of the screen.

Press Escape to open the pause menu. From here, you can toggle the music, continue playing or exit the game.

F1 - Fullscreen
F2 - Toggles FPS
F3 - Toggles FPS cap

\
Additional Documents
------
The complete class diagram for this project is in the root of the repository /download.

Finally
------
Thanks for playing.