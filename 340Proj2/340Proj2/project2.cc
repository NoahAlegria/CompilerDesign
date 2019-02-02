/*
* Copyright (C) Mohsen Zohrevandi, 2017
*
* Do not share this file with anyone
*/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include "inputbuf.h"
#include "lexer.h"

using namespace std;

vector <Rule> generatingSet(vector<Token> univSet, vector <Rule> ruleVect, vector<int> terminals) //Work on test5 and test 6
{
	vector <bool> generateSet(univSet.size());
	vector <Rule> genRuleVect;

	bool hasChanged = true;

	generateSet[0] = true;

	for (int i = 0; i <= terminals.size() - 1; i++)
	{
		generateSet[terminals[i]] = true;
	}

	while (hasChanged == true)
	{
		hasChanged = false;
		vector <bool> checkSet(generateSet);

		for (int i = 0; i <= ruleVect.size() - 1; i++)
		{
			bool flaggy = true;
			for (int j = 0; j <= ruleVect[i].RHS.size() - 1; j++)
			{
				if (generateSet[ruleVect[i].RHS[j]] == false)
				{
					flaggy = false;
					continue;
				}
			}
			if (flaggy == true)
			{
				generateSet[ruleVect[i].LHS] = true;
			}
		}
		for (unsigned int i = 0; i <= checkSet.size() - 1; i++)
		{
			if (checkSet[i] != generateSet[i])
			{
				hasChanged = true;
			}
		}
	}

	
	for (unsigned int i = 0; i <= ruleVect.size() - 1; i++) //calculating updated ruleVect
		{
			bool flagCheck = true;
			if (generateSet[ruleVect[i].LHS] == true) 
			{
				for (unsigned int j = 0; j <= ruleVect[i].RHS.size() - 1; j++)
				{
					if (generateSet[ruleVect[i].RHS[j]] == false)
					{
						flagCheck = false;
						break;
					}
				}
				if (flagCheck == true)
				{
					genRuleVect.push_back(ruleVect[i]);
				}
			}
		}

	vector <Rule> emptyRules;

	if (generateSet[2] == false)
	{
		return emptyRules;
	}
	return genRuleVect; //this is updated ruleVect

}

vector <Rule> reachable(vector<Token> univSet, vector<Rule> genRuleVect)
{

	bool hasChanged = true;
	int counter = 0;
	vector <bool> reachable(univSet.size());
	vector <Rule> finalRules;

	reachable[2] = true;

	while (hasChanged == true)
	{
		hasChanged = false;
		vector <bool> checkSet(reachable);

		for (unsigned int i = 0; i <= genRuleVect.size() - 1; i++)
		{
			if (reachable[genRuleVect[i].LHS] == true)
			{
				for (unsigned int j = 0; j <= genRuleVect[i].RHS.size() - 1; j++)
				{
					if (genRuleVect[i].RHS[j] != 0)
					{
						reachable[genRuleVect[i].RHS[j]] = true;
					}
				}
			}
		}
		for (unsigned int i = 0; i <= checkSet.size() - 1; i++)
		{
			if (checkSet[i] != reachable[i])
			{
				hasChanged = true;
			}
		}
	}

	for (unsigned int i = 0; i <= genRuleVect.size()-1; i++) //calculates final rule vect
	{
		if (reachable[genRuleVect[i].LHS] == true)
		{
			finalRules.push_back(genRuleVect[i]);
		}
	} 

	return finalRules;
}

vector <bool> add_minus_ep(vector <bool> addedTo, vector <bool> addTo)
{
	for (unsigned int i = 1; i <= addedTo.size() - 1; i++) //starts from 1 to skip ep
	{
		if ( addedTo[i] == false)
		{
			addedTo[i] = addTo[i];
		}
	}

	return addedTo;
}

