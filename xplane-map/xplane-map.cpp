#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPLMCamera.h"
#include "XPUIGraphics.h"
#include "XPWidgetUtils.h"
#include "XPLMPlugin.h"
#include "XPLMNavigation.h"
#include "XPLMDataAccess.h"


#ifdef __cplusplus
#  if !defined(__MINGW32__) && !defined(_MSC_VER)
#    define NULL __null
#  else
#    define NULL 0
#  endif
#else
#  define NULL ((void*)0)
#endif

#define CHECKINTERVAL 1.5 //seconds between checks	

static int MenuItem1;
static XPWidgetID wMainWindow, wSubWindow, wBtnReload, wBtnCancel;
static XPLMMenuID id;
std::string logFile = "x-plane-map.log";

float CallBackXPlane(float  inElapsedSinceLastCall,
	float  inElapsedTimeSinceLastFlightLoop,
	int    inCounter,
	void * inRefcon);

static int widgetWidgetHandler(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2);

static void XPLaneMapMenuHandler(void *, void *);

PLUGIN_API int XPluginStart(
	char *		outName,
	char *		outSig,
	char *		outDesc)
{
	strcpy(outName, "XPlane-Map");
	strcpy(outSig, "br.sp.ualter.junior.XPlaneMap");
	strcpy(outDesc, "Plugin X-Plane");

	int item;
	item = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XPlane Map", NULL, 1);
	id = XPLMCreateMenu("XPlane Map", XPLMFindPluginsMenu(), item, XPLaneMapMenuHandler, NULL);
	XPLMAppendMenuItem(id, "Setup", (void *)"Setup", 1);

	MenuItem1 = 0;


	return 1;
}

void CreateWidgetWindow() 
{
	int x = 100;
	int y = 1050;
	int w = 820;
	int h = 455;

	int x2 = x + w;
	int y2 = y - h;

	wMainWindow = XPCreateWidget(x, y, x2, y2, 1, "XPlane Map / v1.0", 1, NULL, xpWidgetClass_MainWindow);
	XPSetWidgetProperty(wMainWindow, xpProperty_MainWindowHasCloseBoxes, 1);

	// Window
	wSubWindow = XPCreateWidget(x + 30, y - 40, x2 - 30, y2 + 30, 1, "", 0, wMainWindow, xpWidgetClass_SubWindow);
	XPSetWidgetProperty(wSubWindow, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

	int leftX = x;
	int topY = y;
	int rightX = x;
	int bottomY = y;

	int topMargin = 70;
	int leftMargin = 40;
	int widthCaption = 30;
	int gapCaptionField = 6;
	int heightFields = 22;

	int tmpX = 0;
	int tmpY = 0;

	// Button Exit
	leftX = x + (tmpX + 100);
	bottomY = topY - heightFields - 5;
	wBtnCancel = XPCreateWidget(leftX, topY, leftX + 60, bottomY, 1, "Exit", 0, wMainWindow, xpWidgetClass_Button);
	XPSetWidgetProperty(wBtnCancel, xpProperty_ButtonType, xpPushButton);

	// Button Reload
	leftX += 100;
	bottomY = topY - heightFields;
	wBtnReload = XPCreateWidget(leftX, topY - 5, leftX + 40, bottomY, 1, "Reload", 0, wMainWindow, xpWidgetClass_Button);
	XPSetWidgetProperty(wBtnReload, xpProperty_ButtonType, xpPushButton);

	XPAddWidgetCallback(wMainWindow, widgetWidgetHandler);
}

#define SERVER "127.0.0.1"
#define BUFLEN  512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

void sendInfo() 
{
	std::ifstream fin(logFile);
	std::ofstream fileIniWriter;
	fileIniWriter.open(logFile);
	fileIniWriter << "Aqui\n";

	SOCKET s;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	fileIniWriter << "\nInitialising Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fileIniWriter << "Failed. Error Code :" + WSAGetLastError();
	}
	fileIniWriter << "\nInitialised.\n";

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		fileIniWriter << "socket() failed with error code :"  + WSAGetLastError();
	}
	fileIniWriter << "Socket created.\n";

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	//si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
	IN_ADDR addr;
	si_other.sin_addr.S_un.S_addr = inet_pton(AF_INET, SERVER, &addr);

	if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
	{
		fileIniWriter << "sendto() failed with error code :" + WSAGetLastError();
	}
	fileIniWriter << "\nmessage sent\n";
	closesocket(s);
	WSACleanup();

	fileIniWriter << "\nOK!\n";
	fileIniWriter.close();

}

void XPLaneMapMenuHandler(void * mRef, void * iRef)
{
	if (!strcmp((char *)iRef, "Setup"))
	{
		if (MenuItem1 == 0)
		{
			CreateWidgetWindow();
			MenuItem1 = 1;
			XPLMRegisterFlightLoopCallback(CallBackXPlane, 1.0, NULL);
		}
	}
}

float CallBackXPlane(float  inElapsedSinceLastCall,
	float  inElapsedTimeSinceLastFlightLoop,
	int    inCounter,
	void * inRefcon)
{
	return CHECKINTERVAL;
}

int widgetWidgetHandler(XPWidgetMessage inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2)
{

	if (inMessage == xpMessage_CloseButtonPushed)
	{
		if (MenuItem1 == 1)
		{
			XPHideWidget(wMainWindow);
			MenuItem1 = 0;
		}
		return 1;
	}

	if (inMessage == xpMsg_PushButtonPressed)
	{
		if (inParam1 == (intptr_t)wBtnReload)
		{
			//XPLMReloadPlugins();
			sendInfo();
			return 1;
		}
		if (inParam1 == (intptr_t)wBtnCancel)
		{
			XPHideWidget(wMainWindow);
			MenuItem1 = 0;
			return 1;
		}
	}
	return 0;
}