#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <windows.h>
#include <string>
#include "Game.h"

#pragma warning(disable: 4996)

using namespace std;

Server::Server() {
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
	if (newConnection == 0)
		cout << "Error #2\n";
	else
		cout << "Client1 Connected!\n";

	Connections[0] = newConnection;

	newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
	if (newConnection == 0) {
		cout << "Error #2\n";
	}
	else {
		cout << "Client2 Connected!\n";
	}

	Connections[1] = newConnection;

}

void Server::Send(int indx, int msgType, string msg) {
	send(Connections[indx], (char*)&msgType, sizeof(int), NULL);
	int msg_size = msg.size();
	send(Connections[indx], (char*)&msg_size, sizeof(int), NULL);
	send(Connections[indx], msg.c_str(), msg_size, NULL);
}

string Server::Recv(int indx) {
	int msg_size;
	recv(Connections[indx], (char*)&msg_size, sizeof(int), NULL);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(Connections[indx], msg, msg_size, NULL);
	string result = string(msg);
	delete[] msg;

	return result;
}

Board::Board() {
	for (int i = 0; i < 8; i++) {
		if (i == 3 || i == 4) {
			for (int j = 0; j < 8; j++) {
				Figure figure(Pos(i, j), Type::Empty);
				figures.push_back(figure);
			}
			continue;
		}
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2)
				if (i < 3) {
					Figure figure(Pos(i, j), Type::Black);
					figures.push_back(figure);
				}
				else {
					Figure figure(Pos(i, j), Type::White);
					figures.push_back(figure);
				}
			else {
				Figure figure(Pos(i, j), Type::Empty);
				figures.push_back(figure);
			}
		}
	}

}

Board::Board(const Board& b) {
	this->figures = b.figures;
}

string Board::compMsg() {
	string result = "";

	char output[8][8];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			output[i][j] = ' ';

	for (int i = 0; i < int(figures.size()); i++) {
		Pos pos = figures[i].pos;
		Type type = figures[i].type;
		if (type == Type::Black)
			output[pos.x][pos.y] = 'b';
		else
			if (type == Type::White)
				output[pos.x][pos.y] = 'w';
			else
				if (type == Type::QueenBlack)
					output[pos.x][pos.y] = 'B';
				else
					if (type == Type::QueenWhite)
						output[pos.x][pos.y] = 'W';
	}

	result += string("   ");
	for (int i = 0; i < 8; i++)
		result += string("  ") + char(65 + i) + string(" ");
	result += '\n';

	result += "   ";
	for (int i = 0; i < 4 * 8 + 1; i++)
		result += string("-");
	result += '\n';

	for (int i = 0; i < 8; i++) {
		result += string(" ") + char(8 - i + 48) + string(" ");
		for (int j = 0; j < 8; j++)
			result += string("| ") + output[i][j] + string(" ");
		result += string("| ") + char(8 - i + 48) + '\n';

		result += string("   ");
		for (int j = 0; j < 4 * 8 + 1; j++)
			result += '-';
		result += '\n';
	}

	result += string("   ");
	for (int i = 0; i < 8; i++)
		result += string("  ") + char(int('A') + i) + ' ';

	result += '\n';

	return result;
}

pair<int, int> Board::Count() {
	int Count_B = 0;
	int Count_W = 0;
	for (int i = 0; i < int(this->figures.size()); i++)
		if (figures[i].type == Type::Black || figures[i].type == Type::QueenBlack)
			Count_B++;
		else
			if (figures[i].type == Type::White || figures[i].type == Type::QueenWhite)
				Count_W++;
	return { Count_B, Count_W };
}

Type Board::get_type(int x, int y) {
	for (int i = 0; i < int(figures.size()); i++) {
		Pos posf = figures[i].pos;
		Type typef = figures[i].type;
		if (posf.x == x && posf.y == y)
			return typef;
	}

	return Type::Empty;
}

Figure Board::get_figure(int x, int y) {
	for (int i = 0; i < int(this->figures.size()); i++)
		if (this->figures[i].pos.x == x && this->figures[i].pos.y == y)
			return this->figures[i];
}

void Board::Erase(Figure figure) {
	for (int i = 0; i < int(this->figures.size()); i++) {
		if (figures[i].pos.x == figure.pos.x && figures[i].pos.y == figure.pos.y && figures[i].type == figure.type)
			figures.erase(figures.begin() + i);
	}
}


