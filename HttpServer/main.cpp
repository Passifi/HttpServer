#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define powInt(base,power) (int)pow((double)base,(double)power)
#define SUCCESS 0
#define FAILURE 1
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

enum AnswerType {
	GET,
	POST
};

enum ResponseType{
	GET_RESP,
	POST_RESP,
	NO_FILE
};

struct Responses {
	 char* ResponseData;
	 char* contentType;
	 char* ContentLength;
	 char* Connection;
	 char* CacheControl;
	 char* ContentLanguage;
	 char* Date;
	 char* ETag;
	 char* Server;
	 char* StrictTransportSec;
	 char* XContentOptions;
	 char* XFrameOptions;
	 char* XProtection;
	 char* Vary;
	 char* Age;
	 char* Content;

} typedef Response;

char* loadFile(char* path);
char* insertValue(int value, char* string);
char* addStrings(char* s1, char* s2);
char        hoststr[NI_MAXHOST],
servstr[NI_MAXSERV];

Response Okay;
Response Fail;

Response initResponse(ResponseType type) {
	
	Response result;

	switch (type) {
		case GET:
			result.ResponseData = (char*)"HTTP/1.1 200 OK";
			break;
		case NO_FILE:
			result.ResponseData = (char*)"HTTP/1.1 404 Not Found";
			break;
	}

	result.contentType = (char*)"Content-Type: text/html; charset=utf-8";
	result.ContentLength = (char*)"Content-Length: {}";
	
		result.Connection = (char*)"Connection: keep - alive";
		result.CacheControl = (char*)"Cache-Control: s - maxage = 300, public, max - age = 0";
		result.ContentLanguage = (char*)"Content-Language: en - US";
		result.Date = (char*)"Date: Thu, 06 Dec 2018 17 : 37 : 18 GMT";
		result.ETag = (char*)"ETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"";
		result.Server = (char*)"Server: Experimental Server/0.6.1";
		result.StrictTransportSec = (char*)"Strict-Transport-Security: max-age=63072000";
		result.XContentOptions = (char*)"X-Content-Type-Options: nosniff";
		result.XFrameOptions = (char*)"X-Frame-Options: DENY";
		result.XProtection = (char*)"X-XSS-Protection: 1; mode=block";
		result.Vary = (char*)"Vary: Accept-Encoding, Cookie";
		result.Age = (char*)"Age: 7";


	return result;

}

char* createAnswerFromStruct(Response ref, char* path) {
	int length =0;
	char* document = NULL;
	if (path != NULL) {
		document = loadFile(path);
		length = strlen(document);
	}
	if(strcmp(document,"NO FILE")==0) 
	{
		ref = initResponse(NO_FILE);
		length = 0;
		document = NULL;
	}
	char* contentLength = insertValue(length,ref.ContentLength);
	char* result = (char*)" ";
	result = addStrings(result,ref.ResponseData);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result,ref.contentType);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, contentLength);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.Connection);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.CacheControl);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.ContentLanguage);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.Date);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.ETag);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.Server);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.StrictTransportSec);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.XContentOptions);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.XFrameOptions);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.XProtection);
	result = addStrings(result, (char*)"\n");
	result = addStrings(result, ref.Vary);
	result = addStrings(result, (char*)"\n\n");
	if (document != NULL) {
		result = addStrings(result,document);
	}

	return result;
}

char okHeader[] = "HTTP/1.1 200 OK\n\
Content-Type: text/html; charset=utf-8\n\
Content-Length: {}\n\
Connection: keep-alive\n\
Cache-Control: s-maxage=300, public, max-age=0\n\
Content-Language: en-US\n\
Date: Thu, 06 Dec 2018 17:37:18 GMT\n\
ETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\n\
Server: MyOwnServer/0.6.1\n\
Strict-Transport-Security: max-age=63072000\n\
X-Content-Type-Options: nosniff\n\
X-Frame-Options: DENY\n\
X-XSS-Protection: 1; mode=block\n\
Vary: Accept-Encoding, Cookie\n\
Age: 7\n\n";

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

char failed[] = "HTTP/1.1 404 Not Found\n\
Content-Type: text/html; charset=utf-8\n\
Content-Length:\n\
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

int cmpStr(char* str1, char* str2) {
	char* ptr1 = str1;
	char* ptr2 = str2;

	while ((*ptr1 && *ptr2) && (*ptr1++ == *ptr2++));
	*ptr1--;
	*ptr2--;
	return *ptr1 == *ptr2;

}

char* concatStr(char* str1, char* str2) {
	int size = strlen(str1) + strlen(str2) + 1;
	char* result = (char*)malloc(sizeof(char) * size);
	int i;
	for (i = 0; str1[i]; i++) {
		result[i] = str1[i];
	}
	for (int k = 0; str2[k]; k++) {
		result[i + k] = str2[k];
	}
	result[size] = '\0';
	return result;
}



char* copyString(char* source, int start, int length) {
	char* result = (char*)malloc(sizeof(char) * length);
	char* tmp = result;
	char* ptr = source + start;
	while (*result++ = *ptr++);

	return tmp;
}

int getValueLength(int value) {
	int i = 0;
	while (value / 10 > 0) {
		value = value / 10;
		i++;
	}
	return i;
}

