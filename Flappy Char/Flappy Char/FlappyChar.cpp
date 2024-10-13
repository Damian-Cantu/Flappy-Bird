#include <iostream>
#include <fstream>
#include <conio.h> //windows specific for console input/output _kbhit, _getch
#include <Windows.h> //windows specific, used to acess console screen buffer
#include <time.h>
using namespace std;

const int mapWidth = 40; //width of map
const int mapHeight = 20; //height of map
char map[mapWidth][mapHeight]; //2D array for the map 
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); //sets handle to standard output handle, access to the console screen buffer

//prototypes
void gotoxy(int column, int line);
void hidecursor();
void timer(int t);
void spawn(unsigned char c, double x, double y);
void pipes(double pipex, double pipey, int pipeWidth, int pipeGap, unsigned char pipe = 219);
template<class T>
void displayText(T text, int x, int y);
void pressContinue(string text, char input, int x, int y);
void hideScore(int x, int y);

int main()
{
	do
	{
		//sets color attribute of screen buffer
		SetConsoleTextAttribute(h, 15); //whole screen set to black (for the case that restarting the loop ingame causes background colors to display outside the map)
		srand((unsigned int)time(NULL)); //for rand

		bool game = false; //game set to false
		char choice; //variable that holds key input

		double velocity = 1.9; //velocity set prior to game so bird can jump at start
		double birdx = mapWidth / 2; //bird x coordinate
		double birdy = mapHeight / 2; //bird y coordinate

		int pipeWidth = 4; //width of the pipes
		int pipeGap = 3; //size of the gap between top and bottom pipes

		double pipex = mapWidth; //pipe x coordinate
		double pipey = rand() % 11 + 3.0; //pipe y coordinate (set to random so that pipe setups can vary)

		double fakeClear = pipeWidth; //fake pipe width
		double fakex = 0; //fake pipe x coordinate
		double fakey = 0; //fake pipe y coordinate

		int newscore = 0; //current score
		int highscore = 0; //highscore from previous game

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		ofstream outFile;
		ifstream inFile;

		system("cls"); 
		hidecursor(); //hides blinking cursor during game

		for (int y = 0; y < mapHeight; y++) //loop for y coordinates on the map COLUMN
		{
			for (int x = 0; x < mapWidth; x++) //loop for x coordinates on the map ROW
			{
				map[x][y] = ' '; //grabs every chracter on the map x&y
				SetConsoleTextAttribute(h, 150); //bakcground of whole map is cyan (150) pretty skies
				cout << map[x][y]; //displays the map
			}
			cout << endl; //breaks the map rows after the rows characters have printed
		}

		spawn(254, birdx, birdy); //spawn bird (extended ascii char 254)

		inFile.open("highscore.txt"); //open hihghscore file
		inFile >> highscore; //store it as highscore
		inFile.close(); //close file

		SetConsoleTextAttribute(h, 14 | 150); //foreground text is off white (14), background text is cyan (150)

		//text displays
		displayText("High Score", mapWidth / 2 - 5, 3);
		displayText(highscore, mapWidth / 2 - 1, 4);
		pressContinue("Press Space Bar to Fly . . .", ' ', mapWidth / 2 - 14, mapHeight / 2 + 5);

		game = true; //game is true after space bar is pressed

		//clear text displays
		displayText("                                ", mapWidth / 2 - 14, mapHeight / 2 + 5);
		displayText("          ", mapWidth / 2 - 5, 3);
		displayText("    ", mapWidth / 2 - 1, 4);

		while (game)//while loop for the game
		{
			timer(4500); //4500 sets speed of game (higher goes slower, lower goes faster)

			//gravity
			SetConsoleTextAttribute(h, 150); //make sure background is cyan
			spawn(' ', birdx, birdy); //clear current bird
			birdy += 0.8;  //0.8 add to birds y coordinate (constantly moving down like gravity)
			spawn(254, birdx, birdy); //spawn bird at new coordinate

			//key press
			if (_kbhit()) //checks if the keyboard is pressed
			{
				choice = _getch(); //gets the pressed key and assigns it to variable choice
				if (choice == ' ') //if space bar pressed
				{
					velocity = 1.9; //1.9 velocity is set for flight
				}
			}

			//fly
			if (velocity > 0) //0 if velocity is greater than 0
			{
				spawn(' ', birdx, birdy); //clear current bird
				birdy -= velocity; //subtract birds y coordinate by velocity (bird moves up by velocity until velocity = 0)
				spawn(254, birdx, birdy); //spawn bird at new coordinate
				velocity -= 0.4; //velocity is decresed
			}

			//erase fake pipe at left side
			if (map[0][0] != ' ')  //if map at 0, 0 is not clear, then there is a pipe
			{
				SetConsoleTextAttribute(h, 150); //make sure background is blue
				fakeClear--; //decrease fake pipe width (this is because map edge starts at 0, so fakeClear runs from (fakeClear - 1 to 0 right to left))
				for (int j = 0; j < mapHeight; j++) //from 0 down to map edge
				{
					spawn(' ', fakeClear, j); //clear pipe at fakeClear, j
				}
			}
			else //if fake pipe is cleared
			{
				fakeClear = pipeWidth; //reset fakeClear value
			}

			//spawn fake pipe, spawn new pipe, score
			if (pipex == birdx) //if pipex is same ar birdx
			{
				//spawn fake pipe
				fakex = birdx; //fakex is set to birdx
				fakey = pipey; //fakey is set to birdy
				//spawn new pipe
				pipex = mapWidth; //pipex restarts 
				pipey = rand() % 11 + 3.0; //pipey randomly calculated 

				//add score
				newscore++; //add to score
				//if score beats high score
				if (newscore > highscore) //if current score is larger than highscore
				{
					outFile.open("highscore.txt"); //open highscore file
					outFile << newscore; //update highscore
					outFile.close(); //close file
				}
			}

			//pipe scroll
			pipes(pipex, pipey, pipeWidth, pipeGap, ' '); //clear pipe
			pipex--; //move pipe to left
			SetConsoleTextAttribute(h, 10); //color pipe green
			pipes(pipex, pipey, pipeWidth, pipeGap); //spawn pipe at new 
			//score display
			SetConsoleTextAttribute(h, 15);
			displayText(newscore, mapWidth / 2, 2);

			//fake pipe scroll
			if (fakex != 0) //if fakex is updated
			{
				SetConsoleTextAttribute(h, 150); //make sure background is blue
				pipes(fakex, fakey, pipeWidth, pipeGap, ' '); //clear current pipe
				fakex--; //move fake pipe left
				SetConsoleTextAttribute(h, 10); //color fake pipe green
				pipes(fakex, fakey, pipeWidth, pipeGap); //spawn fake pipe at new location
				//score display
				SetConsoleTextAttribute(h, 15);
				displayText(newscore, mapWidth / 2, 2);
			}

			//collision YOU LOSE
			//if birdy does not hit the top or bottom of the map, or if the map at the birds coordinates is equal to a pipe
			if (birdy < 0 || birdy > mapHeight - 1.0 || (unsigned char)map[int(birdx)][int(birdy)] == 219) //254 219
			{
				//bird is dead and wait
				SetConsoleTextAttribute(h, 10 | 70); //bird foreground to yellow and background to red
				spawn('X', birdx, birdy); //spawn bird as an x
				timer(10000); //wait a lil after bird dies

				//bird falls to floor
				while (birdy < mapHeight - 1.0) //while bird has not hit the bottom of the map
				{
					timer(3000); //timer between birds descent

					//if the bird is on a pipe
					//if there is no blank space to the left of the bird, or if there is no blank space to the right of the bird and birdx is outside of pipex 
					if (map[int(birdx - 1)][int(birdy)] != ' ' || map[int(birdx + 1)][int(birdy)] != ' ' && birdx >= pipex)
					{
						//bird falls green
						SetConsoleTextAttribute(h, 10); //set color green
						spawn(219, birdx, birdy); //clear bird
					}
					else //else no bird on the pipe
					{
						//bird falls blue
						SetConsoleTextAttribute(h, 150); //set color blue
						spawn(' ', birdx, birdy); //clear bird
					}

					birdy++; //increase bird y position (moves down)
					//dead bird
					SetConsoleTextAttribute(h, 10| 70); //yellow with red
					spawn('X', birdx, birdy); //spawn dad bird at new coordinates

					//display score ontop of bird if it dies at same position at the score
					SetConsoleTextAttribute(h, 15);
					displayText(newscore, mapWidth / 2, 2);
				}
				//end game
				game = false; //end game loop
			}
		}

		//hide score up to four digits
		hideScore(mapWidth/2, 2);

		//end menu box display
		SetConsoleTextAttribute(h, 14 | 450); //yellow text, salmon background
		for (int i = 0; i < 13; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				displayText(' ', i + mapWidth/2 - 6, j + 5);
			}
		}

		//update highscore
		inFile.open("highscore.txt");
		inFile >> highscore;
		inFile.close();

		//end menu text display
		displayText("GAME OVER", mapWidth / 2 - 4, 6);
		displayText("SCORE", mapWidth / 2 - 2, 8);
		displayText(newscore, mapWidth / 2, 9);
		displayText("HIGH SCORE", mapWidth / 2 - 4, 11);
		displayText(highscore, mapWidth / 2, 12);
		pressContinue("Press Enter Key to Continue . . .", 13, mapWidth / 2 - 16, mapHeight / 2 + 5);

	} while(true);
	return 0;
}

