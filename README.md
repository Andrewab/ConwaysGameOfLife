How to play:
Upon starting the program the user will be prompted with a series of prompts.

The first prompt will be for the speed of the simulation. 
	- This is the number of seconds between each of the iterations.
	- This value has to be a positive integer
The second prompt will be for the size of the game.
	- This will be taken as one dimension of the two dimensional game.
	- Ex: if user inputs 10 as size then the game will be of 10x10 dimensions.
The third and fourth input are intertwined
	- The third input is for the low end requirement for a point to be on in the next iteration
	- The fourth input is for the high end requirement for a point to be on in the next iteration

If any input is of incorrect value it will print a related error, and then close the application.

Once all prompts have been successfully filled. Sit back and watch as the game commences and 
Prints each iteration of the game based upon the speed of the game.

To end game use the Control + c command to exit program.

Known issues:
For some reason, on larger game boards, the application will occasionally segfault. I am not sure entirely why this is. But the game works fine on smaller game boards.
