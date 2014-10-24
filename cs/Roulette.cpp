//============================================================================
// Name        : Roulette.cpp
// Author      : Nick Bailey, zjb130030
// Version     :
// Copyright   :
// Description : Roulette Wheel
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
using namespace std;

struct Bet {
	short type;
	short value;
	int amount;
};
struct Bets {
	short num;
	Bet *bets;
};

int StartingFunds();
Bets GetBets(int&);
short Roll(); // Gets a random number between 0 and 36, inclusive

void BetSingleNum(int &, Bets &, short &);
void BetColors(int &, Bets &, short &);
void BetOddsEvens(int &, Bets &, short &);
void BetSets12(int &, Bets &, short &);
void BetHalves(int &, Bets &, short &);
void BetColumns(int &, Bets &, short &);

bool  CheckNum(int, int); 	// returns true/false roll == bet
short CheckOddEven(int); 	// returns 1 "odd" or 2 "even"
short CheckCol(int); 		// returns column number: 1, 2, 3, or 0 for #0
short CheckColor(int); 		// returns 1 (red), 2 (black), or 0 for #0
short Check12(int); 		// shortreturns 1, 2, 3, or 0 for first, second, third set of 12, or 0 for #0
short CheckHalf(int);		// returns first half or second half, or neither.

void exit();

enum bet_types { SINGLE_NUM, RED_BLK, ODD_EVEN, SETS12, HALVES, COLUMNS};
enum colors { ZERO, RED, BLK };
enum odds_evens { NEITHER, ODD, EVEN };
enum sets { ZERO_12, FIRST_12, SECOND_12, THIRD_12 };
enum halves { ZERO_HALF, FIRST_HALF, SECOND_HALF };
enum columns { ZERO_COL, FIRST_COL, SECOND_COL, THIRD_COL };

int main() {
	int StartCash = StartingFunds();	// Intro text + starting funds
	int CurrentCash = 0 + StartCash;

	srand (time(NULL));

	char askplay = 'Y';
	cout << "Would you like to play a round of Roulette? (Y/n) ";
	cin >> askplay;

	while (askplay != 'n' && askplay != 'N') {
		Bets b = GetBets(CurrentCash);
		int roll = Roll();
		cout << endl;

		bool lost = true; // if you win anything, this is set to false.
		for (int i=0; i<b.num; i++) {

			cout << endl;

			switch (b.bets[i].type) {
			case SINGLE_NUM	:	if (CheckNum(roll,b.bets[i].value)) {
					cout << "Your bet on #" << roll << " won, for a winning of $" << b.bets[i].amount * 36;
					CurrentCash += b.bets[i].amount * 36; // $35 per $1 bet, for total of $36 on a win
				}
				lost = false; break;
			case RED_BLK 	:	if (CheckColor(roll) == b.bets[i].value) {
					cout << "Your reds-or-blacks bet won on #" << roll << ", for a winning amount of $" << b.bets[i].amount * 2;
					CurrentCash += b.bets[i].amount * 2;
				}
				lost = false; break;

			case ODD_EVEN	:	if (CheckOddEven(roll) == b.bets[i].value) {
					cout << "Your odds-or-evens bet won on #" << roll << ", for a winning amount of $" << b.bets[i].amount * 2;
					CurrentCash += b.bets[i].amount * 2;
				}
				lost = false; break;

			case SETS12		:	if (Check12(roll) == b.bets[i].value) {
					cout << "Your set-of-12 bet won on #" << roll << ", for a winning amount of $" << b.bets[i].amount * 3;
					CurrentCash += b.bets[i].amount * 3;
				}
				lost = false; break;

			case HALVES		:	if (CheckCol(roll) == b.bets[i].value) {
					cout << "Your first-or-second half bet won on #" << roll << ", for a winning amount of $" << b.bets[i].amount * 2;
					CurrentCash += b.bets[i].amount * 2;
				}
				lost = false; break;

			case COLUMNS	:	if (CheckCol(roll) == b.bets[i].value) {
					cout << "Your columns bet won on " << roll << ", for a winning amount of $" << b.bets[i].amount * 3;
					CurrentCash += b.bets[i].amount * 3;
				}
				lost = false; break;
			}
		}

		if (lost) cout << "You did not win anything this round.  The number was " << roll << ".\n";

		cout << "You have $" << CurrentCash << endl;
		cout << "Would you like to play another round of Roulette? (Y/n) ";
		cin >> askplay;
	}

	cout << "You came with " << StartCash << " and now you have $" << CurrentCash << "." << endl;
	int diff = CurrentCash - StartCash;
	if (diff>=0)
		cout << "You won $" << diff << " during tonight's gambling.";
	else
		cout << "You lost $" << diff << " tonight.  Please be sure to come again!";

	exit();
	return 0;
}

