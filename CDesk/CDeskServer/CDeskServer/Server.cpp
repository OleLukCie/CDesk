#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyx.h>
#pragma comment(lib, "ws2_32.lib")

struct pack_header
{
	int type;	//	0 : TEXT	
	//	1 : IMAGE

	int size;	//	DATA SIZE
};

SOCKET create_listen_socket(unsigned short port)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sock) {
		printf("CREATE SOCKET FAILED!\n");
		return -1;
	}

	struct sockaddr_in local = { 0 };
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr("0.0.0.0");
	if (-1 == bind(sock, (struct sockaddr*)&local, sizeof(local))) {
		printf("BIND SOCKET FAILED!\n");
		return -1;
	}

	if (-1 == listen(sock, 10)) {
		printf("LISTEN SOCKET FAILED!\n");
		return -1;
	}

	return sock;
}

int main(int argc, char* argv[])
{
	initgraph(1280, 720);


	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET listen_socket = create_listen_socket(8080);

	struct sockaddr_in client_addr;
	int socklen = sizeof(struct sockaddr_in);

	IMAGE img;

	while (true)
	{
		SOCKET client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &socklen);

		if (INVALID_SOCKET == client_socket) continue;

		printf("NEW CONNECT, IP: %s, port: %d\n", inet_ntoa(client_addr.sin_addr), (client_addr.sin_port));

		while (true)
		{
			struct pack_header hdr;
			int ret = recv(client_socket, (char*)&hdr, sizeof(hdr), 0);
			if (ret <= 0)	break;
			if (hdr.type == 0) {

			}
			else if (hdr.type == 1)
			{
				FILE* fp;
				errno_t err = fopen_s(&fp, "s.jpg", "wb");
				if (err != 0) {
					printf("FAILED TO OPEN FILE!\n");
					break;
				}

				int size = hdr.size;
				char buffer[4096] = { 0 };
				while (size > 0) {
					ret = recv(client_socket, buffer, size < 4096 ? size : 4096, 0);
					if (ret <= 0) {
						if (fp != nullptr) {
							fclose(fp);
						}
						goto end;
					}
					size -= ret;
					if (fp != nullptr) {
						fwrite(buffer, 1, ret, fp);
					}
				}
				if (fp != nullptr) {
					fclose(fp);
				}
			}

			loadimage(&img, TEXT("s.jpg"), getwidth(), getheight());
			putimage(0, 0, &img);
		}
	end:
		closesocket(client_socket);
		SetWindowTextA(GetHWnd(), "SERVER");
		cleardevice();
	}
	return 0;
}