char* insertValue(int value, char* string) {

	int extra = getValueLength(value);
	char* conversion = (char*)malloc(sizeof(char) * extra);
	sprintf(conversion, "%d", value);
	char* conPointer = conversion;
	int len = strlen(string)+1;
	char* result = (char*)malloc(sizeof(char) * (extra + len));
	char* ptr = result;
	for (int i = 0; string[i]; i++) {
		if (string[i] == '{') {
			while (*ptr++ = *conPointer++);
			*ptr--;
			while (string[i] != '}') {
				i++;
				if (!string[i]) {
					break;
				}
			}
		}
		else {

			*ptr++ = string[i];
		}
	}
	*ptr = '\0';
	//free(conversion);
	return result;
}


char* loadFile( char* path) {
	
	if (strcmp(path, "/") == 0) {
		strcpy(path,"index.html");
	}
	FILE *file = fopen(path,"r");
	if (file == NULL) {
		return (char*)"NO FILE";
	}
	char* document = (char*)malloc(sizeof(char)*2048);
	char line[256];
	int index = 0;
	while (fgets(line, 256, file)) {
		for (int i = 0; line[i] && index < 2048; i++) {
			document[index++] = line[i];
		}
	}
	document[index] = '\0';
	return document;
}

char** stringSplitter(char token, char* string) {
	char* currentString = (char*)malloc(sizeof(char)*1024);
	char** result = (char**)malloc(sizeof(char*)*100);
	int currentIndex = 0;
	int currentStringNumber = 0;
	int length = 0;
	for (int i = 0; string[i]; i++) {
		if(string[i] != token) {
			currentString[currentIndex++] = string[i];
		}
		else {
			currentString[currentIndex] = '\0';
			result[currentStringNumber++] = currentString;
			currentString = (char*)malloc(sizeof(char) * 1024);
			currentIndex = 0;
			length++;
		}

	}
	
	return result;

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

char* addStrings(char* s1, char* s2) {
	int size = strlen(s1) + strlen(s2);
	char* newString = (char*)malloc(sizeof(char)*size);
	int i = 0;
	for (; s1[i]; i++) {
		newString[i] = s1[i];
	}
	for (int k = 0; s2[k]; k++) {
		newString[i++] = s2[k];
	}
	newString[i] = '\0';
	return newString;
}

int initWSA() {
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	WSADATA wsadata;
	iResult = WSAStartup(MAKEWORD(2,2),&wsadata);
	if (iResult != 0) {
			printf("WSA startup has failed! %d\n",iResult);
			return FAILURE;
	}
	
	return SUCCESS;

}

void addLengthInfo(char* data) {
	int length = strlen(data);
	char *firstPart;
	char *secondPart;
	for (int i = 0; answer[i]; i++)
	{
		if (answer[i] = '{') {
			
		}
	}
}

int bindSocket(int portNo, struct addrinfo* result, struct addrinfo &hints) {
	char* port = NULL;
	_itoa(portNo,port,10);
	int iResult;
	//struct addrinfo* ptr = NULL;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL,port,&hints,&result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n",iResult);
		WSACleanup();
		return FAILURE;
	}
}

char* createAnswer(int type, char* path) {
	char* result = NULL;
	switch (type) {
		case GET: 
			char ref[] = "/";
			printf("Path is: %s\n",path);
			if (cmpStr(path,ref)) {
				path = (char*)"index.html";
			}
			printf("Path is: %s\n", path);

			char* document = loadFile(path);
			if (cmpStr(document, (char*)"NO FILE")) {
				return failed;
			}

			int length = strlen(document);
			char* buffer = insertValue(length,okHeader);

			result = concatStr(buffer,(char*)"\n\n");

	}

	return result;
}



char* extractPath(char** value) {
	
	char** split = stringSplitter(' ', value[0]);

	
		return split[1];
	
}

int contains(char* ref, char* cmp) {
	char* ptr = cmp;
	char* ptrRef = ref;
	
	while (*ptr != '\0') {
		if (*cmp == *ptrRef) {
			ptrRef++;
			if (*ptrRef == '\0') {
				return 1;
			}
		}
		else {
			ptrRef = ref;
		}
		ptr++;
	}
	return 0;
}

int main() {

	char recvbuf[DEFAULT_BUFLEN];
	WSADATA wsadata;
	initWSA();
	int iResult,iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	begin:
	iResult = WSAStartup(MAKEWORD(2,2),&wsadata);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return FAILURE;
	}
	Response resp = initResponse(GET_RESP);
	char* generatedAnswer = createAnswerFromStruct(resp,(char*)"index.html");
	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL,"112", &hints, &result);
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
	printf("Listening on: ");
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

	fprintf(stdout, "port %s\n",
		  servstr);

	SOCKET ClientSocket = INVALID_SOCKET;

	printf("Waiting for Client... \n");
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
			
			char** strings = stringSplitter('\n',recvbuf);
			printf("%s", recvbuf);
			if(checkGet(strings[0])) {
				char* path = extractPath(strings);
				if (contains(path, (char*)"css")) {
					resp.contentType = (char*)"Content-type: text/css;";
				}
				else {
					resp.contentType = (char*)"Content-type: text/html; charset=utf-8";
				}
				char* returnValue = createAnswerFromStruct(resp,path);
				iSendResult = send(ClientSocket,returnValue,strlen(returnValue), 0);

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
	closesocket(ClientSocket);
	goto begin;
	WSACleanup();

	return SUCCESS;
}