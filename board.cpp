#include <vector>
#include <sstream>

#include <iostream>
using namespace std;

typedef unsigned int US;

US hor1 = 1 << 0 | 1 << 1 | 1 << 2;
US hor2 = 1 << 3 | 1 << 4 | 1 << 5;
US hor3 = 1 << 6 | 1 << 7 | 1 << 8;
US ver1 = 1 << 0 | 1 << 3 | 1 << 6;
US ver2 = 1 << 1 | 1 << 4 | 1 << 7;
US ver3 = 1 << 2 | 1 << 5 | 1 << 8;
US diag1 = 1 << 0 | 1 << 4 | 1 << 8;
US diag2 = 1 << 2 | 1 << 4 | 1 << 6;
vector<US> masks[3][3] = {
        {{hor1, ver1, diag1}, {hor1, ver2}, {hor1, ver3, diag2}},
        {{hor2, ver1}, {hor2, ver2, diag1, diag2}, {hor2, ver3}},
        {{hor3, ver1, diag2}, {hor3, ver2}, {hor3, ver3, diag1}}
};


US** getBlankGrid() {
    US** ret = new US*[3];
    for (int i = 0; i < 3; ++i) {
        ret[i] = new US[3] {0, 0, 0};
    }
    return ret;
}

class Board {
private:
    US** o;
    US** x;
    US statuses[3] { 0, 0, 0 };
    int boardToPlayOn;
    bool xTurn, xWon, oWon;

    void add_x(int i, int j, int i2, int j2) {
        x[i][j] |= 1 << (i2 * 3 + j2);
        bool won = false;
        for (auto& mask : masks[i2][j2]) {
            if ((x[i][j] & mask) == mask) {
                won = true;
                break;
            }
        }
        if (won) {
            statuses[0] |= 1 << (i * 3 + j);
            for (auto& mask : masks[i][j]) {
                if ((statuses[0] & mask) == mask) {
                    xWon = true;
                }
            }
        }
        if (!won && (x[i][j] | o[i][j]) == (1 << 9) - 1) {
            statuses[2] |= 1 << (i * 3 + j);
        }
    }

    void add_o(int i, int j, int i2, int j2) {
        o[i][j] |= 1 << (i2 * 3 + j2);
        bool won = false;
        for (auto& mask : masks[i2][j2]) {
            if ((o[i][j] & mask) == mask) {
                won = true;
                break;
            }
        }
        if (won) {
            statuses[1] |= 1 << (i * 3 + j);
            for (auto& mask : masks[i][j]) {
                if ((statuses[1] & mask) == mask) {
                    oWon = true;
                }
            }
        }
        if (!won && (x[i][j] | o[i][j]) == (1 << 9) - 1) {
            statuses[2] |= 1 << (i * 3 + j);
        }
    }

    char getChar(int i, int j, int i2, int j2) {
        if (((x[i][j] >> (i2 * 3 + j2)) & 1) == 1) {
            return 'X';
        } else if (((o[i][j] >> (i2 * 3 + j2)) & 1) == 1) {
            return 'O';
        }
        return '-';
    }

public:
    Board() {
        o = getBlankGrid();
        x = getBlankGrid();
        boardToPlayOn = -1;
        xTurn = true;
        xWon = oWon = false;
    }

    Board(US** o, US** x, const US* statuses, int boardToPlayOn, bool xTurn, bool xWon, bool oWon) {
        this->o = new US*[3];
        for (int i = 0; i < 3; i++) {
            this->o[i] = new US[3] { o[i][0], o[i][1], o[i][2] };
        }
        this->x = new US*[3];
        for (int i = 0; i < 3; i++) {
            this->x[i] = new US[3] { x[i][0], x[i][1], x[i][2] };
        }
        this->statuses[0] = statuses[0];
        this->statuses[1] = statuses[1];
        this->statuses[2] = statuses[2];
        this->boardToPlayOn = boardToPlayOn;
        this->xTurn = xTurn;
        this->xWon = xWon;
        this->oWon = oWon;
    }

    Board(const string& fullLine) {
        istringstream iss(fullLine);
        *this = Board();
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) iss >> x[i][j];
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) iss >> o[i][j];
        for (auto &a : statuses) iss >> a;
        iss >> boardToPlayOn >> xTurn >> xWon >> oWon;
    }

    bool add(int i, int j) {
        int i2 = boardToPlayOn == -1 ? i / 3 : boardToPlayOn / 3;
        int j2 = boardToPlayOn == -1 ? j / 3 : boardToPlayOn % 3;
        if ((((statuses[0] | statuses[1] | statuses[2]) >> (i2*3 + j2)) & 1) == 1) return false;
        if ((((x[i2][j2] | o[i2][j2]) >> ((i%3)*3+(j%3))) & 1) == 1) return false;
        if (xTurn) {
            add_x(i2, j2, i % 3, j % 3);
        } else {
            add_o(i2, j2, i % 3, j % 3);
        }
        boardToPlayOn = (i%3) * 3 + (j%3);
        if ((((statuses[0] | statuses[1] | statuses[2]) >> boardToPlayOn) & 1) == 1) {
            boardToPlayOn = -1;
        }
        xTurn = !xTurn;
        return true;
    }

    int checkWin() {
        if (xWon) return 1;
        else if (oWon) return 2;
        if ((statuses[0] | statuses[1] | statuses[2]) == (1 << 9) - 1) {
            return 3;
        }
        return 0;
    }

    int winsByX() {
        int cnt = 0;
        US a = statuses[0];
        while (a > 0) {
            if (a & 1) cnt++;
            a >>= 1;
        }
        return cnt;
    }

    int winsByO() {
        int cnt = 0;
        US a = statuses[1];
        while (a > 0) {
            if (a & 1) cnt++;
            a >>= 1;
        }
        return cnt;
    }

    US xStatus() { return statuses[0]; }
    US oStatus() { return statuses[1]; }

    string toString() {
        string s;
        for (int i = 0; i < 3; i++) {
            for (int i2 = 0; i2 < 3; i2++) {
                for (int j = 0; j < 3; j++) {
                    for (int j2 = 0; j2 < 3; j2++) {
                        s += getChar(i, j, i2, j2);
                    }
                    s += "  ";
                }
                s += "\n";
            }
            s += "\n";
        }
        return s;
    }

    string saveToFile() {
        string s;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                s += to_string(x[i][j]) + " ";
            }
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                s += to_string(o[i][j]) + " ";
            }
        }
        for (auto status : statuses) s += to_string(status) + " ";
        s += to_string(boardToPlayOn) + " " + to_string(xTurn) + " " + to_string(xWon)
                + " " + to_string(oWon);
        return s;
    }

    bool canPlaceAnywhere() { return boardToPlayOn == -1; }
    int boardPlayingOn() { return boardToPlayOn; }

    bool isXTurn() { return xTurn; }

    Board copy() {
        return {o, x, statuses, boardToPlayOn, xTurn, xWon, oWon};
    }

    bool operator<(const Board& loc2) const
    {
        return o[0][0] < loc2.o[2][2];
    }

    void del() {
        delete x;
        delete o;
    }
};