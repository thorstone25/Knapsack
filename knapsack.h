// Knapsack class
// Version f08.1

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>


boost::random::mt19937 gen;

class knapsack
{
public:
    knapsack(ifstream &fin);
    knapsack(const knapsack &);
	knapsack::knapsack(const knapsack &k, bool random);
    int getCost() const;
    int getValue() const;
    int getCost(int) const;
    int getValue(int) const;
    float getDensity(int) const;
    int getNumObjects() const;
    int getCostLimit() const;
    void printSolution();
    void printSelections();
    void select(int);
    void unSelect(int);
	void unSelect();
    void deSelect(int);
    bool isSelected(int) const;
    bool isUnselected(int) const;
    bool isDeselected(int) const;
    void orderByDensity();
    float bound();
	bool complete();
    bool isLegal();
    bool fathomed(int);
	void toggle(int);
	friend bool operator==(const knapsack &, const knapsack &);
	friend bool operator<(const knapsack &, const knapsack &);
	friend bool operator>(const knapsack &, const knapsack &);
	friend knapsack crossover(const knapsack, const knapsack);
    
private:
    int numObjects;
    int costLimit;
    int totalValue;
    int totalCost;
	vector<int> value;
    vector<int> cost;
    vector<float> density; // vector to keep track of value per cost density
    vector<short int> selected;

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
        density[j] = (float)v/(float)c;
        unSelect(j);
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
        else if (k.isUnselected(i))
            unSelect(i);
        else if (k.isDeselected(i))
            deSelect(i);
    }
	orderByDensity();
}

knapsack::knapsack(const knapsack &k, bool random)
// randomized copy of the current knapsack
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

	if( !random )
	{
		for (int i = 0; i < n; i++)
		{
			value[i] = k.getValue(i);
			cost[i] = k.getCost(i);
			density[i] = k.getDensity(i);
			if (k.isSelected(i))
				select(i);
			else if (k.isUnselected(i))
				unSelect(i);
			else if (k.isDeselected(i))
				deSelect(i);
		}
	}
	else
	{
		// get a modulus value used for masking versus a random number
		unsigned int modval = 0;
		unsigned int randval = 0;

		// for all items
		for (int i = 0; i < k.numObjects; i++)
		{
			// copy item values
			value[i] = k.getValue(i);
			cost[i] = k.getCost(i);
			density[i] = k.getDensity(i);

			// if all bits used, get 32 new random bits and reset modulus
			if (modval == 0)
			{
				// get a random integer
				boost::random::uniform_int_distribution<> dist(0, 256*256 - 1);
				randval = dist(gen);

				// reset modulus bit
				modval = 1;
			}

			// choose selection or deselection
			if ((randval & modval) > 0)
				select(i);
			else
				deSelect(i);
			
			// update modulos value
			modval <<= 1;
		}
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

void knapsack::printSelections()
// Prints out the solution.
{
    cout << "------------------------------------------------" << endl;
    
    cout << "Total value: " << getValue() << endl;
    cout << "Total cost: " << getCost() << endl << endl;
    
    // Print out objects in the solution
    for (int i = 0; i < getNumObjects(); i++)
            cout << i << "  " << getValue(i) << " " << getCost(i) << " " << density[i] << " " << selected[i] << endl;
    
    
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
        throw rangeError("Bad value in knapsack::deSelect");
    
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

void knapsack::unSelect()
// unSelect all objects.
{
	for (int i = 0; i < numObjects; i++)
	{
		if (selected[i] == 1)
		{
			totalCost = totalCost - getCost(i);
			totalValue = totalValue - getValue(i);
		}
		selected[i] = 0;
	}
}

bool knapsack::isSelected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::isSelected");
    
    return selected[i] == 1;
}

bool knapsack::isUnselected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::isUnselected");
    
    return selected[i] == 0;
}

bool knapsack::isDeselected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
    if (i < 0 || i >= getNumObjects())
        throw rangeError("Bad value in knapsack::isDeselected");
    
    return selected[i] == -1;
}

void knapsack::orderByDensity ()
//orders the items in this knapsack by highest density (value per cost)
{
    float temp_density = 0;
    int temp_value = 0;
    int temp_cost = 0;
    
    for (int i = 0; i < numObjects - 1; i++)
    {
        for (int j = 0; j < numObjects - i - 1; j++)
        {
            if (density[j+1] > density[j])
            {
                temp_density = density[j+1];
                density[j+1] = density[j];
                density[j] = temp_density;
                temp_value = value[j+1];
                value[j+1] = value[j];
                value[j] = temp_value;
                temp_cost = cost[j+1];
                cost[j+1] = cost[j];
                cost[j] = temp_cost;
            }
        }
    }
}

float knapsack::bound()
// returns an upper bound on the value of objects by solving fractional knapsack
{
	// get constraints and value of the bag
	int cost_remaining = costLimit;
	float value_accumulated = 0;
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
	while((i < numObjects) && (cost_remaining - cost[i] > 0))
	{
		// if the bag is not full or overflowing, add the unselected item
		if(selected[i] == 0)
		{
			cost_remaining -= cost[i];
			value_accumulated += value[i];
		}
		i++;
	}
	
	
	// add fractional object
	i = ((i > 0) ? --i : 0);
	value_accumulated += cost_remaining * density[i]; // cost_remaining is negative,
    cost_remaining -= cost_remaining; // cost remaining should be 0
    
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
    
    return (!isLegal() || (bound() < championValue));
}

void knapsack::toggle(int item) // toggles the selected state of the item
{
	if (item < 0 || item >= getNumObjects())
		throw rangeError("Bad index in knapsack::toggle");
	if (isSelected(item))
		deSelect(item);
	else if (isDeselected(item))
		select(item);
	return;
}

bool operator==(const knapsack &lhs, const knapsack &rhs)
{
	if ((lhs.numObjects == rhs.numObjects) && (lhs.value == rhs.value))
	{
		for (int i = 0; i < lhs.numObjects; i++)
		{
			if (lhs.selected[i] != rhs.selected[i])
				return false;
		}
	}
	return true;
}

bool operator<(const knapsack &lhs, const knapsack &rhs)
{
	return lhs.totalValue < rhs.totalValue;
}

bool operator>(const knapsack &lhs, const knapsack &rhs)
{
	return lhs.totalValue > rhs.totalValue;
}

knapsack crossover(const knapsack k, const knapsack l)
{
	// create a new knapsack
	knapsack m = k;

	// get a modulus value used for masking versus a random number
	unsigned short int modval = 0;
	unsigned short int randval = 0;
	// for all items
	for (int i = 0; i < k.numObjects; i++)
	{
		// if all bits used, get 16 new random bits and reset modulus
		if ( modval == 0)
		{
			// get a random integer
			boost::random::uniform_int_distribution<> dist(0, 65536 - 1);
			randval = dist(gen);

			// reset modulus bit
			modval = 65536/2;
		}

		//  choose parent for the item
		if (randval & modval)
		{
			if (k.isSelected(i))
				m.select(i);
			else if (k.isDeselected(i))
				m.deSelect(i);
			else if (k.isUnselected(i))
				m.unSelect(i);
		}
		else
		{
			if (l.isSelected(i))
				m.select(i);
			else if (l.isDeselected(i))
				m.deSelect(i);
			else if (l.isUnselected(i))
				m.unSelect(i);
		}

		// update modulus by shifting bit left by one
		modval >>= 1;
	}

	return m;
	
}