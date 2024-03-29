/*
	CSC D84 - Unit 2 - MiniMax search and adversarial games

	This file contains stubs for implementing a MiniMax search
        procedure with alpha-beta pruning. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Sep. 15
*/

#include "MiniMax_search.h"
#include <stdio.h>

#define bigg 6969420

bool debug = false;
int prev[2] = {-1, -1};
double currVal = -bigg;
bool initd = false;
int dists[graph_size][graph_size];

double max(double a, double b)
{
	if (a > b)
		return a;
	return b;
}

double min(double a, double b)
{
	if (a < b)
		return a;
	return b;
}

bool isConnected(int a, int b, double gr[graph_size][4])
{
	return gr[a][b];
}

void fwInit(double gr[graph_size][4])
{
	for (int x = 0; x < graph_size; x++)
	{
		for (int y = 0; y < graph_size; y++)
		{
			dists[x][y] = bigg;
		}
	}

	// init connected
	for (int i = 0; i < graph_size; i++)
	{
		dists[i][i] = 0;
		if (i - 1 >= 0 && isConnected(i, 3, gr))
		{ // L
			dists[i][i - 1] = 1;
		}
		if (i - size_X >= 0 && isConnected(i, 0, gr))
		{ // T
			dists[i][i - size_X] = 1;
		}
		if (i + 1 < graph_size && isConnected(i, 1, gr))
		{ // R
			dists[i][i + 1] = 1;
		}
		if (i + size_X < graph_size && isConnected(i, 2, gr))
		{ // D
			dists[i][i + size_X] = 1;
		}
	}

	for (int k = 0; k < graph_size; k++)
	{
		for (int i = 0; i < graph_size; i++)
		{
			for (int j = 0; j < graph_size; j++)
			{
				if (dists[i][j] > dists[i][k] + dists[k][j])
				{
					dists[i][j] = dists[i][k] + dists[k][j];
				}
			}
		}
	}
}

