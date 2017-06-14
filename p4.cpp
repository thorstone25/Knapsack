// Project 2a: Solving knapsack using a greedy algorithm
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

knapsack branchAndBoundKnapsack(knapsack &k, clock_t startTime, int t_limit);
void greedyKnapsack(knapsack &k);
void exhaustiveKnapsack(knapsack &k, int t);
knapsack recursiveKnapsack(knapsack &k, int item, clock_t startTime, int t_limit);
void generateOutput(knapsack &k, string filename, float runtime);

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
        clock_t startTime;
        startTime = clock();
        knapsack solution = branchAndBoundKnapsack(k, startTime, 600);
        solution.printSolution();
        unsigned long diff = clock()-startTime;
        float runTime = (float) diff / CLOCKS_PER_SEC;
        cout << endl << "Branch and Bound Algorithm Total Runtime: " << runTime << "s" << endl;
        generateOutput(k, filename, runTime);

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

int main()
{
    RunAlgorithm("knapsack8.input");
    return 0;
    RunAlgorithm("knapsack12.input");
    RunAlgorithm("knapsack16.input");
    RunAlgorithm("knapsack20.input");
    RunAlgorithm("knapsack28.input");
    RunAlgorithm("knapsack32.input");
    RunAlgorithm("knapsack48.input");
    RunAlgorithm("knapsack64.input");
    RunAlgorithm("knapsack128.input");
    RunAlgorithm("knapsack256.input");
    RunAlgorithm("knapsack512.input");
    RunAlgorithm("knapsack1024.input");
    
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
            for (i = 0; i < current.getNumObjects(); i++) {
                // Find the first unselected object
                // Need to create isUnselected in knapsack.h
                if (current.isUnselected(i)) { break; }
            }
            
            // Make copies of current
            knapsack selectNext = knapsack(current);
            knapsack deselectNext = knapsack(current);
            
            // Select/deselect object i from each
            selectNext.select(i);
            deselectNext.deSelect(i);
            
            subKnapsacks.push_back(selectNext);
            subKnapsacks.push_back(deselectNext);

        }
    }
    return champion;

}

void greedyKnapsack(knapsack &k) {
    k.orderByDensity();
    for (int i = 0; i < k.getNumObjects(); i++)
    {
        // select the object at index i if both:
        // 1. the current toal cost of all the selected items is less than the cost limit
        // 2. adding the current item won't exceed the cost limit
        if (k.getCost() < k.getCostLimit() && k.getCost(i) < k.getCostLimit() - k.getCost())
        {
            k.select(i);
        }
    }
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
    string filenameext = filename + ".output";
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
