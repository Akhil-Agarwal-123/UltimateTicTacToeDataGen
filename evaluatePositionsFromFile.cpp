#include <iostream>
#include <fstream>
#include "evaluator.cpp"

using namespace std;
using namespace std::chrono;

int main() {
    int terminalSpot = 2;
    ifstream fin("/Users/akhil/Desktop/Coding/UltimateTicTacToeGameGenerator/cmake-build-debug/startPositions.txt");
    ofstream fout("/Users/akhil/Desktop/Coding/UltimateTicTacToeGameGenerator/cmake-build-debug/positionsWithEvals" + to_string(terminalSpot+1) + ".txt");
    string s; getline(fin, s);
    // 26152, 29000, 33000, 37000, 41000, 45000
    int st = 29000 + 4000 * (terminalSpot - 2);
    for (int i = 0; i < st; i++) getline(fin, s);
    for (int i = st; i < st + 4000; i++) { // total 230385
        Board b(s);
        double val = EvaluatorDFSMiniMax().getTrueEval(b, 200);
        cout << "(" << i+1 << ")\t" << val << endl;
        fout << b.saveToFile() << " " << val << endl;
        b.del();
        getline(fin, s);
    }
    fout.close();

//    cout << b.toString();
//    cout << "X currently has " << b.winsByX() << " wins" << endl;
//    cout << "O currently has " << b.winsByO() << " wins" << endl;
//    cout << "It is " << (b.isXTurn() ? 'X' : 'O') << "\'s turn on board " << b.boardPlayingOn() << endl;
//
//    auto start = high_resolution_clock::now();
//    cout << "Minimax gives eval " << EvaluatorDFSMiniMax().getTrueEval(b, 200);
//    auto stop = high_resolution_clock::now();
//    auto duration = duration_cast<microseconds>(stop - start);
//    cout << " in " << (double)duration.count()/1000 << " milliseconds" << endl;
}