![alt text](https://ejohnd98.github.io/images/roguelike.gif "Roguelike")
The gif quality isn't the best, but shows roughly what things look like (as of August 8th 2020)

## About this Project
This project was started in an effort to learn the basics of C++ while creating a simple roguelike. Generally, a roguelike is a tile and turn based game where the player delves into a procedurally generated dungeon filled with enemies and loot ([more in-depth description here](https://en.wikipedia.org/wiki/Roguelike)). 

The type of game was chosen both out of interest, and also for its well defined and complex nature. Most aspects of these games have plenty of online resources to give ideas of how to design things, which means more time is spent on software design than game design (which sounded good for learning a language). 

[SDL](https://www.libsdl.org/) (Simple DirectMedia Layer) is the graphics library being used for the project, and allows for the sprites to be rendered to a window. Everything else has been created from scratch, although [RogueBasin](http://www.roguebasin.com/index.php?title=Main_Page) and the [RoguelikeDev subreddit](https://www.reddit.com/r/roguelikedev/) have been invaluable resources for ideas and inspiration. [This page in particular](http://www.roguebasin.com/index.php?title=Articles) has been quite helpful and gives a pretty good overview of a roguelike's components.
## State of the Game
As for the state of the project, a lot of the work so far has been creating the underlying architecture to support everything. The current design philosophy is to try and future proof each class/feature, while not spending too long overthinking and optimizing things before they might matter (otherwise very little progress would ever be made). That said, a lot of time has been spent recently on refactoring the entire project to implement an ECS system and allow for animations, which were previously impossible.

The ECS implementation I'm using is heavily based on https://austinmorlan.com/posts/entity_component_system/ which helped me better understand how to actually go about writing one up. Entities are essentially just id numbers which correspond to indicies in a given component's array of component data. The linked page provides some good details on the specific implementation.
