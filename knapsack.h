// Knapsack class
// Version f08.1

class knapsack
{
public:
    knapsack(ifstream &fin);
    knapsack(const knapsack &);
    int getCost() const;
    int getValue() const;
    int getCost(int) const;
    int getValue(int) const;
    float getDensity(int) const;
    int getNumObjects() const;
    int getCostLimit() const;
    void printSolution();
    void select(int);
    void unSelect(int);
    void deSelect(int);
    bool isSelected(int) const;
    bool isUnselected(int) const;
    bool isDeselected(int) const;
    void orderByDensity();
    int bound();
	bool complete();
    bool isLegal();
    bool fathomed(int);
    
private:
    int numObjects;
    int costLimit;
    vector<int> value;
    vector<int> cost;
    vector<float> density; // vector to keep track of value per cost density
    vector<short int> selected;
    int totalValue;
    int totalCost;
};

knapsack::knapsack(ifstream &fin)
// Construct a new knapsack instance using the data in fin.
{
    int n, b, j, v, c;
    
    fin >> n;  // read the number of objects
    fin >> b;  // read the cost limit
    
    numObjects = n;
    costLimit = b;
    
    value.resize(n);
    cost.resize(n);
    density.resize(n);
    selected.resize(n);
    
    for (int i = 0; i < n; i++)
    {
        fin >> j >> v >> c;
        value[j] = v;
        cost[j] = c;
        density[j] = v/c;
        deSelect(j);
    }
    
    totalValue = 0;
    totalCost = 0;
    
    orderByDensity();
}

knapsack::knapsack(const knapsack &k)
// Knapsack copy constructor.
{
    int n = k.getNumObjects();
    
    value.resize(n);
    cost.resize(n);
    density.resize(n);
    selected.resize(n);
    numObjects = k.getNumObjects();
    costLimit = k.getCostLimit();
    
    totalCost = 0;
    totalValue = 0;
    
    for (int i = 0; i < n; i++)
    {
        value[i] = k.getValue(i);
        cost[i] = k.getCost(i);
        density[i] = k.getDensity(i);
        if (k.isSelected(i))
            select(i);
        else
            deSelect(i);
    }
}

int knapsack::getNumObjects() const
{
    return numObjects;
}

int knapsack::getCostLimit() const
{
    return costLimit;
}

int knapsack::getValue(int i) const
// Return the value of the ith object.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::getValue");
    
    return value[i];
}

int knapsack::getCost(int i) const
// Return the cost of the ith object.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::getCost");
    
    return cost[i];
}

float knapsack::getDensity(int i) const
// Return the density (value per cost) of the ith object;
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::getDensity");
    
    return density[i];
}

int knapsack::getCost() const
// Return the cost of the selected objects.
{
    return totalCost;
}

int knapsack::getValue() const
// Return the value of the selected objects.
{
    return totalValue;
}

ostream &operator<<(ostream &ostr, const knapsack &k)
// Print all information about the knapsack.
{
    cout << "------------------------------------------------" << endl;
    cout << "Num objects: " << k.getNumObjects() << " Cost Limit: " << k.getCostLimit() << endl;
    
    int totalValue = 0;
    int totalCost = 0;
    
    for (int i = 0; i < k.getNumObjects(); i++)
    {
        totalValue += k.getValue(i);
        totalCost += k.getCost(i);
    }
    
    cout << "Total value: " << totalValue << endl;
    cout << "Total cost: " << totalCost << endl << endl;
    
    for (int i = 0; i < k.getNumObjects(); i++)
        cout << i << "  " << k.getValue(i) << " " << k.getCost(i) << endl;
    
    cout << endl;
    
    return ostr;
}

void knapsack::printSolution()
// Prints out the solution.
{
    cout << "------------------------------------------------" << endl;
    
    cout << "Total value: " << getValue() << endl;
    cout << "Total cost: " << getCost() << endl << endl;
    
    // Print out objects in the solution
    for (int i = 0; i < getNumObjects(); i++)
        if (isSelected(i))
            cout << i << "  " << getValue(i) << " " << getCost(i) << endl;
    
    cout << endl;
}

ostream &operator<<(ostream &ostr, vector<bool> v)
// Overloaded output operator for vectors.
{
    for (int i = 0; i < v.size(); i++)
        cout << v[i] << endl;
    
    return ostr;
}

void knapsack::select(int i)
// Select object i.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::Select");
    
    if (selected[i] < 1)
    {
        selected[i] = 1;
        totalCost = totalCost + getCost(i);
        totalValue = totalValue + getValue(i);
    }
}

void knapsack::deSelect(int i)
// deSelect object i.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::unSelect");
    
    if (selected[i] == 1)
    {
        totalCost = totalCost - getCost(i);
        totalValue = totalValue - getValue(i);
    }
    selected[i] = -1;
}

void knapsack::unSelect(int i)
// unSelect object i.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::unSelect");
    
    if (selected[i] == 1)
    {
        totalCost = totalCost - getCost(i);
        totalValue = totalValue - getValue(i);
    }
    selected[i] = 0;
}

bool knapsack::isSelected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::getValue");
    
    return selected[i] == 1;
}

bool knapsack::isUnselected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::getValue");
    
    return selected[i] == 0;
}

bool knapsack::isDeselected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::getValue");
    
    return selected[i] == -1;
}

void knapsack::orderByDensity ()
//orders the items in this knapsack by highest density (value per cost)
{
    int temp = 0;
    for (int i = 0; i < numObjects - 1; i++)
    {
        for (int j = 0; j < numObjects - i - 1; j++)
        {
            if (density[j] > density[j-1]) {
                temp = density[j];
                density[j] = density[j-1];
                density[j-1] = temp;
                temp = value[j];
                value[j] = value[j-1];
                value[j-1] = temp;
                temp = cost[j];
                cost[j] = cost[j-1];
                cost[j-1] = temp;
            }
        }
    }
}

int knapsack::bound()
// returns an upper bound on teh value of objects by solving fractional knapsack
{
	// get constraints and value of the bag
	int cost_remaining = costLimit;
	int value_accumulated = 0;
	int i = 0; // indexer
	
	// add all selected items to the bag
	for(i = 0; i < numObjects; i++)
	{
		if(selected[i] == 1)
		{
			cost_remaining -= cost[i];
			value_accumulated += value[i];
		}
	}
	
	// add unselected items in order of density to the bag until it overflows
	i = 0;
	while((cost_remaining > 0) && (i < numObjects))
	{
		// if the bag is not full or overflowing, add the unselected item
		if(selected[i] == 0)
		{
			cost_remaining -= cost[i];
			value_accumulated += value[i];
		}
		i++;
	}
	
	// remove excess objects
	i--;
	value_accumulated += cost_remaining * density[i]; // cost_remaining is negative, i is the last object added
	
	// return the fractional knapsack result
	return value_accumulated;
}

bool knapsack::complete() // returns whether the knapsack is a complete solution
{
	for(int i = 0; i < numObjects; i++)
	{
		if(!selected[i])
			return false;
	}
	return true;
}

bool knapsack::isLegal() // returns whether the knapsack is a legal solution
{
	return totalCost < costLimit;
}

bool knapsack::fathomed(int championValue) // returns whether the knapsack is fathomed
{
    // A subproblem is fathomed under any of the following conditions:
    // 1. the solution is infeasible (cost exceeds limit)
    // 2. the total cost is worse than the current champion value
    // 3. ?
    
    return (!isLegal() || (getValue() <= championValue));
}