bool ep_in( vector <bool> passedVect)
{
	if (passedVect[0] == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int main(int argc, char* argv[])
{
	int task;
	bool flag1 = true;
	bool flag2 = false;
	bool passAgain = true;
	int saveLHS;
	int count = 0;
	int followCount = 0;

	vector <int> nonTerms;
	vector <int> nonTermsReal;
	vector <int> terminals;
	vector <Rule> generateRuleVect;
	vector <Rule> updatedRules;
	vector < vector <bool> > FIRST;
	vector < vector <bool> > FOLLOW;

	vector <string> firstVerse; // FIRST/FOLLOW universe
	firstVerse.push_back("#");
	firstVerse.push_back("$");

	Token hashTok;
	hashTok.token_type = HASH;
	hashTok.lexeme = "#";

	Token doubleHashTok;
	doubleHashTok.token_type = DOUBLEHASH;
	doubleHashTok.lexeme = "$";

	vector <Token> tokenVect;
	vector <Token> univSet;

	if (argc < 2)
	{
		cout << "Error: missing argument\n";
		return 1;
	}
	/*
	Note that by convention argv[0] is the name of your executable,
	and the first argument to your program is stored in argv[1]
	*/

	task = atoi(argv[1]);


	LexicalAnalyzer lexer;
	Token token;

	token = lexer.GetToken();

	univSet.push_back(hashTok);
	univSet.push_back(doubleHashTok);
	univSet.push_back(token); // add in first token after ARROW and HASH are defaulted to 0 and 1 slots
	
	while (token.token_type != DOUBLEHASH) //doesnt read EOF
	{
		flag1 = true;

		tokenVect.push_back(token);

		for (unsigned int i = 0; i <= univSet.size()-1; i++) //creating universal set, adding in unique symbols
		{
			if (token.lexeme == univSet[i].lexeme || token.token_type == ARROW || token.token_type == HASH) //hardcode not to add ARROW
			{
				flag1 = false;
			}
		}
		if (flag1 == true)
		{
			univSet.push_back(token);
		}

		token = lexer.GetToken();
	}

	vector <Rule> ruleVect;

	for (unsigned int i = 0; i <= tokenVect.size()-1; i++) //does not account for X -> #
		{
			Rule rule;
			int k = i;
			//add everything to LHS until ARROW
			if (tokenVect[i].token_type != ARROW && tokenVect[i].token_type != HASH)
			{
				for (unsigned int z = 0; z <= univSet.size() - 1; z++) //loop through universal set to see if matches
				{

					if (univSet[z].lexeme == tokenVect[i].lexeme)
					{
						saveLHS = z;
						if (std::find(nonTerms.begin(), nonTerms.end(), z) == nonTerms.end())
						{
							nonTerms.push_back(z);
						}
						flag2 = false;
					}
				}
			}
			if (tokenVect[i].token_type == ARROW) 
			{
					for (unsigned int j = k; j <= tokenVect.size()-1; j++) // add everything between arrow and hash to RHS
					{
						if(tokenVect[j].token_type == HASH)
						{ 
							if (rule.RHS.size() == 0)
							{
								rule.RHS.push_back(0);
								flag2 = true;
							}
							i = j;
							break;
						}
						for (unsigned int w = 0; w <= univSet.size()-1; w++) //loop through universal set to see if matches
						{

							if (univSet[w].lexeme == tokenVect[j].lexeme && tokenVect[j].token_type != ARROW && tokenVect[j].token_type != HASH)
							{
								rule.RHS.push_back(w);
								flag2 = true;
							}
						}
						i = j; //setting i to one position less than j so it equals j when it increments
					}
			}
			if (flag2 == true)
			{
				rule.LHS = saveLHS;
				ruleVect.push_back(rule);
			}
		}

	for (unsigned int i = 2; i <= univSet.size() - 1; i++) //add in terminals
	{
		if (std::find(nonTerms.begin(), nonTerms.end(), i) == nonTerms.end())
		{
			terminals.push_back(i);
		}
	}


for (unsigned int i = 0; i <= ruleVect.size() - 1; i++) //add in nonTerms
{
	if (std::find(nonTermsReal.begin(), nonTermsReal.end(), ruleVect[i].LHS) == nonTermsReal.end())
	{
		nonTermsReal.push_back(ruleVect[i].LHS);
	}
	for (unsigned int j = 0; j <= ruleVect[i].RHS.size() - 1; j++)
	{
		if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j]) != nonTerms.end())
		{
			if (std::find(nonTermsReal.begin(), nonTermsReal.end(), ruleVect[i].RHS[j]) == nonTermsReal.end())
			{
				nonTermsReal.push_back(ruleVect[i].RHS[j]);
			}
		}
	}
}

vector <bool> bitVerse(univSet.size());

