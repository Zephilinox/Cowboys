Low-Level Programming
==================
Ths is the brief for the 5th and final game that will form  part of your portfolio. 

\
Prologue
------
"Wait one second!", exclaimed Dave. "You know what would make this game totally awesome!! Networking!!!". The others in the group shot up like a pack of Meerkats. More than aware of his penchant for ambitious ideas, none of them were necessarily convinced it was a good idea. "It'll be a piece of cake" Dave explained, "A piece of cake".

\
Brief
------
This is the fifth and final game that contributes to your portfolio. You are tasked with the creation of a game that uses turn-based networking as it's core mechanic. In small groups you will work to implement your architecture of choice i.e. client-server or P2P. The spec does not dictate the genre of your game nor how it plays out, just the turn-based mechanic that must be present. 


\
Details
------
You will be presented with with two different gameplay mechanics found in two different single player games. They are from well known IPs and are largely focused on the combat within those games. Your team/group will need to pick one and implement a multi-player version of it. You will then be required to create a vertical slice of a game that uses your mechanic. The genre and narrative are not provided. Your team should ensure that your game combines with the implemented networking mechanic to form a larger gameplay experience. The entire game does not have to be fully complete, but it should be of a high enough standard to constitute a vertical slice or a demo. 

The following assumptions on networking conditions are in place
- **clients** - clients are either localhost or on a LAN and the game does not need internet support. 
- **prediction** - you do not need to implement client-side prediction, but can if you wish.
- **cheating** - you do not need to implement anti-cheat solutions.


**Summary: choose a mechanic, create a multi-player version of it and embed in a game of your own making. This will be your own unique IP.**

\
Mechanic 1: Heroes of Might and Magic
------
:imp: :imp: :imp:

Heroes of Might and Magic first came onto the PC gaming scene in 1995. It is a turn based strategy game that has led to a successful franchise. Heroes of Might and Magic takes place in a medieval fantasy world filled with creatures frequently associated with myth and legend. These creatures compose the military forces (troops) with which the player attempts to conquer opponents. The player leads generals through the game world who act as the head of armies of troops. These generals, called "heroes," provide a means to explore, attack, defeat, and acquire, which are the four basic principles in the game. 

The combat is resolved by both player's armies being placed on a game grid. The players take it in turns to move, attack or defend. Not all troops can attack from range nor from all angles. Once the opposition's army is depleted the combat situation is completed. 

###### Screenshot of combat arena
![](https://vignette.wikia.nocookie.net/mightandmagic/images/3/30/CombatPhase_H1.png/revision/latest?cb=20141018083303&path-prefix=en)

**Implementation of this mechanic requires**
- A turn-based navigable game grid for combat (does not need to be isometric)
- Multiple units supported in a single battleground 
- Ability to attack and defend
- Unit stats
- Support for fleeing 
- Armies or grouping of units 
- One action per turn


#### Heroes of Might and Magic: YouTube Video
[![Heroes of Might and Magic](https://img.youtube.com/vi/wEZMoN3S6ns/0.jpg)](https://youtu.be/wEZMoN3S6ns?t=2s "Heroes of Might and Magic")

#### Let's Play Series: Heroes of Might and Magic: YouTube Video
[![Let's Play: Heroes of Might and Magic](https://img.youtube.com/vi/XyltcY2HTag/0.jpg)](https://www.youtube.com/watch?v=XyltcY2HTag&list=PLe0K-GUDQkNIqKR2jAhps4qSzASHEBh6p "Let's Play: Heroes of Might and Magic")

\
Mechanic 2: UFO Enemy Unknown
------
:alien: :alien: :alien:

UFO Enemy Unknown was originally released in 1994 on the PC. The game was a critical success and has a chequered history with its IP. Thankfully Firaxis has managed to release a number of modern takes on it to great aplomb. The game mixes real-time management simulation with turn-based tactics. The player takes the role of commander of X-COM – a clandestine, international paramilitary organization defending Earth from an alien invasion. Through the game, the player is tasked with issuing orders to individual X-COM troops in a series of turn-based tactical missions. It is the tactical missions you will need to recreate with networking support.

Both players are shown a battleground that utilises fog of war for visibility. You are not able to engage with enemies unless you can see them. There is an element of exploration as well as combat in this mechanic. Additionally each solider has a number of attributes that control their ability to move, react, fire at distances, aim etc. A soldier with poor reactions will not return fire automatically.  

**Recently "Mario + Rabbids Kingdom Battle" was released and is an adaptation of the X-COM combat mechanic. Might be of interest to you Nintendo fans!**

###### Screenshots of game
![](http://www.abandonwaredos.com/public/aban_img_screens/ufo-5.jpg)
![](http://i.imgur.com/j7nwp.png)

**Implementation of this mechanic requires**
- A turn-based navigable game grid for combat (does not need to be isometric)
- Multiple units supported in a single battleground 
- Ability to attack and move
- Unit stats
- Fog of War
- Time Units for movement


#### YouTube Videos
[![Let's play X-Com:UFO (part 1)](https://img.youtube.com/vi/6YVFkmAw2Eg/0.jpg)](https://youtu.be/6YVFkmAw2Eg?t=7m4s "Let's play X-Com:UFO (part 1)")

[![Mario & Rabbids Kingdom Battle](https://img.youtube.com/vi/LLRmt79sRuY/0.jpg)](https://youtu.be/LLRmt79sRuY?t=7m3s "Mario & Rabbids Kingdom Battle")

\
Finally
------
**Remember, it is not enough to just create the working turn-based networking mechanic, you are also expected to embed it inside a game demo or vertical slice that your team have sole creative control of.** 