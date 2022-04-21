#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma once
#include <vector>
#include <string>

#pragma warning(disable: 4996)

using namespace std;

class Server {
public:
	SOCKET Connections[2];
	Server();
	void Send(int, int, string);
	string Recv(int);
};

struct Pos {
	int x;
	int y;

	Pos() {};
	Pos(int x, int y) : x(x), y(y) {}
	Pos CorrectInput(char a, char b) {
		Pos result(-1, -1);
		int x1 = int(a - 'A');
		int y1 = int(b - '1');
		if (x1 >= 0 && x1 < 8 && y1 >= 0 && y1 < 8) {
			result.x = 7 - y1;
			result.y = x1;
			return result;
		}
		x1 = int(a - '1');
		y1 = int(b - 'A');
		if (x1 >= 0 && x1 < 8 && y1 >= 0 && y1 < 8) {
			result.x = 7 - x1;
			result.y = y1;
			return result;
		}
		return result;
	}
};

enum class Type { Empty, Black, White, QueenBlack, QueenWhite };

class Figure {
public:
	Pos pos;
	Type type;

	Figure() {};
	Figure(Pos pos, Type type) : pos(pos), type(type) {};
};

class Board {
private:
	vector<Figure> figures;
public:
	Board();
	Board(const Board&);

	pair<int, int> Count();
	Type get_type(int, int);
	Figure get_figure(int, int);

	void Print() const;
	string compMsg();
	void Erase(Figure);
	void Insert(Figure);
	void Swap(Pos, Pos);
};

class Game {
public:
	char WhoseMove;
	Game();

	Pos InputPosition(Board&, Server&);
	vector<Pos> InputMoves(Pos&, Board&, bool&, int, Server&);
	bool Moves(Pos, Board&, vector<Pos>);
};