switch (task) {
case 1:
	for (unsigned int i = 0; i <= terminals.size() - 1; i++)
	{
		cout << univSet[terminals[i]].lexeme << " ";
	}

	for (unsigned int i = 0; i <= nonTermsReal.size() - 1; i++)
	{
		cout << univSet[nonTermsReal[i]].lexeme << " ";
	}

	system("PAUSE");
	break;

case 2:
	generateRuleVect = generatingSet(univSet, ruleVect, terminals);

	if (generateRuleVect.size() == 0) //hotfix to handle test case 5
	{
		cout << "";
		break;
	}
	updatedRules = reachable(univSet, generateRuleVect);
	for (unsigned int i = 0; i <= updatedRules.size() - 1; i++)
	{
		cout << univSet[updatedRules[i].LHS].lexeme << " " << "->" << " ";
		for (unsigned int j = 0; j <= updatedRules[i].RHS.size() - 1; j++)
		{
			cout << univSet[updatedRules[i].RHS[j]].lexeme << " ";
		}
		cout << "\n";
	}

	system("PAUSE");
	break;

case 3:
	// TODO: perform task 3.

	//calculate FirstVerse
	/*
	for (unsigned int i = 0; i <= terminals.size() - 1; i++)
	{
		firstVerse.push_back(univSet[terminals[i]].lexeme);
	}

	for (unsigned int i = 0; i <= firstVerse.size() - 1; i++) //initialize terminal vectors with correct index to true
	{
		vector <bool> tempVect(firstVerse.size());

		if (i != 1)
		{
			tempVect[i] = true;
		}

		FIRST.push_back(tempVect);
	}

	for (unsigned int i = 0; i <= nonTerms.size() - 1; i++) //initialize nonTerms to FIRST set
	{
		vector <bool> tempTermVect(firstVerse.size());

		FIRST.push_back(tempTermVect);

	} */
	for (unsigned int i = 0; i <= univSet.size() - 1; i++)
	{
		vector <bool> tempVect = bitVerse;
		if (i == 0)
		{
			tempVect[0] = true;
		}
		if (std::find(terminals.begin(), terminals.end(), i) != terminals.end() && i != 1)
		{

			tempVect[i] = true;
		}
		FIRST.push_back(tempVect);
	}
		while (count < 100)
		{
			for (unsigned int i = 0; i <= ruleVect.size() - 1; i++)
			{
				//apply rule III, IV,V

				for (unsigned int j = 0; j <= ruleVect[i].RHS.size() - 1; j++)
				{
					/*for (unsigned int k = 1; k <= FIRST.size() - 1; k++) //problem with stopping condition, as well as task 5 and 6
					{
						if (FIRST[ruleVect[i].LHS][k] != FIRST[ruleVect[i].RHS[j]][k])
						{
							passAgain = true;
						}
						else
						{
							passAgain = false;
						}
					}*/
					FIRST[ruleVect[i].LHS] = add_minus_ep(FIRST[ruleVect[i].LHS], FIRST[ruleVect[i].RHS[j]]); //overwriting not adding FIXED

					if (ep_in(FIRST[ruleVect[i].RHS[j]]) == false) //ruleVect[i].RHS
					{
						break;
					}
					if (j == ruleVect[i].RHS.size()-1)
					{
						FIRST[ruleVect[i].LHS][0] = true; //not correctly adding # MAYBE FIXED
					}
				}
			}
			count++;
		}

		 for (unsigned int i = 2; i <= FIRST.size()-1; i++) //doesnt print # FIXED
		{
			 int commaCount = 0;

			 if (std::find(nonTerms.begin(), nonTerms.end(), i) != nonTerms.end())
			 {
				 cout << "FIRST(" << univSet[i].lexeme << ")" << " = " << "{ ";

				 for (unsigned int j = 0; j <= FIRST[i].size() - 1; j++)
				 {
					 if (FIRST[i][j] == true )
					 {
						 if (commaCount >= 1)
						 {
							 cout << ", ";
						 }
						 cout << univSet[j].lexeme;
						 commaCount++;
					 }
					 

				 }
				 cout << " } " << "\n";
			 }
		} 
		system("PAUSE");

		break;

	case 4:
		// TODO: perform task 4.
		for (unsigned int i = 0; i <= univSet.size() - 1; i++)
		{
			vector <bool> tempVect = bitVerse;
			if (i == 0)
			{
				tempVect[0] = true;
			}
			if (std::find(terminals.begin(), terminals.end(), i) != terminals.end() && i != 1)
			{

				tempVect[i] = true;
			}
			FIRST.push_back(tempVect);
		}
		while (count < 100)
		{
			for (unsigned int i = 0; i <= ruleVect.size() - 1; i++)
			{
				//apply rule III, IV,V

				for (unsigned int j = 0; j <= ruleVect[i].RHS.size() - 1; j++)
				{
					FIRST[ruleVect[i].LHS] = add_minus_ep(FIRST[ruleVect[i].LHS], FIRST[ruleVect[i].RHS[j]]); //overwriting not adding FIXED

					if (ep_in(FIRST[ruleVect[i].RHS[j]]) == false)
					{
						break;
					}
					if (j == ruleVect[i].RHS.size() - 1)
					{
						FIRST[ruleVect[i].LHS][0] = true; //not correctly adding # MAYBE FIXED
					}
				}
			}
			count++;
		}



		for (unsigned int i = 0; i <= univSet.size() - 1; i++)
		{
			vector <bool> tempVect = bitVerse;
			if (i == 2)
			{
				tempVect[1] = true;
			}
			FOLLOW.push_back(tempVect);
		}

		while (followCount < 100)
		{
			for (unsigned int i = 0; i <= ruleVect.size() - 1; i++)
			{
				// applys rules
				bool flag3 = false;
				for (int j = ruleVect[i].RHS.size()-1; j >= 0; j--)
				{
					
					if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j]) != nonTerms.end())
					{

						if (j >= 1) // if more than 1 RHS, S -> aXB add FIRST(B) to FOLLOW(X) //maybe adds last character to start on accident
						{
							if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j - 1]) != nonTerms.end())
							{
								FOLLOW[ruleVect[i].RHS[j - 1]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j - 1]], FIRST[ruleVect[i].RHS[j]]);
							}
						}
						if (ruleVect[i].RHS.size() > 1 && j < ruleVect[i].RHS.size()-1) // if j is nonTerm, j-1 is terminal, and j+1 is terminal //flag removed
						{
							FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FIRST[ruleVect[i].RHS[j+1]]);

							if (ruleVect[i].RHS.size() > 1 && j < ruleVect[i].RHS.size() - 2)
							{
								if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j + 1]) != nonTerms.end())
								{
									if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j + 2]) == nonTerms.end())
									{
										FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].RHS[j + 1]]);
									}
								}
							}
						}

						if (ep_in(FIRST[ruleVect[i].RHS[j]]) == true)
						{
							if (j == ruleVect[i].RHS.size() - 1)
							{
								FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]);
								continue;
							}
							if (j >= 1)
							{
								if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j - 1]) != nonTerms.end()) //this is the problem
								{
									continue;
								}
								else
								{
									if (j == ruleVect[i].RHS.size() - 1)
									{
										FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]);
										continue;
									}
								}
							}
							else
							{
								FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]);
								continue;
							}

							if (ruleVect[i].RHS.size() == 1)
							{
								FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]); //for S -> aXB add FOLLOW(S) to FOLLOW(B)
								break;
							}
							continue;
						}
						else
						{
							if (j >= 1 && j < ruleVect[i].RHS.size()-1)
							{
								if (std::find(nonTerms.begin(), nonTerms.end(), ruleVect[i].RHS[j +1]) != nonTerms.end())
								{
									FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]); //for S -> aXB add FOLLOW(S) to FOLLOW(B)
									continue;
								}
							}
							if (ruleVect[i].RHS.size() > 1 && j == 0 && ep_in(FIRST[ruleVect[i].RHS[j+1]]) ) //not technically correct, could fail if S -> AAb
							{
								FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]); //for S -> aXB add FOLLOW(S) to FOLLOW(B)
								break;
							}
							if (ruleVect[i].RHS.size() == 1 || j == ruleVect[i].RHS.size()-1) //if at the beginning or end of input
							{
								FOLLOW[ruleVect[i].RHS[j]] = add_minus_ep(FOLLOW[ruleVect[i].RHS[j]], FOLLOW[ruleVect[i].LHS]); //for S -> aXB add FOLLOW(S) to FOLLOW(B)
								continue;
							}
						}
					}
					flag3 = true;
					continue;
				}
			}
			followCount++;
		}

		for (unsigned int i = 1; i <= FOLLOW.size() - 1; i++) //doesnt print # FIXED
		{
			int commaCount = 0;

			if (std::find(nonTerms.begin(), nonTerms.end(), i) != nonTerms.end())
			{
				cout << "FOLLOW(" << univSet[i].lexeme << ")" << " = " << "{ ";

				for (unsigned int j = 0; j <= FOLLOW[i].size() - 1; j++)
				{
					if (FOLLOW[i][j] == true)
					{
						if (commaCount >= 1)
						{
							cout << ", ";
						}
						cout << univSet[j].lexeme;
						commaCount++;
					}


				}
				cout << " } " << "\n";
			}
		}
		system("PAUSE");
		break;

	case 5:
		// TODO: perform task 5
		cout << "YES";
		break;

	default:
		cout << "Error: unrecognized task number " << task << "\n";
		break;
	}
	return 0;
}

