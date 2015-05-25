/*
 * This file contains the maze-solving strategy.
 */

#include <pololu/3pi.h>
#include "follow-segment.h"
#include "turn.h"

// The path variable will store the path that the robot has taken.  It
// is stored as an array of characters, each of which represents the
// turn that should be made at one intersection in the sequence:
//  'L' for left
//  'R' for right
//  'S' for straight (going straight through an intersection)
//  'B' for back (U-turn)
//
// Whenever the robot makes a U-turn, the path can be simplified by
// removing the dead end.  The follow_next_turn() function checks for
// this case every time it makes a turn, and it simplifies the path
// appropriately.
char path[100] = "";
unsigned char path_length = 0; // the length of the path

// Displays the current path on the LCD, using two rows if necessary.
void display_path()
{
	// Set the last character of the path to a 0 so that the print()
	// function can find the end of the string.  This is how strings
	// are normally terminated in C.
	path[path_length] = 0;

	clear();
	print(path);

	if(path_length > 8)
	{
		lcd_goto_xy(0,1);
		print(path+8);
	}
}

// This function decides which way to turn during the learning phase of
// maze solving.  It uses the variables found_left, found_straight, and
// found_right, which indicate whether there is an exit in each of the
// three directions, applying the "left hand on the wall" strategy.
char select_turn(unsigned char found_left, unsigned char found_straight, unsigned char found_right,int d)
{
	if(d%2 == 1)
	{
		if(found_left)
		return 'L';
		else if(found_straight)
		return 'S';
		else if(found_right)
		return 'R';
		else
		return 'B';
	}
	else{
		// Make a decision about how to turn.  The following code
		// implements a left-hand-on-the-wall strategy, where we always
		// turn as far to the left as possible.
		if(found_right)
		return 'R';
		else if(found_straight)
		return 'S';
		else if(found_left)
		return 'L';
		else
		return 'B';
		
	}
}

// Path simplification.  The strategy is that whenever we encounter a
// sequence xBx, we can simplify it by cutting out the dead end.  For
// example, LBL -> S, because a single S bypasses the dead end
// represented by LBL.
void simplify_path()
{
	// only simplify the path if the second-to-last turn was a 'B'
	if(path_length < 3 || path[path_length-2] != 'B')
		return;

	int total_angle = 0;
	int i;
	for(i=1;i<=3;i++)
	{
		switch(path[path_length-i])
		{
		case 'R':
			total_angle += 90;
			break;
		case 'L':
			total_angle += 270;
			break;
		case 'B':
			total_angle += 180;
			break;
		}
	}
	

	// Get the angle as a number between 0 and 360 degrees.
	total_angle = total_angle % 360;

	// Replace all of those turns with a single one.
	switch(total_angle)
	{
	case 0:
		path[path_length - 3] = 'S';
		break;
	case 90:
		path[path_length - 3] = 'R';
		break;
	case 180:
		path[path_length - 3] = 'B';
		break;
	case 270:
		path[path_length - 3] = 'L';
		break;
	}

	// The path is now two steps shorter.
	path_length -= 2;
}
 void time_reset();
 int total = 0;
 int n = 0;
// This function is called once, from main.c.
int mukis[100];
int tates[100];
int yokos[100];

