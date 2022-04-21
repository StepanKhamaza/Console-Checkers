#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <string>

#pragma warning(disable: 4996)

using namespace std;

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";

	while (true) {
		int msgType;
		int msg_size;
		recv(Connection, (char*)&msgType, sizeof(int), NULL);
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		std::cout << msg << std::endl;

		if (msgType == -1)
			break;
		else
		if (msgType == 1) {
			string input;
			getline(cin, input);
			msg_size = input.size();
			send(Connection, (char*)&msg_size, sizeof(int), NULL);
			send(Connection, input.c_str(), msg_size, NULL);
		}
	}

	system("pause");
	return 0;
}