# Whack-A-Mole

## Description
Just like the real game (whack-a-mole), this is a game with the same idea, but instead of the hammer and the mole, it is just a group of buttons that had LEDs.
To win, you just have to push the buttons that light up. So it is more like pushing bottons than whacking, but still has the fun.


## Usage
The buttons are put in the electric circuit in a way to act as voltage dividers, for which the microcontroller can identify which button was pressed.
Each player has 3 seconds before the lit button switches, it also switches if pressed.
For each correct button press, the player's score increases by 1 point. The winning score is 50 points.
The new butto to be lit is chosen by a random number generated by the `random` function which is in `GenerateNewNumber` function.
All of the previous happens independently for each player and simultaneously.