void Board::Insert(Figure figure) {
	this->figures.push_back(figure);
}

void Board::Swap(Pos pos1, Pos pos2) {
	Figure f1 = get_figure(pos1.x, pos1.y);
	Erase(f1);
	Figure f2 = get_figure(pos2.x, pos2.y);
	Erase(f2);
	swap(f1.pos, f2.pos);

	if (f1.type == Type::White && f1.pos.x == 0)
		f1.type = Type::QueenWhite;
	if (f1.type == Type::Black && f1.pos.x == 7)
		f1.type = Type::QueenBlack;

	Insert(f1);
	Insert(f2);

}

Pos Game::InputPosition(Board& board, Server& SERVER) {
	int indx = WhoseMove == 'W' ? 0 : 1;
	char x, y;
	Pos PosInput = Pos();

	while (true) {
		SERVER.Send(indx, 1, string("Enter the coordinates of the figure you want to move: "));
		string input = SERVER.Recv(indx);
		
		int cnt = 0;
		for (int i = 0; i < int(input.size()); i++) {
			if (input[i] == ' ')
				continue;
			if (cnt == 0) {
				x = input[i];
				cnt++;
			}
			else
				if (cnt == 1) {
					y = input[i];
					cnt++;
				}
				else
					cnt++;
		}
		
		if (cnt != 2)
			PosInput = Pos(-1, -1);
		else 
			PosInput = PosInput.CorrectInput(x, y);

		if (PosInput.x > -1 && PosInput.y > -1) {
			Type type = board.get_type(PosInput.x, PosInput.y);
			if (this->WhoseMove == 'W' && (type == Type::White || type == Type::QueenWhite))
				break;

			if (this->WhoseMove == 'B' && (type == Type::Black || type == Type::QueenBlack))
				break;
			SERVER.Send(indx, 0, string(char(this->WhoseMove) + string("'s turn to move") + '\n'));
		}
		else
			SERVER.Send(indx, 0, string("Data is incorrect. Try again!\n"));
	}
	return PosInput;
}

vector<Pos> Game::InputMoves(Pos& PosInput, Board& board, bool& change, int indx, Server& SERVER) {
	vector<Pos> moves;
	bool info = 1;
	
	while (true) {
		char in[2] = { ' ', ' ' };
		string input = "";
		int cnt_data = 0;
		moves.clear();
		if (info)
			SERVER.Send(indx, 1, string("Enter \"change\" to change the coordinates of the figure you want to move or enter the coordinates where you want to move the figure : \n"));
		else 
			SERVER.Send(indx, 1, string("Enter \"change\" to change the coordinates of the figure you want to move or enter the coordinates where you want to move the figure : \n"));
		input = SERVER.Recv(indx);

		if (!(input.size())) {
			info = 0;
			continue;
		}

		auto j = input.find("change");
		if (j != string::npos) {
			change = 1;
			return moves;
		}

		info = 1;
		for (char i : input) {
			if (i == ' ')
				continue;
			in[cnt_data % 2] = i;
			if (cnt_data % 2) {
				Pos pos = Pos();
				pos = pos.CorrectInput(in[0], in[1]);
				Figure figure = board.get_figure(pos.x, pos.y);
				if ((figure.type != Type::Empty) || (pos.x == -1 && pos.y == -1)) {
					if (cnt_data > 0 && (figure.pos.x == PosInput.x && figure.pos.y == PosInput.y)) {}
					else {
						moves.clear();
						break;
					}
				}
				moves.push_back(pos);
			}
			cnt_data++;
		}

		if (cnt_data % 2 || moves.empty()) {
			SERVER.Send(indx, 0, string("Data is incorrect. Try again!\n"));
			continue;
		}

		break;
	}
	return moves;
}