void maze_solve(int d)
{
	int muki =21;
	int tate = 1;
	int yoko =0;
	
	int p=0;
	int q=0;
	
	int j=0;
	int l=0;
	
	//int m=0;
	long int t1,t2 = 0;
	 time_reset();
	 t1 = get_ms();
	 
	// Loop until we have solved the maze.
	while(1)
	{
		clear();
		print_long(d);
		print_long(muki);
		print_long(tate);
		print_long(yoko);
		// FIRST MAIN LOOP BODY  
		follow_segment();
		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		// Note that we are slowing down - this prevents the robot
		// from tipping forward too much.
		set_motors(50,50);
		delay_ms(50);

		// These variables record whether the robot has seen a line to the
		// left, straight ahead, and right, whil examining the current
		// intersection.
		unsigned char found_left=0;
		unsigned char found_straight=0;
		unsigned char found_right=0;

		// Now read the sensors and check the intersection type.
		unsigned int sensors[5];
		read_line(sensors,IR_EMITTERS_ON);

		// Check for left and right exits.
		if(sensors[0] > 100)
			found_left = 1;
		if(sensors[4] > 100)
			found_right = 1;

		// Drive straight a bit more - this is enough to line up our
		// wheels with the intersection.
		set_motors(40,40);
		delay_ms(100);

		// Check for a straight exit.
		read_line(sensors,IR_EMITTERS_ON);
		if(sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
			found_straight = 1;

		// Check for the ending spot.
		// If all three middle sensors are on dark black, we have
		// solved the maze.
		if(sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
			break;

		// Intersection identification is complete.
		// If the maze has been solved, we can follow the existing
		// path.  Otherwise, we need to learn the solution.
		unsigned char dir = select_turn(found_left, found_straight, found_right,d);

		// Make the turn indicated by the path.
		turn(dir);
		switch(dir)
		{
			case 'L':
			muki += 1;
			break;
			
			case 'R':
			muki -= 1;
			break;
			
			case 'B':
			muki += 2;
			break;
			
			case 'S':	
			break;
		}
		switch(muki%4)
		{
			
		case(1):
		tate += 1;
		break;
		case(2):
		yoko -= 1;
		break;
		case(3):
		tate -= 1;
		break;
		case(0):
		yoko +=1;
		break;
		}
		mukis[p]=muki%4;
		tates[p]=tate;
		yokos[p]=yoko;
		l=0;
		for(q=0;q<p;q++)
		{
			if(mukis[q]==muki%4 && tates[q]==tate && yokos[q] == yoko)
			{
			l++;
			}
			
		}
		d=1+l;
		p++;
		
		
		
		// Store the intersection in the path variable.
		path[path_length] = dir;
		path_length ++;
        j++;
		// You should check to make sure that the path_length does not
		// exceed the bounds of the array.  We'll ignore that in this
		// example.

		// Simplify the learned path.
		simplify_path();
/*		
	if(path_length > 10)
		{
			switch(path[path_length-11])
			{
				case 'R':
				l += 1;
				break;
				case 'L':
				l -= 1;
				break;
			}
		}
		
	
		

total=0;
total = total + l;
for(n=1;n<=10;n++)
{
	switch(path[path_length-n])
	{
		case 'R':
		total += 1;
		break;
		case 'L':
		total -= 1;
		break;
		case 'S':
		break;
		
	}
}
if(total==4 || total==8 || total==12 || total== -4 || total== -8 || total== -12)
{
	d++;
}
*/

		// Display the path on the LCD.
		//display_path();
	}
	for(m=1;m<=path_length;m++){
if(path[path_length-m]=='R' && path[path_length-m-1]=='R' && path[path_length-m-2]=='R' && path[path_length-m-3]=='R')
{
	m
	path[path_length-m-3]=path[path_length-m+1]=
}
	}
	// Solved the maze!
    long int t3,t4,t5,t6;
	// Now enter an infinite loop - we can re-run the maze as many
	// times as we want to.
	while(1)
	{
		// Beep to show that we finished the maze.
		set_motors(0,0);
		play(">>a32");
		t2 = get_ms();
		
		t3 = (t2 - t1)/1000;
		t4 = t2 - t1 - t3*1000;
		t5 = t4%100;
		t6 = (t4 - t5)/100;
		
		// Wait for the usto press a button, while displaying
		// the solution.
		while(!button_is_pressed(BUTTON_B))
		{
			if(get_ms() % 2000 < 1000)
			{
				clear();
				print_long(t3);
				print(".");
				print_long(t6);
				lcd_goto_xy(0,1);
				print_long(path_length);
				print("  ");
				print_long(j);
			}
			else
			display_path();
			delay_ms(30);
		
		}
		while(button_is_pressed(BUTTON_B));
	
		delay_ms(1000);
        time_reset();
        t1 = get_ms();
		// Re-run the maze.  It's not necessary to identify the
		// intersections, so this loop is really simple.
		int i;
		//j = 0;
		for(i=0;i<path_length;i++)
		{
			// SECOND MAIN LOOP BODY  
			follow_segment();

			// Drive straight while slowing down, as before.
			set_motors(50,50);
			delay_ms(50);
			set_motors(40,40);
			delay_ms(100);

			// Make a turn according to the instruction stored in
			// path[i].
			turn(path[i]);
		}
		
		// Follow the last segment up to the finish.
		follow_segment();

        
		// Now we should be at the finish!  Restart the loop.
	}
	
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
