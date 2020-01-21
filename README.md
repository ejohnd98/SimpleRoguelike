![alt text](https://ejohnd98.github.io/images/roguelike.gif "SimpleRoguelikeGif")

## About this Project
This project was started in an effort to learn the basics of C++ while creating a simple roguelike. Generally, a roguelike is a tile and turn based game where the player delves into a procedurally generated dungeon filled with enemies and loot ([more in-depth description here](https://en.wikipedia.org/wiki/Roguelike)). 

The type of game was chosen both out of interest, and also for its well defined and complex nature. Most aspects of these games have plenty of online resources to give ideas of how to design things, which means more time is spent on software design than game design (which sounded good for learning a language). 

[SDL](https://www.libsdl.org/) (Simple DirectMedia Layer) is the graphics library being used for the project, and allows for the sprites to be rendered to a window. Everything else has been created from scratch, although [RogueBasin](http://www.roguebasin.com/index.php?title=Main_Page) and the [RoguelikeDev subreddit](https://www.reddit.com/r/roguelikedev/) have been invaluable resources for ideas and inspiration. [This page in particular](http://www.roguebasin.com/index.php?title=Articles) has been quite helpful and gives a pretty good overview of a roguelike's components.
## State of the Game
As for the state of the project, a lot of the work so far has been creating the underlying architecture to support everything. The current design philosophy is to try and future proof each class/feature, while not spending too long overthinking and optimizing things before they might matter (otherwise very little progress would ever be made).

As of the time writing this, the game consists of a dungeon, which holds a number of maps. Each map consists of tiles which are generally either walls, empty, or contain an actor (ie. player or enemies) or prop (ie. doors, stairs). When the player gives an input, they take their action, and then advance the turn (other actors now act). Only tiles in the players line of sight are shown to the player, and enemies are similarly not aware of the player until seeing them. Only the most basic combat system in currently in place, where each actor has a health value and attack value. A rather basic map generator is currently in use which randomly places 5 rooms, then carves corridors to connect some of them. This number is currently hardcoded, but will be changed to be more flexible soon enough. Just trying to go for the minimum functionality to be viable before fleshing things out. Placeholder enemies are also randomly placed throughout the rooms, and doors separate them.