bool Game::Moves(Pos curpos, Board& board, vector<Pos> moves) {
	bool Move1 = 0;
	bool Move2 = 0;
	bool Move3 = 0;
	Figure ConstFigure = board.get_figure(curpos.x, curpos.y);
	Type TypeConstFigure = ConstFigure.type;

	for (Pos i : moves) {
		if (Move1)
			return false;

		if (TypeConstFigure == Type::QueenBlack || TypeConstFigure == Type::QueenWhite) {
			if (abs(curpos.x - i.x) == abs(curpos.y - i.y)) {
				Figure figure = board.get_figure(curpos.x, curpos.y);
				if (figure.type == Type::QueenBlack || figure.type == Type::QueenWhite) {
					int sx = curpos.x < i.x ? 1 : -1, sy = curpos.y < i.y ? 1 : -1;
					int r = curpos.x + sx, c = curpos.y + sy;
					int cnt = 0;
					while (r != i.x && c != i.y) {
						Figure cur = board.get_figure(r, c);
						if ((this->WhoseMove == 'W' && (cur.type == Type::Black || cur.type == Type::QueenBlack)) ||
							(this->WhoseMove == 'B' && (cur.type == Type::White || cur.type == Type::QueenWhite)))
						{
							cnt++;
							Figure ins = Figure(cur.pos, Type::Empty);
							board.Erase(cur);
							board.Insert(ins);
							board.Swap(curpos, i);
							Move3 = 1;
						}
						if (cnt > 1)
							return false;
						r += sx, c += sy;
					}

					if (cnt == 0) {
						board.Swap(curpos, i);
						Move1 = true;
						if (Move3)
							return false;
					}
					curpos = i;
					continue;
				}
			}
		}

		int r = this->WhoseMove == 'W' ? 1 : -1;
		if ((curpos.x - i.x == r && curpos.y - i.y == r) || (curpos.x - i.x == r && i.y - curpos.y == r)) {
			if (Move2)
				return false;
			board.Swap(curpos, i);
			curpos = i;
			Move1 = true;
			continue;
		}

		if (abs(curpos.x - i.x) == 2 && abs(curpos.y - i.y) == 2) {
			Figure remove = board.get_figure((curpos.x + i.x) / 2, (curpos.y + i.y) / 2);
			if ((this->WhoseMove == 'W' && (remove.type == Type::Black || remove.type == Type::QueenBlack)) ||
				(this->WhoseMove == 'B' && (remove.type == Type::White || remove.type == Type::QueenWhite)))
			{
				Figure ins = Figure(remove.pos, Type::Empty);
				board.Erase(remove);
				board.Insert(ins);
				board.Swap(curpos, i);
				curpos = i;
				Move2 = true;
				continue;
			}
		}

		return false;
	}

	return true;
}

Game::Game() {
	Server SERVER = Server();
	Board board = Board();
	WhoseMove = 'W';
	int indx = 0;
	char x, y;
	bool ok = true;
	while (true) {
		vector<Pos> moves;
		pair<int, int> cnt = board.Count();
		Pos PosInput = Pos();
		bool change = 0;
		
		string print = board.compMsg();
		//cout << print.size() << endl;
		int msg_size = print.size();

		print += '\t' + string("Score") + '\t' + '\n';
		print += string("Black: ") + char(12 - cnt.second + 48) + '\t' + string("White: ") + char(12 - cnt.first + 48) + '\n' + '\n';
		print += WhoseMove + string("'s turn to move") + '\n';

		SERVER.Send(0, 0, print);
		SERVER.Send(1, 0, print);

		if (cnt.first == 0) {
			SERVER.Send(0, -1, "Game over. You won!");
			SERVER.Send(1, -1, "Game over. You lose!");
			cout << "Game over. W's won!" << endl;
			break;
		}
		if (cnt.second == 0) {
			SERVER.Send(1, -1, "Game over. You won!");
			SERVER.Send(0, -1, "Game over. You lose!");
			cout << "Game over. B's won!" << endl;
			break;
		}

		PosInput = InputPosition(board, SERVER);
		cout << this->WhoseMove << "'s entered coordinates" << endl;

		while (true) {
			moves.clear();

			moves = InputMoves(PosInput, board, change, indx, SERVER);
			if (change)
				break;
			Board ConstBoard = board;
			ok = Moves(PosInput, board, moves);
			if (ok)
				break;
			SERVER.Send(indx, 0, string("Data is incorrect. Try again!\n"));
			board = ConstBoard;
		}
		if (change) {
			continue;
		}

		if (WhoseMove == 'W') {
			WhoseMove = 'B';
			indx = 1;
		}
		else {
			WhoseMove = 'W';
			indx = 0;
		}
	}
}