void gotoxy(int column, int line) //goes to screen coordinates
{
	COORD coord; //COORD structure defines coordinates of console screen buffer
	coord.Y = line; //assign y coordinate to coord.y
	coord.X = column; //assign x coordinate to coord.x
	SetConsoleCursorPosition(h , coord); //sets the cursor position in the specified console screen buffer
}

void hidecursor() //hides cursor on screen
{
	CONSOLE_CURSOR_INFO info; //structure defines info for the console cursor
	info.dwSize = 100; //size of cursor
	info.bVisible = FALSE; //set cursor visibility to false
	SetConsoleCursorInfo(h, &info); //set changes to console cursor
}

void timer(int t) //timer function
{
	for (int x = 0; x < t; x++) //loop that does nothing actually
	{
		gotoxy(0, 0); //sets the cursor to these coordinates for however big t is
		//creates illusion of a timer
	}
}

void spawn(unsigned char c, double x, double y) //spawn function takes in char and x, y coordinates
{
	gotoxy(int(x), int(y)); //goto coordinates
	cout << c; //display specified char
	map[int(x)][int(y)] = c; //update map at coordinates to char
}

void pipes(double pipex, double pipey, int pipeWidth, int pipeGap, unsigned char pipe) //loads in top and bottom pipe
{ //takes in x, y coordinates, pipewidth and pipegap, takes in char for what pipe looks like (219) by default
	int dist = 0; //distance between pipex and the wall
	if (pipex + pipeWidth < mapWidth) //if the whole pipe is within the visible map
	{
		for (int i = 0; i < pipey; i++) //from 0 on the y axis, all the way until pipey
		{
			for (int j = int(pipex); j < pipex + pipeWidth; j++) //from pipex on x axis, all the way until the pipes width is reached
			{
				spawn(pipe, j, i); //spawn a pipe on designated loop coordinates
			}
		}

		for (int i = int(pipey) + pipeGap; i < mapHeight; i++) //from the coordinate after the pipegap + pipey, all the way until the end of the map is reached
		{
			for (int j = int(pipex); j < pipex + pipeWidth; j++) //from pipex on x axis, all the way until the pipes width is reached
			{
				spawn(pipe, j, i); //spawn a pipe on designated loop coordinates
			}
		}
	}
	else //if all or part of the pipe is out of visible map screen
	{
		dist++; //add one to distance counter
		for (int i = 0; i < pipey; i++) //from 0 on the y axis, all the way until pipey
		{
			for (int j = int(pipex); j < mapWidth - dist + 1; j++) //from pipex on x axis, all the way to the edge of the map, considering in the distance between pipex and the edge
			{
				spawn(pipe, j, i); //spawn a pipe on designated loop coordinates
			}
		}

		for (int i = int(pipey) + pipeGap; i < mapHeight; i++) //from the coordinate after the pipegap + pipey, all the way until the end of the map is reached
		{
			for (int j = int(pipex); j < mapWidth - dist + 1; j++) //from pipex on x axis, all the way to the edge of the map, considering in the distance between pipex and the edge
			{
				spawn(pipe, j, i); //spawn a pipe on designated loop coordinates
			}
		}
	}
}

template<class T>
void displayText(T text, int x, int y) //displays text at x, y coordinates
{
	gotoxy(x, y); //goto coordinates
	cout << text; //display text
}

void pressContinue(string text, char input, int x, int y) //press specified key to continue
{
	char c = 'c';
	displayText(text, x, y); //display message
	while (c != input) //while c is not desired input
	{
		c = _getch(); //get keyboard input
	}
}

void hideScore(int x, int y) //hides ingame score after death 
{
	for (int i = 0; i < 4; i++) //hides up to 4 digits on the score
	{
		if ((unsigned char)map[x + i][y] == 219) //if the score is ontop of a pipe
		{
			SetConsoleTextAttribute(h, 10); //set green
			displayText(char(219), x + i, y); //clear text
		}
		else //ontop of sky
		{
			SetConsoleTextAttribute(h, 150); //set blue
			displayText(' ', x + i, y); //clear text
		}
	}
}