int StartingFunds() {
	int startcash;
	cout << "Welcome to Money-Maker Nick's totally balanced, non-rigged casino!" << endl;
	cout << "How much money are you willing to lose tonight? (no cents): ";
	cin >> startcash;

	while (startcash > 1000000 || startcash<=0) {
		cout << "Stop trying to break Nick's casino.  ";
		if (startcash > 1000000)	// Custom message for too much starting money
			cout << "There are not enough chips in the conversion machine." << endl;
		if (startcash <= 0 )		// Custom message for no or fewer funds
			cout << "You can't gamble without any money." << endl;
		cout << "How much money are you planning to lose tonight? ";
		cin >> startcash;
	}

	return startcash;
}

Bets GetBets(int & CurrentCash) {
	Bets b; //destroy old bets, if any

	cout << "How many bets will you be making for this next spin? (1-8): ";
	cin >> b.num;
	while (b.num<=0 || b.num>8) {
		if (b.num<=0) cout << "Watching the table is not permitted.";	// bets must be >0
		if (b.num> 8) cout << "You're not allowed to try to win that frequently.";	// limit bets to 8
		cout << "  Please try again (1-8): ";
		cin >> b.num;
	}
	b.bets = new Bet[b.num];	// make new dynamically-sized array

	for (short i=0; i<b.num; i++) {
		if (CurrentCash<=0){	// if person runs out of funds exactly, stop the loop prematurely
			cout << "You don't have enough money to keep gambling!";
			b.num = i - 1;
			break;
		}

		short choice;
		cout << "\n\n";
		cout << "You have $" << CurrentCash << ".\n";
		cout << "Please enter the number of the type of bet you want to make: " << endl;
		cout << "1) Specific Number (0-36)" << endl;
		cout << "2) Red or Black" << endl;
		cout << "3) Odd or Even" << endl;
		cout << "4) First, second, or third set of 12" << endl;
		cout << "5) First or second half" << endl;
		cout << "6) First, second, or third column:" 	 << endl <<
				"    1  4  7 10 13 16 19 22 25 28 31 34" << endl <<
				"    2  5  8 11 14 17 20 23 26 29 32 35" << endl <<
				"    3  6  9 12 15 18 21 24 27 30 33 36" << endl << endl;
		cout << "Your choice: ";
		cin >> choice;

		while (choice <1 || choice >6) {
			cout << "That's not a proper option.  Try again: ";
			cin >> choice;
		}
	//	enum bet_types { SINGLE_NUM, RED_BLK, ODD_EVEN, SETS12, HALVES, COLUMNS};

		b.bets[i].type = choice-1;
		switch (b.bets[i].type) {
		case SINGLE_NUM	: BetSingleNum(CurrentCash,b,i); break;
		case RED_BLK 	: BetColors(CurrentCash,b,i); break;
		case ODD_EVEN 	: BetOddsEvens(CurrentCash,b,i); break;
		case SETS12 	: BetSets12(CurrentCash,b,i); break;
		case HALVES 	: BetHalves(CurrentCash,b,i); break;
		case COLUMNS 	: BetColumns(CurrentCash,b,i); break;
		}

	}

	return b;
}

