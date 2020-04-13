After taking a break from working on the project due to work and end of term homework/exams, I've come back to the project with the goal of refactoring the entire thing. I'm shifting towards an Entity Component System instead of the more OOP style of before. Some code can definitely be resused, but not until later.

The ECS implementation is heavily based on https://austinmorlan.com/posts/entity_component_system/ which helped me better understand how to actually go about writing one up. Entities are essentially just id numbers which correspond to indicies in a given component's array of component data. The linked page provides some good details on the specific implementation.

One of the main goals of this refactoris to make the system more flexible, and also to allow for things like animation, which was not previously possible. I'm also shifting to using shared pointers when possible.
