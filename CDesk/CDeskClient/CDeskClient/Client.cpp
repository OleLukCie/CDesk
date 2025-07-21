#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyx.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

struct pack_header
{
    int type;	//	0 : TEXT	
    //	1 : IMAGE

    int size;	//	DATA SIZE
};

int main()
{
    printf("Starting CDesk application...\n");

    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        printf("WSAStartup failed with error: %d\n", wsaResult);
        return -1;
    }
    printf("WSAStartup succeeded.\n");

    HWND hDeskWnd = GetDesktopWindow();
    if (hDeskWnd == NULL) {
        printf("GetDesktopWindow failed with error: %d\n", GetLastError());
        WSACleanup();
        return -1;
    }
    printf("Desktop window handle retrieved successfully.\n");

    HDC hDeskDc = GetWindowDC(hDeskWnd);
    if (hDeskDc == NULL) {
        printf("GetWindowDC failed with error: %d\n", GetLastError());
        WSACleanup();
        return -1;
    }
    printf("Desktop device context retrieved successfully.\n");

    int hDeskWidth = GetDeviceCaps(hDeskDc, HORZRES);
    int hDeskHeight = GetDeviceCaps(hDeskDc, VERTRES);
    printf("Desktop dimensions: %dx%d\n", hDeskWidth, hDeskHeight);

    IMAGE BackgroundImage(hDeskWidth, hDeskHeight);
    printf("Image object created successfully.\n");

    HDC hImageDc = GetImageHDC(&BackgroundImage);
    if (hImageDc == NULL) {
        printf("GetImageHDC failed!\n");
        ReleaseDC(hDeskWnd, hDeskDc);
        WSACleanup();
        return -1;
    }
    printf("Image device context retrieved successfully.\n");

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("CREATE SOCKET FAILED with error: %d\n", WSAGetLastError());
        ReleaseDC(hDeskWnd, hDeskDc);
        WSACleanup();
        return -1;
    }
    printf("Socket created successfully.\n");

    struct sockaddr_in target = { 0 };
    target.sin_family = AF_INET;
    target.sin_port = htons(8080);
    target.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Attempting to connect to server at 127.0.0.1:8080...\n");
    int connectResult = connect(client_socket, (struct sockaddr*)&target, sizeof(target));
    if (connectResult == SOCKET_ERROR) {
        printf("CONNECT SERVER FAILED with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        ReleaseDC(hDeskWnd, hDeskDc);
        WSACleanup();
        return -1;
    }
    printf("Connected to server successfully.\n");

    printf("Entering main loop...\n");
    while (true) {
        BOOL bitBltResult = BitBlt(hImageDc, 0, 0, hDeskWidth, hDeskHeight, hDeskDc, 0, 0, SRCCOPY);
        if (!bitBltResult) {
            printf("BitBlt failed with error: %d\n", GetLastError());
            break;
        }

        printf("Saving image to c.jpg...\n");
        saveimage(TEXT("c.jpg"), &BackgroundImage);

        FILE* testFile = nullptr;
        errno_t err = fopen_s(&testFile, "c.jpg", "r");
        if (err != 0) {
            printf("Failed to save image file!\n");
            break;
        }
        fclose(testFile);
        printf("Image saved successfully.\n");

        struct pack_header hdr = { 1 };
        FILE* fp;
        err = fopen_s(&fp, "c.jpg", "rb");
        if (err != 0) {
            printf("FAILED TO OPEN FILE!\n");
            break;
        }
        if (fp == nullptr) continue;

        fseek(fp, 0, SEEK_END);
        hdr.size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (hdr.size <= 0) {
            fclose(fp);
            printf("File size is zero. Continuing...\n");
            continue;
        }
        printf("File size: %d bytes\n", hdr.size);

        int ret = send(client_socket, (const char*)&hdr, sizeof(hdr), 0);
        if (ret <= 0) {
            printf("send header failed with error: %d\n", WSAGetLastError());
            fclose(fp);
            break;
        }
        printf("Header sent successfully.\n");

        printf("Sending file content...\n");
        char buffer[1024] = { 0 };
        int totalSent = 0;
        while (1) {
            ret = fread(buffer, 1, 1024, fp);
            if (ret <= 0) {
                printf("Finished reading file.\n");
                break;
            }

            int send_ret = send(client_socket, buffer, ret, 0);
            if (send_ret <= 0) {
                printf("send data failed with error: %d\n", WSAGetLastError());
                break;
            }
            totalSent += send_ret;
        }
        fclose(fp);
        printf("File sent successfully. Total bytes sent: %d\n", totalSent);

        Sleep(1000);
    }

    printf("Closing socket...\n");
    closesocket(client_socket);

    printf("Releasing device context...\n");
    ReleaseDC(hDeskWnd, hDeskDc);

    printf("Cleaning up Winsock...\n");
    WSACleanup();

    printf("Application exiting normally.\n");
    return 0;
}