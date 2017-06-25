// Project 5: Solving knapsack using Local Search Algorithms

#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <stack>
#include <time.h>
#include <cstdlib>
#include <algorithm>    // std::sort
#include <vector>       // std::vector

using namespace std;

#include "d_except.h"
#include "d_matrix.h"
// #include "graph.h"
#include "knapsack.h"

knapsack branchAndBoundKnapsack(knapsack &k, clock_t startTime, int t_limit);
knapsack greedyKnapsack(knapsack &k, int index);
void exhaustiveKnapsack(knapsack &k, int t);
knapsack recursiveKnapsack(knapsack &k, int item, clock_t startTime, int t_limit);
void generateOutput(knapsack &k, string filename, float runtime);
int RunAlgorithm(string filenameext);
bool timeExpired(int t_limit, clock_t startTime);
bool updateChampion(knapsack &, knapsack &);
knapsack bestNeighbor(knapsack &k);
knapsack bestNeighbor(const knapsack &k, bool type); 
knapsack steepestDescentKnapsack(knapsack &k, const clock_t startTime, const int t_limit);
knapsack geneticKnapsack(knapsack &k, const clock_t startTime, const int t_limit);


int main()
{
	RunAlgorithm("knapsack8.input");
	RunAlgorithm("knapsack12.input");
    RunAlgorithm("knapsack16.input");
    RunAlgorithm("knapsack20.input");
    RunAlgorithm("knapsack28.input");
	/*
	RunAlgorithm("knapsack32.input");
    RunAlgorithm("knapsack48.input");
    RunAlgorithm("knapsack64.input");
    RunAlgorithm("knapsack128.input");
    RunAlgorithm("knapsack256.input");
    RunAlgorithm("knapsack512.input");
	RunAlgorithm("knapsack1024.input");
    */
	return 0;

}


