#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "clientHeaders.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"user32.lib")


// defining controls
#define ID_SEARCH 7
#define ID_RESULT 8

#define ID_EDIT_DATE 0
#define ID_EDIT_EXTERNAL_ID 1
#define ID_EDIT_NUMBER 2
#define ID_EDIT_CONSTANT 3
#define ID_EDIT_DIGITS 4
#define ID_EDIT_DECIMALS 5

// main
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {

    MSG  msg;    
    WNDCLASSW wc = {0};
    wc.lpszClassName = L"Client";
    wc.hInstance     = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Client",
                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                  220, 220, 900, 800, 0, 0, hInstance, 0);  

    while (GetMessage(&msg, NULL, 0, 0)) {
    
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

//We catch evens here
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam) {

    static HWND hwndEdit_VAL, hwndEdit_EXTERNAL_ID, hwndEdit_NUMBER, hwndEdit_CONSTANT, hwndEdit_DIGITS, hwndEdit_DECIMALS, hwndEdit_RESULT;
    HWND hwndButton;
	
	static wchar_t *date = L"Enter date query";
	static wchar_t *ext_id = L"Enter external_id query";
	static wchar_t *number = L"Enter number query";
	static wchar_t *constant = L"Enter constant query";
	static wchar_t *digits = L"Enter digits query";
	static wchar_t *decimals = L"Enter decimals query";
	static wchar_t *res = L"Results";

    switch(msg) {

        case WM_CREATE: // on creation create all inputs and outputs
		
		
			CreateWindowW(L"Static", res, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                350, 20, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);

            hwndEdit_RESULT = CreateWindowW(L"Listbox" , NULL, 	
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                350, 40, 400, 700, hwnd, (HMENU) ID_RESULT,
                NULL, NULL);
		
			CreateWindowW(L"Static", date, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 280, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);

            hwndEdit_VAL = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                50, 300, 150, 20, hwnd, (HMENU) ID_EDIT_DATE,
                NULL, NULL);
				
			CreateWindowW(L"Static", ext_id, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 330, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);	
			
			hwndEdit_EXTERNAL_ID = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                50, 350, 150, 20, hwnd, (HMENU) ID_EDIT_EXTERNAL_ID,
                NULL, NULL);
				
			CreateWindowW(L"Static", number, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 380, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);	
			
			hwndEdit_NUMBER = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                50, 400, 150, 20, hwnd, (HMENU) ID_EDIT_NUMBER,
                NULL, NULL);
				
			CreateWindowW(L"Static", constant, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 430, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);	
			
			hwndEdit_CONSTANT = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                50, 450, 150, 20, hwnd, (HMENU) ID_EDIT_CONSTANT,
                NULL, NULL);
				
			CreateWindowW(L"Static", digits, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 480, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);	
			
			hwndEdit_DIGITS = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                50, 500, 150, 20, hwnd, (HMENU) ID_EDIT_DIGITS,
                NULL, NULL);
				
			CreateWindowW(L"Static", decimals, 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 530, 300, 230, 
                hwnd, (HMENU) 9, NULL, NULL);	
				
			hwndEdit_DECIMALS = CreateWindowW(L"Edit", NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                50, 550, 150, 20, hwnd, (HMENU) ID_EDIT_DECIMALS,
                NULL, NULL);	
				
			
            hwndButton = CreateWindowW(L"button", L"Search",
                WS_VISIBLE | WS_CHILD, 50, 600, 80, 25,
                hwnd, (HMENU) ID_SEARCH, NULL, NULL);

            break;

        case WM_COMMAND:	

            if (LOWORD(wParam) == ID_SEARCH) { // on command and search, build search query as comma seperated list
				
				char total [1024];
				for(int i = 0;i < 6;i++){
					int len = GetWindowTextLength(GetDlgItem(hwnd, i));
					if(len > 0){
						char * query = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemText(hwnd, i, query, len + 1);
						char * query_in_quotes = surroundWithQuotesPlusComma(query);
						char * appended = appendStrings(total,query_in_quotes);
						strcpy(total, appended);
						free(appended);
						free(query_in_quotes);
						
					}else{
						char * to_append = "'',";
						char * appended = appendStrings(total,to_append);
						strcpy(total, appended);
						free(appended);
					}
				}
				SendMessage(hwndEdit_RESULT, LB_RESETCONTENT, 0, 0); // reset listbox, new search
				Search(total, hwndEdit_RESULT);
            }

            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


//connets to server, sends query, gets response, writes it to listbox
void Search(char *str, HWND hwnd){
	
	int port = 9090;
	char * server_address = "127.0.0.1";                 
    char *to_send = str;  
	
	SOCKET sock; 
    struct sockaddr_in echoServAddr;             
    char server_response[1024] = "";     
    int response_length;         
    WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) 
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf(" socket failed with error code : %d" , WSAGetLastError());
		exit(1);
	}
	
	
    memset(&echoServAddr, 0, sizeof(echoServAddr));     
    echoServAddr.sin_family      = AF_INET;             
    echoServAddr.sin_addr.s_addr = inet_addr(server_address);   
    echoServAddr.sin_port        = htons(port); 
	

	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
		printf(" connect failed with error code : %d" , WSAGetLastError());
		exit(1);
	}else{
		printf("Connected to server ...\n");
	}
	
	if (send(sock, to_send, strlen(to_send) + 1, 0) != (strlen(to_send) + 1)){
		printf(" send failed with error code : %d" , WSAGetLastError());
		exit(1);
	}else{
		printf("Sent query to server successfully ...\n");
	}
	do {

        response_length = recv(sock, server_response, 1024, 0);
        if ( response_length > 0 ){
			SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)server_response);
			
		}else if ( response_length == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
		
		
    } while( response_length > 0 );
	
	
	closesocket(sock);
	
	
}

//appends two strings, needs to be freed afterwards.
char * appendStrings(char *str1, char *str2){
	
	char * new_str ;
		if((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL){
		new_str[0] = '\0';   // ensures the memory is an empty string
		strcat(new_str,str1);
		strcat(new_str,str2);
		} else {
			printf("malloc failed! when trying to append strings %s and &s\n", str1, str2);
			exit(1);
		}
	return new_str;
}
//surrounds string with quotes, needed for SQL
char * surroundWithQuotesPlusComma(char *str){
	
	char * quote = "'";
	char * comma = ",";
	char * app = appendStrings(quote, str);
	char * app1 = appendStrings(app, quote);
	char * app2 = appendStrings(app1, comma);
	
	free(app1);
	free(app);
	
	return app2;
	
}