Ants VS. SomeBees (Python)
=================

A partner and I created a tower defense game called Ants Vs. SomeBees. The game consists of a queen bee that must populate the colony with ants to defend her against the attacking bees. The colony is made of 8 spaces as seen in gameplay.png. Each ant has a special attribute, and requires a different amount of food to place it. During each turn the bees advance towards the end of the colony (where the Queen is) unless there is an ant inpeding it - which it will sting and kill. 

This project combines functional and object-oriented programming, and requires the understanding, extending, and testing of a large program with many related parts. All of the ants and bees are sublasses of the Insect class, and all ants are subclasses of the Ant subclass. Each special type of ant is described and documented in the code. 

All game logic and classes are implemented in ants.py, with the following files as utility files:
- tests.py: and more_tests.py: A series of unit tests to test various parts of the project
- ants_gui.py: graphics for the game
- graphics.py: general functions for displaying simple two-dimensional animations
- ucb.py: utility functions for the course
- img: directory of images used by the graphical version of the game

Acknowledgements: Tom Magrino and Eric Tzeng developed this project with John DeNero. Jessica Wan contributed the artwork. Joy Jeng and Mark Miyashita invented the queen ant.

- Partner: Song King