int RunAlgorithm(string filenameext)
{
	ifstream fin;
	stack <int> moves;
	string filename;

	// Read the name of the graph from the keyboard or
	// hard code it here for testing.

	//cout << "Enter filename" << endl;
	//cin >> filenameext;

	filename = filenameext.substr(0, filenameext.find_last_of("."));

	fin.open(filenameext.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << filenameext << endl;
		exit(1);
	}

	try
	{
		cout << "Reading knapsack instance: " << filenameext << endl;
		knapsack k(fin);

		// get time reference for start
		clock_t startTime = clock();

		// run the algorithm
		cout << "Optimizing . . ." << endl;
		knapsack solution = geneticKnapsack(k, startTime, 15);

		// report back to user
		// solution.printSolution();
		unsigned long diff = clock() - startTime;
		float runTime = (float)diff / CLOCKS_PER_SEC;
		cout << endl << "Genetic Algorithm Total Runtime: " << runTime << "s" << endl << endl << endl;
		generateOutput(solution, filename, runTime);
		return 0;
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

knapsack geneticKnapsack(knapsack &k, const clock_t startTime, const int t_limit)
{
	/* create a population of knapsacks */
	vector<knapsack> population;
	knapsack b = k; // champion knapsack
	int pop_size = k.getNumObjects() / 4;
	pop_size = (pop_size < 8) ? 8 : pop_size;
	pop_size = (pop_size > 50) ? 50 : pop_size;
	float threshold = k.getNumObjects() * 10;
	int update_count = 0;
	bool updated = false;

	// randomize each knapsack
	for (int i = 0; i < pop_size; i++)
		population.push_back(knapsack(k, 1));

	// sort population by best knapsack
	sort(population.begin(), population.end()); 
	reverse(population.begin(), population.end()); // first element is the best
	// 
	while ((!timeExpired(t_limit, startTime)) && (k.isLegal()) && (update_count < threshold)) // until time expires or very near the bound
	{
		// cross every set of adjacent pairs of knapsacks
		for (int i = 0; i < pop_size - 1; i++)
			population.push_back(crossover(population[i], population[i + 1]));

		/* choose survivors */
		unique(population.begin(), population.end()); // remove copies

		for (int i = 0; i < population.size(); i) // next, remove illegal solutions
			if (!population[i].isLegal())
				population.erase(population.begin() + i);
			else
				i++;

		sort(population.begin(), population.end()); // sort the population
		reverse(population.begin(), population.end()); // order with the best at the front

		// then, remove the worst solutions until the population reaches the original size
		if (population.size() > pop_size / 2)
			population.erase(population.begin() + pop_size/2 - 1, population.begin() + population.size() - 1);

		// if necessary add new random solutions to maintain the population size
		if (population.size() < pop_size)
			for (int i = population.size() - 1; i < pop_size; i++)
				population.push_back(knapsack(k, 1)); // add random knapsacks
		
		// re-sort the new knapsacks (ignores legality)
		sort(population.begin(), population.end()); // sort the population
		reverse(population.begin(), population.end()); // order with the best at the front


		// update best knapsack so far
		for (int i = 0; i < population.size(); i++)
			updated |= updateChampion(k, population[i]);
		if (updated)
			update_count = 0;
		update_count++;
	}
	if ((update_count > threshold))
		cout << "Population fully adapted with value " << k.getValue() << endl;
	else if (timeExpired(t_limit, startTime))
		cout << "Time Expired!";
	return k;
}

knapsack steepestDescentKnapsack(knapsack &k, const clock_t startTime, const int t_limit)
{
	// temporary solution
	knapsack b = knapsack(k, 1);
	k.unSelect(); // unselect all objects to ensure the while loop executes

	//	// get an initial solution
	greedyKnapsack(b, 0);
	
	// continue improving the solution until time or at a local minima
	while (!timeExpired(t_limit, startTime) && !(b == k))
	{
		// move to the best neighbor
		k = b;
		b = bestNeighbor(k);
	}
	// return k after optimization is complete
	return b;
}

knapsack bestNeighbor(const knapsack &k, bool type)
{
	knapsack b = k, l = k; // best knapsack neighbor so far.
	for (int i = 0; i < l.getNumObjects(); i++) // for each item
	{
		// toggle the item
		l.toggle(i);

		// greedily fill the rest of the knapsack
		greedyKnapsack(l, i);

		// update champion
		updateChampion(b, l);

		// reset the knapsack
		l = k;
	}
	return b;
}

knapsack bestNeighbor(knapsack &k) // returns the best 1-opt / 2-opt / 3-opt neighbor to the knapsack
{
	knapsack b = k; // best knapsack neighbor so far.
	for (int i = 0; i < k.getNumObjects(); i++) // choose one item to put in
	{
		if (k.isSelected(i))
			continue;
		for (int j = 0; j < k.getNumObjects(); j++) // choose one item to take out
		{
			if (k.isDeselected(j))
				continue;
			for (int l = 0; l < k.getNumObjects(); l++) // choose another item to put in
			{

				if ( (l == i) || k.isSelected(l))
					continue;

				if (k.getValue(i) + k.getValue(l) > k.getValue(j)) // if swapping items gives a net gain
				{
					// select the first item
					k.toggle(i);

					// if knapsack becomes illegal
					if (!k.isLegal())
					{
						// else deselect the second item to try to make it legal
						k.toggle(j);

						// if knapsack becomes legal
						if (k.isLegal())
						{
							// deselect the third item to try to make it legal
							k.toggle(l);

							// update champion
							updateChampion(b, k);

							// reset knapsack
							k.toggle(l);
							k.toggle(j);
							k.toggle(i);
						}
						else
						{
							// update champion
							updateChampion(b, k);
							// reset knapsack
							k.toggle(j);
							k.toggle(i);
						}
					}
					else
					{
						// update champion
						updateChampion(b, k);
						// reset knapsack
						k.toggle(i);
					}
				}
			}
		}
	}
	return b;
}

knapsack branchAndBoundKnapsack(knapsack &k, clock_t startTime, int t_limit) {
    
    /*Your solution should maintain
    a list, possibly implemented as a deque, of partial solutions to a knapsack instance. Each
    partial solution should be stored as a separate knapsack object. The solver should run for
    up to 10 minutes per instance*/
    
    
    // Initializes the champion knapsack object with all items deselected
    knapsack champion = knapsack(k);
    for (int i = 0; i < champion.getNumObjects(); i++) {
        champion.deSelect(i);
    }
    
    // Create a deque containing integers
    deque<knapsack> subKnapsacks;
    
    // Push the knapsack with all items unselected
    knapsack blank = knapsack(k);
    for (int i = 0; i < blank.getNumObjects(); i++) {
        blank.unSelect(i);
    }
    subKnapsacks.push_back(blank);

    
    while (!subKnapsacks.empty()) {
        
        // if time limit is exceeded, return current champion
        unsigned long diff = clock()-startTime;
        if( (float)t_limit <  (float)diff / CLOCKS_PER_SEC) {
            return champion;
        }
        
        // Gets the last item in the deque and removes it
        knapsack current = subKnapsacks.back();
		subKnapsacks.pop_back();
        
		// If the current knapsack is complete and feasible (legal),
        // check it against the champion
        if (current.complete() && current.isLegal()) {
            if (current.getValue() > champion.getValue()) {
                champion = current;
            }
        }
        // Else if the current knapsack is incomplete and not fathomed
        // then push two copies of current onto the deque; one with the
        // next unselected item selected and one with is deselected
        else if (!current.complete() && !current.fathomed(champion.getValue())) {
            
            int i;
            int index = 0;
            for (i = 0; i < current.getNumObjects(); i++) {
                // Find the first unselected object
                // Need to create isUnselected in knapsack.h
                if (current.isUnselected(i)) {
                    index = i;
					break;
                }
            }
            
            // Make copies of current
            knapsack selectNext = knapsack(current);
            knapsack deselectNext = knapsack(current);
            
            // Select/deselect object i from each
            selectNext.select(index);
            deselectNext.deSelect(index);
            
            subKnapsacks.push_back(deselectNext);
            subKnapsacks.push_back(selectNext);

        }
    }
    return champion;

}

knapsack greedyKnapsack(knapsack &k, int index) 
{
	if (index == 0)
	{
		k.unSelect(); // unselect all

		// select the object at index i adding the current item won't exceed the cost limit
		for (int i = 0; i < k.getNumObjects(); i++)
			if (k.getCost() + k.getCost(i) < k.getCostLimit())
				k.select(i);
	}
	else if (index < k.getNumObjects() && index > 0)
	{
		for (int i = index; i < k.getNumObjects(); i++) // deselect all items after index
			k.deSelect(i);

		for (int i = index; i < k.getNumObjects(); i++) // greedily fill the knapsack
			if (k.getCost() + k.getCost(i) < k.getCostLimit())
				k.select(i);
	}
	else
		if (index < 0 || index >= k.getNumObjects())
			throw rangeError("Bad index in greedyKnapsack");
	return k;

}


void exhaustiveKnapsack(knapsack &k, int t)
{
    // get time reference for start
    clock_t startTime;//, endTime;
    startTime = clock();
    
    // permute over all possible knapsack combinations
    k = recursiveKnapsack(k, 0, startTime, t);
    unsigned long diff = clock()-startTime;
    cout << endl << "Exhaustive Algorithm Total Runtime: " << (float) diff / CLOCKS_PER_SEC << "s" << endl;
    
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
    unsigned long diff = clock()-startTime;
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

void generateOutput (knapsack &k, string filename, float runtime) {
    ofstream myfile;
    string filenameext = filename + ".txt";
	myfile.open (filenameext.c_str());
    
    myfile << "------------------------------------------------" << endl;
    
    myfile << "Total value: " << k.getValue() << endl;
    myfile << "Total cost: " << k.getCost() << endl << endl;
    
    // Print out objects in the solution
    for (int i = 0; i < k.getNumObjects(); i++)
        if (k.isSelected(i))
            myfile << i << "  " << k.getValue(i) << " " << k.getCost(i) << endl;
    
    myfile<< endl;
    myfile << "Time to completion: " << runtime << " seconds." << endl;
    myfile.close();
}

bool timeExpired(int t_limit, clock_t startTime)
{
	return ((float)t_limit < (float)(clock() - startTime) / CLOCKS_PER_SEC);
}


bool updateChampion(knapsack &b, knapsack &k)
{
	if (k.isLegal() && (k > b))
	{
		b = k;
		return true;
	}
	else
		return false;
}