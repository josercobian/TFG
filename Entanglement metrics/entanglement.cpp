#include "entanglement.h"

struct hash_fn
{
	std::size_t operator() (const vector<int>& V) const
	{
		size_t H = 0;
		for (int i = 0; i < V.size(); i++)
			H ^= V[i];

		return H;
	}
};

using truth_table = unordered_map<vector<int>, int, hash_fn>;

// creates the truth table of a boolean function as a map,
// in which the key represents an input \vec{x} and the value is
// its associated output f(\vec{x})
void create_map_truth_table(truth_table & TT, vector<int>& x, const int depth, const vector<int>& f) {
	// Base condition
	if (depth == x.size())
		TT[x] = f[TT.size()];

	// Recursive condition
	else {
		x[depth] = 1;
		create_map_truth_table(TT, x, depth + 1, f);
		x[depth] = 0;
		create_map_truth_table(TT, x, depth + 1, f);
	}
}

// given a subset returns its complementary
void complementary_subset(const vector<int>& subset, vector<int>& subset_compl, const int N) {
	int pos = 0;
	for (int i = 0; i < N; i++) {
		if (pos < subset.size() && subset[pos] == i) pos++;
		else subset_compl.push_back(i);
	}
}

// given a subset S, it obtains {g^{\vec{x}_S}}, denoted as fun_set
void calculate_g_functions(vector<int>& x, const vector<int>& S, const int pos_s, const bool B, 
	vector<int>& g, unordered_set<vector<int>, hash_fn>& g_set, const int N, const truth_table & TT) {
	// Base condition
	if (pos_s < 0) {
		// S1
		if (B) {
			vector<int> S_c;
			complementary_subset(S, S_c, N);
			vector<int> g;
			calculate_g_functions(x, S_c, S_c.size() - 1, false, g, g_set, N, TT);
			g_set.insert(g);
		}
		// S2
		else {
			g.push_back(TT.at(x));
		}
	}
	// Recursive conditions
	else {
		x[S[pos_s]] = 1;
		calculate_g_functions(x, S, pos_s - 1, B, g, g_set, N, TT);
		x[S[pos_s]] = 0;
		calculate_g_functions(x, S, pos_s - 1, B, g, g_set, N, TT);
	}
}

// given a subset S, it returns i(S) + i(S_c), where S_c denotes its complementary
int calculate_information_shared(const vector<int>& S, const int N, const truth_table & TT) {
	vector<int> S_c;
	complementary_subset(S, S_c, N);

	vector<int> x(N); vector<int> fun;
	unordered_set<vector<int>, hash_fn> fun_set;
	calculate_g_functions(x, S, S.size() - 1, true, fun, fun_set, N, TT);
	// the value of i(S) the size of {g^{\vec{x}_S}} 
	int i = fun_set.size();

	unordered_set<vector<int>, hash_fn> fun_set2;
	calculate_g_functions(x, S_c, S_c.size() - 1, true, fun, fun_set2, N, TT);
	// the value of i(S_c) the size of {g^{\vec{x}_S_c}} 
	int i_c = fun_set2.size();

	return i + i_c;
}

int entanglement_recursive(vector<int>& subset, vector<int>& C, int index, const int N, const truth_table & TT) {
	// Calculate i(S) + i(S_c), where |S| = floor(N/2)
	int ent = subset.size() != N / 2 ? INT_MAX : calculate_information_shared(subset, N, TT);

	// Loop to choose from different elements present
	// after the current index 
	for (int i = index; i < N; i++) {

		// include the C[i] in subset.
		subset.push_back(C[i]);

		// update the value of entnaglement and move onto the next element.
		ent = min(ent, entanglement_recursive(subset, C, i + 1, N, TT));

		// exclude the C[i] from subset and triggers
		// backtracking.
		subset.pop_back();
	}

	return ent;
}

// Function that given a certain boolean function returns
// its entanglement value.
int entanglement(const vector<int>& f, const int N) {
	// Initialize the truth table
	truth_table TT;
	// Fill the truth table hash map from the function f
	vector<int> x(N); create_map_truth_table(TT, x, 0, f);
	// Initialize the set where the partitions (S1,S2) are made.
	// This is the set {0, ..., N - 1}.
	vector<int> C(N);
	for (int i = 0; i < N; i++) { C[i] = i; }
	// Call the recursive entanglement function which is 
	// a backtracking algorithm
	vector<int> subset;  
	return entanglement_recursive(subset, C, 0, N, TT);
}