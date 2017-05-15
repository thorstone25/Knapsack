// Project 1a: Solving knapsack using exhaustive search
//

#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <stack>
#include <vector>
#include <time.h>
#include <cstdlib>

using namespace std;

#include "d_except.h"
#include "d_matrix.h"
// #include "graph.h"
#include "knapsack.h"

void exhaustiveKnapsack(knapsack &k, int t);
knapsack recursiveKnapsack(knapsack &k, int item, clock_t startTime, int t_limit);

int main()
{
   char x;
   ifstream fin;
   stack <int> moves;
   string fileName;
   
   // Read the name of the graph from the keyboard or
   // hard code it here for testing.
   
   // fileName = "knapsack16.input";

   cout << "Enter filename" << endl;
   cin >> fileName;
   
   fin.open(fileName.c_str());
   if (!fin)
   {
      cerr << "Cannot open " << fileName << endl;
      exit(1);
   }

   try
   {
      cout << "Reading knapsack instance" << endl;
      knapsack k(fin);

      exhaustiveKnapsack(k, 600);

      cout << endl << "Best solution" << endl;
      k.printSolution();
      
   }    

   catch (indexRangeError &ex) 
   { 
      cout << ex.what() << endl; exit(1);
   }
   catch (rangeError &ex)
   {
      cout << ex.what() << endl; exit(1);
   }
}



void exhaustiveKnapsack(knapsack &k, int t)
{	
	// get time reference for start
    clock_t startTime, endTime;
	startTime = clock();
	
	// permute over all possible knapsack combinations
	k = recursiveKnapsack(k, 0, startTime, t);
	
}

knapsack recursiveKnapsack(knapsack &k, int item, clock_t startTime, int t_limit)
{
	// if there are no more items to consider, return this knapsack
	if (item >= k.getNumObjects())
	{
		return k;
	}
	// otherwise, consider the knapsack with/without the item and choose
	
	// placeholder best knapsack
	knapsack k_empty = k;
	for( int i = 0; i < k_empty.getNumObjects(); i++)
	{ k_empty.unSelect(i);	}
	
	knapsack k_with = k_empty;
	knapsack k_without = k_empty;
	
	// if time limit is exceeded, return unchanged knapsack
	int diff = clock()-startTime;
	if( (float)t_limit <  (float)diff / CLOCKS_PER_SEC)
	{	return k_empty; }
	
	// deselect the item
	k.unSelect(item);
	
	// check all knapsacks without the item
	k_without = recursiveKnapsack(k, item + 1, startTime, t_limit);
	
	// if time limit is exceeded, return first knapsack
	diff = clock()-startTime;
	if( (float)t_limit <  (float)diff / CLOCKS_PER_SEC)
	{	return k_without; }
	
	// select the item
	k.select(item);
	
	// check all knapsacks with the item
	k_with = recursiveKnapsack(k, item + 1, startTime, t_limit);
	
	// // // // choose the best legal knapsack // // // //
	bool k_without_legal = k_without.getCost() < k.getCostLimit();
	bool k_with_legal = k_with.getCost() < k.getCostLimit();
	
	if(k_without_legal && k_with_legal)
	{ return ((k_with.getValue() > k_without.getValue()) ? k_with : k_without);	}
	
	if( k_without_legal)
	{ return k_without; }
	
	if( k_with_legal)
	{ return k_with;	}
	
	else
	{ return k_empty;	}
	
}