/*
 *
 * Below: Bet Setting (mostly the same among the different bets)
 * Warning, this part is ugly... the rest of the code starts at around 290
 *
 */


void BetSingleNum(int & CurrentCash, Bets & b, short & i) {
	cout << "What number will you be betting on?";
	cin >> b.bets[i].value;
	while (b.bets[i].value <0 && b.bets[i].value >36) {
		cout << "Try again, please: ";
		cin >> b.bets[i].value;
	}
	cout << "Please enter how much you want to bet: ";
	cin >> b.bets[i].amount;
	while (b.bets[i].amount <=0) {
		cout << "Please enter an actual bid: ";
		cin >> b.bets[i].amount;
	}
	if (b.bets[i].amount > CurrentCash) {
		cout << "You cannot pay for that bet.  We're cutting you off of the rest of this round.";
		b.bets[i].type=10;
		b.num = i;
		i=10;	// 10>8, so this ends the previous loop.
	} else {
		CurrentCash -= b.bets[i].amount;
	}
}
void BetColors(int & CurrentCash, Bets & b, short & i) {
	cout << "What color, red ('1') or black ('2') will you be betting on?";
		cin >> b.bets[i].value;
		while (b.bets[i].value <0 && b.bets[i].value >2) {
			cout << "Try again, please: ";
			cin >> b.bets[i].value;
		}
		cout << "Please enter how much you want to bet: ";
		cin >> b.bets[i].amount;
		while (b.bets[i].amount <=0) {
			cout << "Please enter an actual bid: ";
			cin >> b.bets[i].amount;
		}
		if (b.bets[i].amount > CurrentCash) {
			cout << "You cannot pay for that bet.  We're cutting you off of the rest of this round.";
			b.bets[i].type=10;
			b.num = i;
			i=10;	// 10>8, so this ends the previous loop.
		} else {
			CurrentCash -= b.bets[i].amount;
		}
}
void BetOddsEvens(int & CurrentCash, Bets & b, short & i) {
	cout << "Will you be betting on odds ('1') or evens ('2') today? ";
		cin >> b.bets[i].value;
		while (b.bets[i].value <0 && b.bets[i].value >2) {
			cout << "Try again, please: ";
			cin >> b.bets[i].value;
		}
		cout << "Please enter how much you want to bet: ";
		cin >> b.bets[i].amount;
		while (b.bets[i].amount <=0) {
			cout << "Please enter an actual bid: ";
			cin >> b.bets[i].amount;
		}
		if (b.bets[i].amount > CurrentCash) {
			cout << "You cannot pay for that bet.  We're cutting you off of the rest of this round.";
			b.bets[i].type=10;
			b.num = i;
			i=10;	// 10>8, so this ends the previous loop.
		} else {
			CurrentCash -= b.bets[i].amount;
		}
}
void BetSets12(int & CurrentCash, Bets & b, short & i) {
	cout << "Which set of twelve will you be betting on?  ('1', '2', or '3')";
		cin >> b.bets[i].value;
		while (b.bets[i].value <0 && b.bets[i].value >3) {
			cout << "Try again, please: ";
			cin >> b.bets[i].value;
		}
		cout << "Please enter how much you want to bet: ";
		cin >> b.bets[i].amount;
		while (b.bets[i].amount <=0) {
			cout << "Please enter an actual bid: ";
			cin >> b.bets[i].amount;
		}
		if (b.bets[i].amount > CurrentCash) {
			cout << "You cannot pay for that bet.  We're cutting you off of the rest of this round.";
			b.bets[i].type=10;
			b.num = i;
			i=10;	// 10>8, so this ends the previous loop.
		} else {
			CurrentCash -= b.bets[i].amount;
		}
}
void BetHalves(int & CurrentCash, Bets & b, short & i) {
	cout << "Which half, '1'st or '2'nd, will you be betting on?";
		cin >> b.bets[i].value;
		while (b.bets[i].value <0 && b.bets[i].value >2) {
			cout << "Try again, please: ";
			cin >> b.bets[i].value;
		}
		cout << "Please enter how much you want to bet: ";
		cin >> b.bets[i].amount;
		while (b.bets[i].amount <=0) {
			cout << "Please enter an actual bid: ";
			cin >> b.bets[i].amount;
		}
		if (b.bets[i].amount > CurrentCash) {
			cout << "You cannot pay for that bet.  We're cutting you off of the rest of this round.";
			b.bets[i].type=10;
			b.num = i;
			i=10;	// 10>8, so this ends the previous loop.
		} else {
			CurrentCash -= b.bets[i].amount;
		}
}
void BetColumns(int & CurrentCash, Bets & b, short & i) {
	cout << "Which column will you be betting on?  ('1', '2', or '3') ";
		cin >> b.bets[i].value;
		while (b.bets[i].value <0 && b.bets[i].value >36) {
			cout << "Try again, please: ";
			cin >> b.bets[i].value;
		}
		cout << "Please enter how much you want to bet: ";
		cin >> b.bets[i].amount;
		while (b.bets[i].amount <=0) {
			cout << "Please enter an actual bid: ";
			cin >> b.bets[i].amount;
		}
		if (b.bets[i].amount > CurrentCash) {
			cout << "You cannot pay for that bet.  We're cutting you off of the rest of this round.";
			b.bets[i].type=10;
			b.num = i;
			i=10;	// 10>8, so this ends the previous loop.
		} else {
			CurrentCash -= b.bets[i].amount;
		}
}

