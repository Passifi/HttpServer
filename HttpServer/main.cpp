#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define SUCCESS 0
#define FAILURE 1
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
char        hoststr[NI_MAXHOST],
servstr[NI_MAXSERV];
char answer[] = "HTTP/1.1 200 OK\n\
Content-Type: text/html; charset=utf-8\n\
Content-Length: 178\n\
Connection: keep-alive\n\
Cache-Control: s-maxage=300, public, max-age=0\n\
Content-Language: en-US\n\
Date: Thu, 06 Dec 2018 17:37:18 GMT\n\
ETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\n\
Server: meinheld/0.6.1\n\
Strict-Transport-Security: max-age=63072000\n\
X-Content-Type-Options: nosniff\n\
X-Frame-Options: DENY\n\
X-XSS-Protection: 1; mode=block\n\
Vary: Accept-Encoding, Cookie\n\
Age: 7\n\n\
<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
<meta charset=\"utf-8\">\n\
<title>A simple webpage</title>\n\
</head>\n\
<body>\n\
<h1>Simple HTML webpage</h1>\n\
<p>Hello, world!</p>\n\
</body>\n\
</html>";

char testString[] = "<!DOCTYPE html>\n\
< html lang = \"en\">\n\
<head>\n\
<meta charset=\"utf-8\">\n\
<title>A simple webpage</title>\n\
</head>\n\
<body>\n\
<h1>Simple HTML webpage</h1>\n\
<p>Hello, world!</p>\n\
</body>\n\
</html>";

char** stringSplitter(char token, char* string) {
	char* currentString = (char*)malloc(sizeof(char)*1024);
	char** strings = (char**)malloc(sizeof(char*)*100);
	int currentIndex = 0;
	int currentStringNumber = 0;
	for (int i = 0; string[i]; i++) {
		if(string[i] != token) {
			currentString[currentIndex++] = string[i];
		}
		else {
			currentString[currentIndex] = '\0';
			strings[currentStringNumber++] = currentString;
			currentString = (char*)malloc(sizeof(char) * 1024);
			currentIndex = 0;
		}

	}
	return strings;

}

bool checkGet(char* string) {
	char ref[] = "GET";
	
	for (int i = 0; string[i] && i <3; i++) {
		if (ref[i] != string[i]) {
			return false;
		}
	}
	return true;
}

int main() {
	printf("%d\n",strlen(testString));
	char recvbuf[DEFAULT_BUFLEN];
	WSADATA wsadata;

	int iResult,iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	iResult = WSAStartup(MAKEWORD(2,2),&wsadata);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return FAILURE;
	}
	
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result);
	if (iResult != 0) {
		printf("getaddrinfo Failed: %d\n", iResult);
		WSACleanup();
		return FAILURE;
	}
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	iResult = bind(ListenSocket,result->ai_addr,(int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	for (int i = 0; result->ai_addr->sa_data[i]; i++) {
		printf("%x \n", result->ai_addr->sa_data[i]);
	}

	freeaddrinfo(result);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	iResult = getnameinfo(
		result->ai_addr,
		(socklen_t)result->ai_addrlen,
		hoststr,
		NI_MAXHOST,
		servstr,
		NI_MAXSERV,
		NI_NUMERICHOST | NI_NUMERICSERV
	);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	fprintf(stdout, "bound to address %s and port %s\n",
		 hoststr, servstr);

	SOCKET ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(ListenSocket,NULL,NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	closesocket(ListenSocket);
	
	do {
		iResult = recv(ClientSocket,recvbuf,recvbuflen,0);
		if (iResult > 0) {
			printf("Bytes received: %d\n",iResult);
			//printf("%s\n",recvbuf);
			char** strings = stringSplitter('\n',recvbuf);
			printf("%s\n",strings[0]);
			if(checkGet(strings[0])) {
			iSendResult = send(ClientSocket,answer,strlen(answer), 0);
			}
			else {
				continue;
			}
			if(iSendResult == SOCKET_ERROR) {
			
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;

			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return SUCCESS;
}