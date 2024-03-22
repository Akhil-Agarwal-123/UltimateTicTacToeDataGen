#include "evaluator.cpp"
#include <iostream>

using namespace std;

void updateWithAIMove(Board &initial) {
    cout << "It is the AI\'s turn. Please wait for it to think..." << endl;
    pair<int, int> ij = EvaluatorDFSMiniMax().getBestMove(initial, 1000);
    initial.add(ij.first, ij.second);
    cout << "The AI has placed their piece at row #" << (ij.first+1) << " and column #" << (ij.second+1) << endl;
}

void updateWithInput(Board &initial) {
    int rowNum, colNum;
    cout << "Your turn to place a piece!" << endl;
    if (!initial.canPlaceAnywhere()) {
        cout << "You must place your piece in the ";
        if (initial.boardPlayingOn() == 0) cout << "top left";
        else if (initial.boardPlayingOn() == 1) cout << "top middle";
        else if (initial.boardPlayingOn() == 2) cout << "top right";
        else if (initial.boardPlayingOn() == 3) cout << "left middle";
        else if (initial.boardPlayingOn() == 4) cout << "middle";
        else if (initial.boardPlayingOn() == 5) cout << "right middle";
        else if (initial.boardPlayingOn() == 6) cout << "bottom left";
        else if (initial.boardPlayingOn() == 7) cout << "bottom middle";
        else cout << "bottom right";
        cout << " tic-tac-toe grid." << endl;
    }
    bool valid = false;
    while (!valid) {
        cout << "Please enter the row number (1-9) to place your piece in: ";
        cin >> rowNum; rowNum--;
        cout << "Please enter the column number (1-9) to place your piece in: ";
        cin >> colNum; colNum--;
        valid = true;
        if (!initial.canPlaceAnywhere() &&
            (rowNum / 3 != initial.boardPlayingOn() / 3 || colNum / 3 != initial.boardPlayingOn() % 3)) {
            cout << "Please place the token in the correct square." << endl;
            valid = false;
        } else if (!initial.add(rowNum, colNum)) {
            cout << "That square is taken by another piece. Please try again." << endl;
            valid = false;
        }
    }
}

int main() {
    Board b;
    cout << "Would you like to be X or O?" << endl;
    string s; cin >> s;
    bool userTurn = s == "x" || s == "X";
    while (b.checkWin() == 0) {
        cout << b.toString();
        if (userTurn) {
            updateWithInput(b);
        } else {
            updateWithAIMove(b);
        }
        userTurn = !userTurn;
    }
    if (b.checkWin() == 3) cout << b.toString() << "We tied! GGs." << endl;
    else if ((b.checkWin() == 1) == (s == "x" || s == "X")) cout << b.toString() << "You won! Congrats." << endl;
    else cout << b.toString() << "You lost! L" << endl;
}