/*
 * END setting bets
 *
 * Below is for comparing values
 */

short Roll() {	// Gets a random number between 0 and 36, inclusive
	int rndm = rand() %37;
	return rndm;
}

/*
 * Above:
 *
 * Below:Number comparison logic
 */

bool CheckNum(int roll, int bet) {		// function to check roll vs bet
	return (roll == bet);
}

short CheckOddEven(int roll) {
	if (roll!=0)
		if (roll%2==1)
			return ODD;
		else
			return EVEN;
	return NEITHER;
}

short CheckCol(int roll) {		// returns column number: 1, 2, 3, or 0 for #0
	if (roll != 0)
		switch (roll % 3) {
		case 1 : return FIRST_COL; break;
		case 2 : return SECOND_COL; break;
		case 0 : return THIRD_COL; break;
		}

	return ZERO_COL;
}

short CheckColor(int roll) {	// returns 1 (red), 2 (black), or 0 for #0
	int table[37] = { ZERO,	//  0,
			RED, BLK, RED,	//  1,  2,  3
			BLK, RED, BLK,	//  4,  5,  6
			RED, BLK, RED,	//  7,  8,  9
			BLK, BLK, RED,	// 10, 11, 12
			BLK, RED, BLK,	// 13, 14, 15
			RED, BLK, RED,	// 16, 17, 18
			RED, BLK, RED,	// 19, 20, 21
			BLK, RED, BLK,	// 22, 23, 24,
			RED, BLK, RED,	// 25, 26, 27,
			BLK, BLK, RED,	// 28, 29, 30,
			BLK, RED, BLK,	// 31, 32, 33,
			RED, BLK, RED,	// 34, 35, 36
	};
	return table[roll];
}

short Check12(int roll) {		// returns 1, 2, 3, or 0 for first, second, third set of 12, or 0 for 0
	if (roll > 0 && roll <= 12)
		return FIRST_12;
	else if (roll > 12 && roll <= 24)
		return SECOND_12;
	else if (roll > 24 && roll <= 36)
		return THIRD_12;

	return ZERO_12;
}

short CheckHalf(int roll) {
	if (roll>0 && roll<=18)
		return FIRST_HALF;
	if (roll>18)
		return SECOND_HALF;
	return ZERO_HALF;
}

void exit() {// Exit the program...
	cout << "\n" << "Press enter to exit. ";
	char c;
	cin.ignore();
	cin.get(c);
}
