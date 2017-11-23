CONTROLS:

--L: Start a local game
--C: Connect to the server to play multiplayer
--R: Replay after game over
--S: Drop bombs (only player on top can do this)

Both players can only move horizontally. IF LOCAL: A/D and J/L to move left and right. IF NETWORKED: Only A/D
The top player's goal is to drop bombs with S onto the bottom player. If they get hit, the score gets incremented. Once score reaches 10, game over.

EXTRA IMPORTANT::

We couldn't figure out why sometimes you can't do anything when you run the game. We found that cleaning//rebuilding and deleting the 'd' key and retyping it
seems to be the fix. So if it doesn't work the first time, try deleting the 'd' key and retyping that and hopefully that will work!! WE PROMISE

Also I deleted all of the garbage files from the project, but I think whenever I do that you have to re-set the client and server to be start up in the solution properties, and 
I wasn't sure which garbage file was deleting that information, so that is also a step to do before you run it!!