double MiniMax(double gr[graph_size][4], int path[1][2], double minmax_cost[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, double (*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]), int agentId, int depth, int maxDepth, double alpha, double beta)
{
	/*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in MiniMax_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:

	Exactly the same as for Assignment 1 - have a look at your code if you need a reminder of how the adjacency
	list and agent positions are stored.	

	Note that in this case, the path will contain a single move - at the top level, this function will provide
	an agent with the 'optimal' mini-max move given the game state.


   IMPORTANT NOTE: Mini-max is a recursive procedure. This function will need to fill-in the mini-max values for 
	 	   all game states down to the maximum search depth specified by the user. In order to do that,
		   the function needs to be called with the correct state at each specific node in the mini-max
		   search tree.

		   The game state is composed of:

			* Mouse, cat, and cheese positions (and number of cats and cheeses)
			
		   At the top level (when this function is called by the mini-max driver code), the game state
		   correspond to the current situation of the game. But once you start recursively calling
		   this function for lower levels of the search tree the positions of agents will have changed.
		   
		   Therefore, you will need to define local variables to keep the game state at each node of the
		   mini-max search tree, and you will need to update this state when calling recursively so that
		   the search does the right thing.

		   This function *must check* whether:
			* A candidate move results in a terminal configuration (cat eats mouse, mouse eats cheese)
			  at which point it calls the utility function to get a value
	 		* Maximum search depth has been reached (depth==maxDepth), at which point it will also call
			  the utility function to get a value
			* Otherwise, call recursively using the candidate configuration to find out what happens
			  deeper into the mini-max tree.

   Arguments:
		gr[graph_size][4]   		- This is an adjacency list for the maze
		path[1][2] 			- Your function will return the optimal mini-max move in this array.
		minmax_cost[size_X][size_Y]	- An array in which your code will store the
						  minimax value for maze locations expanded by
						  the search *when called for the mouse, not
						  for the cats!*

						  This array will be used to provide a visual 
						  display of minimax values during the game.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- No alpha-beta pruning
					mode = 1	- Alpha-beta pruning

		(*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]);
				- This is a pointer to the utility function which returns a value for a specific game configuration

				   NOTE: Unlike the search assignment, this utility function also gets access to the graph so you can do any processing 					 that requires knowledge of the maze for computing the utility values.

				  * How to call the utility function from within this function : *
					- Like any other function:
						u = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
						
		agentId: Identifies which agent we are doing MiniMax for. agentId=0 for the mouse, agentId in [1, cats] for cats. Notice that recursive calls
                         to this function should increase the agentId to reflect the fact that the next level down corresponds to the next agent! For a game
                         with two cats and a mouse, the agentIds for the recursion should look like 0, 1, 2, 0, 1, 2, ...
	
		depth: Current search depth - whether this is a MIN or a MAX node depends both on depth and agentId.
		
		maxDepth: maximum desired search depth - once reached, your code should somehow return
			  a minimax utility value for this location.

		alpha. beta: alpha and beta values passed from the parent node to constrain search at this
			     level.

   Return values:
		Your search code will directly update data passed-in as arguments:
		
		- Mini-Max value	: Notice this function returns a double precision number. This is
					  the minimax value at this level of the tree. It will be used 
					  as the recursion backtracks filling-in the mini-max values back
					  from the leaves to the root of the search tree. 

		- path[1][2]		: Your MiniMax function will return the location for the agent's 
					  next location (i.e. the optimal move for the agent). 
		- minmax_cost[size_X][size_Y] 	:  Your search code will update this array to contain the
						   minimax value for locations that were expanded during
						   the search. This must be done *only* for the mouse.

						   Values in this array will be in the range returned by
						   your utility function.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

	/********************************************************************************************************
 * 
 * TO DO:	Implement code to perform a MiniMax search. This will involve a limited-depth BFS-like
 *              expansion. Once nodes below return values, your function will propagate minimax utilities
 *		as per the minimax algorithm.
 *	
 *		Note that if alpha-beta pruning is specified, you must keep track of alphas and betas
 *		along the path.
 *
 *		You can use helper functions if it seems reasonable. Add them to the MiniMax_search.h
 *		file and explain in your code why they are needed and how they are used.
 *
 *		Recursion should appear somewhere.
 *
 *		MiniMax cost: If the agentId=0 (Mouse), then once you have a MiniMax value for a location
 *		in the maze, you must update minmax_cost[][] for that location.
 *
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

	// Stub so that the code compiles/runs - This will be removed and replaced by your code!
	if (!initd)
	{
		fwInit(gr);
		initd = true;
	}

	double nextNodeVal;

	// max depth reached / is a terminal node
	if (depth == maxDepth || checkForTerminal(mouse_loc, cat_loc, cheese_loc, cats, cheeses))
	{
		// return utility of this node
		nextNodeVal = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
		if (agentId)
			minmax_cost[mouse_loc[0][0]][mouse_loc[0][1]] = max(nextNodeVal, minmax_cost[mouse_loc[0][0]][mouse_loc[0][1]]);
		else
		{
			minmax_cost[mouse_loc[0][0]][mouse_loc[0][1]] = min(nextNodeVal, minmax_cost[mouse_loc[0][0]][mouse_loc[0][1]]);
		}

		return nextNodeVal;
	}

	double ret;

	int xW[4], yW[4];
	int x, y;

	if (!agentId)
	{
		ret = -bigg;
		x = mouse_loc[0][0];
		y = mouse_loc[0][1];
	}
	else
	{
		ret = bigg;
		x = cat_loc[agentId - 1][0];
		y = cat_loc[agentId - 1][1];
	}

	xW[0] = 0;						  // top
	xW[1] = gr[x + (size_X * y)][1];  // right
	xW[2] = 0;						  // bottom
	xW[3] = -gr[x + (size_X * y)][3]; //left

	yW[0] = -gr[x + (size_X * y)][0]; // top
	yW[1] = 0;						  // right
	yW[2] = gr[x + (size_X * y)][2];  // bottom
	yW[3] = 0;						  // left
	for (int i = 0; i < 4 && (!mode || alpha < beta); i++)
	{
		// if mouse turn; id = 0
		if (!agentId && xW[i] != yW[i])
		{
			int new_mouse_loc[1][2];
			// let node = the node to explore
			new_mouse_loc[0][0] = mouse_loc[0][0];
			new_mouse_loc[0][1] = mouse_loc[0][1];
			// memcpy(new_mouse_loc, mouse_loc, sizeof(int) * 1 * 2);
			new_mouse_loc[0][0] += xW[i];
			new_mouse_loc[0][1] += yW[i];

			nextNodeVal = MiniMax(gr, path, minmax_cost, cat_loc, cats, cheese_loc, cheeses, new_mouse_loc, mode, utility,
								  agentId + 1, depth + 1, maxDepth, alpha, beta);
			ret = max(ret, nextNodeVal);
			alpha = max(alpha, ret);
		}
		else if (xW[i] != yW[i])
		{ // cat, id > 0
			int new_cat_loc[10][2];
			// let node = the node to explore
			// memcpy(new_cat_loc, cat_loc, sizeof(int) * 10 * 2);
			for (int cat = 0; cat < cats; cat++)
			{
				new_cat_loc[cat][0] = cat_loc[cat][0];
				new_cat_loc[cat][1] = cat_loc[cat][1];
			}
			new_cat_loc[agentId - 1][0] += xW[i];
			new_cat_loc[agentId - 1][1] += yW[i];

			// fprintf(stderr, "cats:")
			nextNodeVal = MiniMax(gr, path, minmax_cost, new_cat_loc, cats, cheese_loc, cheeses, mouse_loc, mode, utility,
								  agentId == cats ? 0 : agentId + 1, depth + 1, maxDepth, alpha, beta);
			ret = min(ret, nextNodeVal);
			beta = min(beta, ret);
		}
	}

	if (!agentId)
	{
		if (minmax_cost[x][y] == 0.0)
			minmax_cost[x][y] = ret;
		else
			minmax_cost[x][y] = min(ret, minmax_cost[x][y]);
	}

	if (depth == 0)
	{ // get largest path from the 4 around:
		double t, l, d, r;
		t = yW[0] ? minmax_cost[x][y - 1] : (double)-bigg;
		l = xW[3] ? minmax_cost[x - 1][y] : (double)-bigg;
		d = yW[2] ? minmax_cost[x][y + 1] : (double)-bigg;
		r = xW[1] ? minmax_cost[x + 1][y] : (double)-bigg;

		int toPick = max(t, l);
		toPick = max(toPick, d);
		toPick = max(toPick, r);

		if (toPick == t)
		{
			path[0][0] = x;
			path[0][1] = y - 1;
		}
		else if (toPick == l)
		{
			path[0][0] = x - 1;
			path[0][1] = y;
		}
		else if (toPick == d)
		{
			path[0][0] = x;
			path[0][1] = y + 1;
		}
		else
		{ // r
			path[0][0] = x + 1;
			path[0][1] = y;
		}

		prev[0] = mouse_loc[0][0];
		prev[1] = mouse_loc[0][1];

		if (true)
			fprintf(stderr, "Final selected Path for this run: @(%d, %d) w/ %f cost\n", path[0][0], path[0][1], minmax_cost[path[0][0]][path[0][1]]);
	}
	return (ret);
}

int manDist(int x1, int y1, int x2, int y2)
{
	return (abs(x1 - x2) + abs(y1 - y2));
}

int closestCheese(int mouse_loc[1][2], int cheese_loc[10][2], int cheeses)
{
	int closest = -1;
	int dist = bigg;
	int temp;

	for (int i = 0; i < cheeses; i++)
	{
		temp = manDist(mouse_loc[0][0], mouse_loc[0][1], cheese_loc[i][0], cheese_loc[i][1]);
		if (temp < dist)
		{
			closest = i;
			dist = temp;
		}
	}

	return closest;
}

bool isOnCheese(int mouse_loc[1][2], int cheese_loc[10][2], int cheeses)
{
	for (int i = 0; i < cheeses; i++)
	{
		if (mouse_loc[0][0] == cheese_loc[i][0] && mouse_loc[0][1] == cheese_loc[i][1])
			return true;
	}
	return false;
}

int countWalls(int x, int y, double gr[graph_size][4])
{
	int count = 0;
	for (int i = 0; i < 4; i++)
		count += !gr[x + size_X * y][i];
	return count;
}

bool isOnMultCheese(int x, int cheese_loc[10][2], int cheeses)
{
	for (int i = 0; i < cheeses; i++)
	{
		if (x == cheese_loc[i][0] + size_X * cheese_loc[i][1])
			return true;
	}
	return false;
}

double utility(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4])
{
	/*
	This function computes and returns the utility value for a given game configuration.
	As discussed in lecture, this should return a positive value for configurations that are 'good'
	for the mouse, and a negative value for locations that are 'bad' for the mouse.

	How to define 'good' and 'bad' is up to you. Note that you can write a utility function
	that favours your mouse or favours the cats, but that would be a bad idea... (why?)

	Input arguments:

		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		depth - current search depth
		gr - The graph's adjacency list for the maze

		These arguments are as described in A1. Do have a look at your solution!
 */

	int x = mouse_loc[0][0];
	int y = mouse_loc[0][1];
	int index = x + y * size_X;

	for (int i = 0; i < cats; i++)
	{
		// if (dists[index][cat_loc[i][0] + cat_loc[i][1] * size_X] < 3)
		if (manDist(x, y, cat_loc[i][0], cat_loc[i][1]) < 4)
		{
			return (double)(-4000 - depth);
		}
	}

	if (countWalls(x, y, gr) == 3 && !isOnCheese(mouse_loc, cheese_loc, cheeses))
	{
		return (double)(-600 - depth);
	}

	int shortest = bigg;
	for (int i = 0; i < cheeses; i++)
	{
		if (dists[index][cheese_loc[i][0] + cheese_loc[i][1] * size_X] < shortest)
		{
			shortest = dists[index][cheese_loc[i][0] + cheese_loc[i][1] * size_X];
		}
	}

	return (double)(300 - shortest - depth);
}

int checkForTerminal(int mouse_loc[1][2], int cat_loc[10][2], int cheese_loc[10][2], int cats, int cheeses)
{
	/* 
   This function determines whether a given configuration constitutes a terminal node.
   Terminal nodes are those for which:
     - A cat eats the mouse
     or
     - The mouse eats a cheese
   debug
   debugns 1, else it returns 0
 */

	// Check for cats having lunch
	for (int i = 0; i < cats; i++)
		if (mouse_loc[0][0] == cat_loc[i][0] && mouse_loc[0][1] == cat_loc[i][1])
			return (1);

	// Check for mouse having lunch
	for (int i = 0; i < cheeses; i++)
		if (mouse_loc[0][0] == cheese_loc[i][0] && mouse_loc[0][1] == cheese_loc[i][1])
			return (1);

	return (0);
}
