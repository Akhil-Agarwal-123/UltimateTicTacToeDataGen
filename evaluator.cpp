#include "board.cpp"
#include <chrono>
#include <vector>
#include <thread>
#include <queue>

using namespace std;
using namespace std::chrono;

class Evaluator {
protected:
    vector<double> threadResults;
    virtual double noDepthEval(Board &b) = 0;
    virtual void addThread(Board b, int indexNum, double maxTime) {
        threadResults[indexNum] = getTrueEval(b, maxTime);
        b.del();
    }
public:
    virtual double getTrueEval(Board &b, double maxTime) = 0;
    pair<int, int> getBestMove(Board &initial, double maxTime) {
        vector<thread> threads;
        vector<pair<int, int>> ijUsed;
        threadResults.clear();
        for (int i = 0; i < (initial.canPlaceAnywhere() ? 9 : 3); i++) {
            for (int j = 0; j < (initial.canPlaceAnywhere() ? 9 : 3); j++) {
                Board nextBoard = initial.copy();
                if (nextBoard.add(i, j)) {
                    // start threading it!
                    threadResults.push_back(0);
                    threads.emplace_back(&Evaluator::addThread, this, nextBoard, threadResults.size()-1, maxTime);
                    if (!initial.canPlaceAnywhere()) {
                        ijUsed.emplace_back((initial.boardPlayingOn()/3) * 3 + i, (initial.boardPlayingOn()%3) * 3 + j);
                    } else {
                        ijUsed.emplace_back(i, j);
                    }
                }
            }
        }
        pair<int, int> best;
        double bestEval = initial.isXTurn() ? -1000 : 1000;
        for (int i = 0; i < ijUsed.size(); i++) {
            threads[i].join();
            if ((initial.isXTurn() && threadResults[i] > bestEval) || (!initial.isXTurn() && threadResults[i] < bestEval)) {
                bestEval = threadResults[i];
                best = ijUsed[i];
            }
        }
        return best;
    }
};

class EvaluatorDFSMiniMax : public Evaluator {
protected:
    double minimax(Board &b, int maxDepth, int depth, bool isX, double alpha, double beta) {
        if (b.checkWin() != 0) return b.checkWin() == 1 ? 300 : b.checkWin() == 2 ? -300 : 0;
        if (depth >= maxDepth) return noDepthEval(b);

        double bestVal = isX ? -1000 : 1000;
        bool exit = false;
        Board b2 = b.copy();
        for (int i = 0; i < (b.canPlaceAnywhere() ? 9 : 3) && !exit; i++) {
            for (int j = 0; j < (b.canPlaceAnywhere() ? 9 : 3) && !exit; j++) {
                if (b2.add(i, j)) {
                    double val = minimax(b2, maxDepth, depth + (b.canPlaceAnywhere() ? 2 : 1), !isX, alpha, beta);
                    bestVal = isX ? max(bestVal, val) : min(bestVal, val);
                    if (isX) alpha = max(alpha, bestVal);
                    else beta = min(beta, bestVal);
                    if (beta <= alpha) exit = true;
                    b2.del();
                    b2 = b.copy();
                }
            }
        }
        b2.del();
        return bestVal;
    }
    double noDepthEval(Board &b) override {
        int xWins = b.winsByX(), oWins = b.winsByO();
        double eval = xWins * 10 - oWins * 10;
        US x = b.xStatus();
        US o = b.oStatus();
        for (int i = 0; i < 9; i++) {
            if ((((x | o) >> i) & 1) != 1) {
                x |= 1 << i;
                o |= 1 << i;
                for (auto mask : masks[i/3][i%3]) {
                    if ((x & mask) == mask) eval += 5;
                    if ((o & mask) == mask) eval -= 5;
                }
                x = b.xStatus();
                o = b.oStatus();
            }
        }
        return eval;
    }
public:
    double getEvalDFS(Board &b, double maxTime) {
        auto start = high_resolution_clock::now();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        int depth = 1;
        double eval = 0;
        while (depth < 12 && (double)duration.count()/1000 < maxTime) {
            start = high_resolution_clock::now();
            eval = minimax(b, depth, 0, b.isXTurn(), -1000, 1000);
            end = high_resolution_clock::now();
            duration = duration_cast<microseconds>(end - start);
            depth++;
        }
        return eval;
    }
    double getTrueEval(Board &b, double maxTime) override { return getEvalDFS(b, maxTime); }
};
