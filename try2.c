// C++ program for the above approach
#include <bits/stdc++.h>
using namespace std;

void findend(int i,int j, vector<vector<int>> &a,
			vector<vector<int>> &output,int index)
{
int x = a.size();
int y = a[0].size();

// flag to check column edge case,
// initializing with 0
int flagc = 0;

// flag to check row edge case,
// initializing with 0
int flagr = 0;
int n, m;

for (m = i; m < x; m++)
{

	// loop breaks where first 1 encounters
	if (a[m][j] == 1)
	{
	flagr = 1; // set the flag
	break;
	}

	// pass because already processed
	if (a[m][j] == 5) continue;

	for (n = j; n < y; n++)
	{
	// loop breaks where first 1 encounters
	if (a[m][n] == 1)
	{
		flagc = 1; // set the flag
		break;
	}

	// fill rectangle elements with any
	// number so that we can exclude
	// next time
	a[m][n] = 5;
	}
}

if (flagr == 1)
	output[index].push_back(m-1);
else
	// when end point touch the boundary
	output[index].push_back(m);

if (flagc == 1)
	output[index].push_back(n-1);
else
	// when end point touch the boundary
	output[index].push_back(n);
}

void get_rectangle_coordinates(vector<vector<int>> a)
{

// retrieving the column size of array
int size_of_array = a.size();

// output array where we are going
// to store our output
vector<vector<int>> output;

// It will be used for storing start
// and end location in the same index
int index = -1;

for (int i = 0; i < size_of_array; i++)
{
	for (int j = 0; j < a[0].size(); j++)
	{
	if (a[i][j] == 0)
	{

		// storing initial position
		// of rectangle
		output.push_back({i, j});

		// will be used for the
		// last position
		index = index + 1;
		findend(i, j, a, output, index);
	}
	}
}

cout << "[";
int aa = 2, bb = 0;

for(auto i:output)
{
	bb = 3;
	cout << "[";
	for(int j:i)
	{
	if(bb)
		cout << j << ", ";
	else
		cout << j;
	bb--;
	}
	cout << "]";
	if(aa)
	cout << ", ";
	aa--;

}
cout << "]";
}

// Driver code
int main()
{
vector<vector<int>> tests = {
	{1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 0, 0, 0, 1},
	{1, 0, 1, 0, 0, 0, 1},
	{1, 0, 1, 1, 1, 1, 1},
	{1, 0, 1, 0, 0, 0, 0},
	{1, 1, 1, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1}
};

get_rectangle_coordinates(tests);

return 0;
}

// This code is contributed by mohit kumar 29.
