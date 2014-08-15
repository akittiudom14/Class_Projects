Hog Dice Game
=============

In this project, a partner and I created a simulator and a strategy for the dice game, Hog. We implemented higher-order functions, used a random number generator for the dice, and represented the dice using ASCII art. 

The game of Hog consists of two players alternating turns trying to reach 100 points first. On each turn, the current player chooses some number of dice to roll, up to 10. He/She scores the sum of the dice outcomes, unless any of the dice come up a 1 ("Pig Out"), in which case she scores only 1 point for the turn. Our game also featured 5 specials rules:

1. "Hog Tied" - If the sum of both players' scores ends in a seven (e.g., 17, 27, 57), then the current player can roll at most one die.
2. "Hog Wild" - If the sum of both players' scores is a multiple of seven (e.g., 14, 21, 35), then the current player rolls four-sided dice instead of the usual six-sided dice.
3. "Free Bacon" - If a player chooses to roll zero dice, she scores one more than the tens digit of her opponent's score. E.g., if the first player has 32 points, the second player can score four by rolling zero dice. If the opponent has fewer than 10 points (tens digit is zero), then the player scores one.
4. "Touchdown" - If a player's score for the turn is a multiple of 6, then the current player gains additional points equal to the turn score divided by 6. E.g., if the original turn score is 12, the player will earn 14 (12 + 2) points total for the turn, after the Touchdown rule.
5. "49ers" - If a player's score before she rolls is equal to 49, then during that turn, the player will earn the sum of all the dice even if a 1 comes up.

Our strategy was simple, try to force the othe player to get Hog Tied or Hog Wild. If we were down by a large deficit, then take higher risks to earn a lot of points. If 49er's rule is in play, always roll 10 dice to maximize the points, and if we were close to 100, roll zero dice to take zero risks and use Free Bacon rule. 

We implemented code in hog.py, with dice.py and ucb.py as utility files for roling dice/ utility functions. 

- Partner: Song King - 
