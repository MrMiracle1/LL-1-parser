#pragma once
#include<set>
#include<vector>
#include<map>
#include<deque>
#include<string>

using namespace std;

typedef string token;

struct production {
	token left;
	deque<token> right;
};

class grammar {
public:
	vector<token> non_terminators, terminators;
	token start_token;
	vector<production> productions;
	map<token, bool> first_over, follow_over;
	map<token, set<token>> first,follow;
	map<token, set<int>> productions_class;
	vector<set<token>> first_of_production;
	map<token, map<token, bool>> follow_include;
public:
	grammar();
	void load_file();
	void remove_recursion();
	void first_derivation(const token&);
	void follow_derivation(const token&);
	void output_productions();
	void output_first();
	void output_follow();
};
