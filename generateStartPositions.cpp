#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "board.cpp"

using namespace std;
using namespace std::chrono;

typedef unsigned int US;

int main() {
    ofstream fout("startPositions.txt");
    auto start = high_resolution_clock::now();

    vector<Board> boards;
    int totalGames = 10000;
    int xWins = 0;
    int oWins = 0;
    int ties = 0;
    for (int zz = 0; zz < totalGames; zz++) {
        Board b = Board();
        bool changed = false;
        while (b.checkWin() == 0) {
            int x = (int)((double)rand()/RAND_MAX * 9);
            int y = (int)((double)rand()/RAND_MAX * 9);
            if (changed && (double)rand()/RAND_MAX < 0.4) boards.push_back(b.copy());
            changed = b.add(x, y);
        }
        if (b.checkWin() == 1) xWins++;
        else if (b.checkWin() == 2) oWins++;
        else ties++;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Total Games: " << totalGames << endl;
    cout << "X won: " << xWins << endl;
    cout << "O won: " << oWins << endl;
    cout << "Ties: " << ties << endl;
    cout << (double)duration.count()/1000 << " milliseconds elapsed" << endl;
    cout << "Saving " << boards.size() << " positions to file startPositions.txt" << endl;
    for (Board &b : boards) {
        fout << b.saveToFile() << endl;
    }
    fout.close();
}
