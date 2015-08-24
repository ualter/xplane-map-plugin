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
#include "SocketClient.h"
#include <math.h>
#include <vector>
#include <windows.h>
#include <GL/gl.h>
#include "XPListBoxWidget.h"

#pragma comment(lib, "OpenGL32.lib")


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
static XPWidgetID wMainWindow, wSubWindow, wBtnReload, wBtnCancel, wBtnSendInfo;
static XPWidgetID wTextServerAddress, wTextServerPort, wTextDataRefs, wDataRefListBox;
static XPLMMenuID id;
std::string logFile = "x-plane-map.log";
static char szListBoxText[4096];

float CallBackXPlane(float  inElapsedSinceLastCall,
	float  inElapsedTimeSinceLastFlightLoop,
	int    inCounter,
	void * inRefcon);

static int widgetWidgetHandler(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2);

// ListBox Implementation
#define LISTBOX_ITEM_HEIGHT 12
struct	XPListBoxData_t {
	// Per item:
	std::vector<std::string>	Items;		// The name of the item
	std::vector<int>			Lefts;		// The rectangle of the item, relative to the top left corner of the listbox/
	std::vector<int>			Rights;
};

static XPListBoxData_t *pListBoxData;

#define	xpWidgetClass_ListBox					10

enum {
	// This is the item number of the current item, starting at 0.
	xpProperty_ListBoxCurrentItem = 1900,
	// This will add an item to the list box at the end.
	xpProperty_ListBoxAddItem = 1901,
	// This will clear the list box and then add the items.
	xpProperty_ListBoxAddItemsWithClear = 1902,
	// This will clear the list box.
	xpProperty_ListBoxClear = 1903,
	// This will insert an item into the list box at the index.
	xpProperty_ListBoxInsertItem = 1904,
	// This will delete an item from the list box at the index.
	xpProperty_ListBoxDeleteItem = 1905,
	// This stores the pointer to the listbox data.
	xpProperty_ListBoxData = 1906,
	// This stores the max Listbox Items.
	xpProperty_ListBoxMaxListBoxItems = 1907,
	// This stores the highlight state.
	xpProperty_ListBoxHighlighted = 1908,
	// This stores the scrollbar Min.
	xpProperty_ListBoxScrollBarMin = 1909,
	// This stores the scrollbar Max.
	xpProperty_ListBoxScrollBarMax = 1910,
	// This stores the scrollbar SliderPosition.
	xpProperty_ListBoxScrollBarSliderPosition = 1911,
	// This stores the scrollbar ScrollBarPageAmount.
	xpProperty_ListBoxScrollBarPageAmount = 1912
};

enum {
	// This message is sent when an item is picked.
	// param 1 is the widget that was picked, param 2
	// is the item number.
	xpMessage_ListBoxItemSelected = 1900
};

static XPWidgetID           XPCreateListBox(
	int                  inLeft,
	int                  inTop,
	int                  inRight,
	int                  inBottom,
	int                  inVisible,
	const char *         inDescriptor,
	XPWidgetID           inContainer);

static int	XPListBoxProc(
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

	// Preload Listbox
	szListBoxText[0] = '\0';
	strcat(szListBoxText, "1234567890123456789012345678901234567890123456789012345678901234567890");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 2");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 3");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 4");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 5");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 6");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 7");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 8");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 9");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 10");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 11");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 12");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 13");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 14");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 15");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 16");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 17");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 18");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 19");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 20");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 21");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 22");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 23");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 24");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 25");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 26");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 27");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 28");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 29");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 30");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 31");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 32");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 33");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 34");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 35");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 36");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 37");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 38");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 39");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 40");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 41");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 42");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 43");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 44");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 45");
	strcat(szListBoxText, ";");
	strcat(szListBoxText, "Item 46");
	strcat(szListBoxText, ";");

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

	int lineY = 1030;
	int lineX = 150;
	int size = 80;

	// Server Address
	XPCreateWidget(lineX, lineY - 39, lineX + size, lineY - 59, 1, "Server Address:", 0, wMainWindow, xpWidgetClass_Caption);
	lineX += 88;
	wTextServerAddress = XPCreateWidget(lineX, lineY - 40, lineX + 100, lineY - 60, 1, "192.168.0.179", 0, wMainWindow, xpWidgetClass_TextField);
	XPSetWidgetProperty(wTextServerAddress, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(wTextServerAddress, xpProperty_MaxCharacters, 15);
	// Port
	lineX += 130;
	XPCreateWidget(lineX, lineY - 39, lineX + size, lineY - 59, 1, "Port:", 0, wMainWindow, xpWidgetClass_Caption);
	lineX += 32;
	wTextServerPort = XPCreateWidget(lineX, lineY - 40, lineX + 50, lineY - 60, 1, "5583", 0, wMainWindow, xpWidgetClass_TextField);
	XPSetWidgetProperty(wTextServerPort, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(wTextServerPort, xpProperty_MaxCharacters, 8);

	// DataRefs
	lineY = 1010;
	lineX = 150;
	XPCreateWidget(lineX, lineY - 40, lineX + size, lineY - 62, 1, "DataRefs:", 0, wMainWindow, xpWidgetClass_Caption);
	lineY = 990;
	//wDataRefListBox = XPCreateListBox(lineX + 2, lineY - 40, lineX + 480, lineY - 240, 1, szListBoxText, wMainWindow);

	XPListBoxWidget xpListBox = XPListBoxWidget();
	wDataRefListBox = xpListBox.XPCreateListBox(lineX + 2, lineY - 40, lineX + 480, lineY - 240, 1, szListBoxText, wMainWindow);
	
	// Buttons
	lineY = 770;
	lineX = 230;
	// Button Exit
	wBtnCancel = XPCreateWidget(lineX, lineY - 40, lineX + size, lineY - 60, 1, "Exit", 0, wMainWindow, xpWidgetClass_Button);
	XPSetWidgetProperty(wBtnCancel, xpProperty_ButtonType, xpPushButton);

	// Button Send
	lineX += 100;
	wBtnSendInfo = XPCreateWidget(lineX, lineY - 40, lineX + size, lineY - 60, 1, "Send", 0, wMainWindow, xpWidgetClass_Button);
	XPSetWidgetProperty(wBtnSendInfo, xpProperty_ButtonType, xpPushButton);

	// Button Reload
	lineX += 100;
	wBtnReload = XPCreateWidget(lineX, lineY - 40, lineX + size, lineY - 60, 1, "Reload", 0, wMainWindow, xpWidgetClass_Button);
	XPSetWidgetProperty(wBtnReload, xpProperty_ButtonType, xpPushButton); 

	XPAddWidgetCallback(wMainWindow, widgetWidgetHandler);
}

static XPWidgetID           XPCreateListBox(
	int                  inLeft,
	int                  inTop,
	int                  inRight,
	int                  inBottom,
	int                  inVisible,
	const char *         inDescriptor,
	XPWidgetID           inContainer)
{
	return XPCreateCustomWidget(
		inLeft,
		inTop,
		inRight,
		inBottom,
		inVisible,
		inDescriptor,
		0,
		inContainer,
		XPListBoxProc);
}


void sendInfo() 
{
	std::ifstream fin(logFile);
	std::ofstream fileIniWriter;
	fileIniWriter.open(logFile);
	fileIniWriter << "Aqui\n";

	SocketClient so = SocketClient("192.168.0.179");
	so.sendTo("Hello from X-Plane 11");
	so.sendTo("Hello from X-Plane 22");
	so.~SocketClient();

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
			XPLMReloadPlugins();
			return 1;
		}
		if (inParam1 == (intptr_t)wBtnSendInfo)
		{
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

// ListBox Methods

// Enums for x-plane native colors. 
enum {

	xpColor_MenuDarkTinge = 0,
	xpColor_MenuBkgnd,
	xpColor_MenuHilite,
	xpColor_MenuLiteTinge,
	xpColor_MenuText,
	xpColor_MenuTextDisabled,
	xpColor_SubTitleText,
	xpColor_TabFront,
	xpColor_TabBack,
	xpColor_CaptionText,
	xpColor_ListText,
	xpColor_GlassText,
	xpColor_Count
};

// Enums for the datarefs we get them from.
static const char *	kXPlaneColorNames[] = {
	"sim/graphics/colors/menu_dark_rgb",
	"sim/graphics/colors/menu_bkgnd_rgb",
	"sim/graphics/colors/menu_hilite_rgb",
	"sim/graphics/colors/menu_lite_rgb",
	"sim/graphics/colors/menu_text_rgb",
	"sim/graphics/colors/menu_text_disabled_rgb",
	"sim/graphics/colors/subtitle_text_rgb",
	"sim/graphics/colors/tab_front_rgb",
	"sim/graphics/colors/tab_back_rgb",
	"sim/graphics/colors/caption_text_rgb",
	"sim/graphics/colors/list_text_rgb",
	"sim/graphics/colors/glass_text_rgb"
};
// Those datarefs are only XP7; if we can't find one,
// fall back to this table of X-Plane 6 colors.
static const float	kBackupColors[xpColor_Count][3] =
{
	{ (const float)(33.0 / 256.0), (const float)(41.0 / 256.0), (const float)(44.0 / 256.0) },
	{ (const float)(53.0 / 256.0), (const float)(64.0 / 256.0), (const float)(68.0 / 256.0) },
	{ (const float)(65.0 / 256.0), (const float)(83.0 / 256.0), (const float)(89.0 / 256.0) },
	{ (const float)(65.0 / 256.0), (const float)(83.0 / 256.0), (const float)(89.0 / 256.0) },
	{ (const float)0.8, (const float)0.8, (const float)0.8 },
	{ (const float)0.4, (const float)0.4, (const float)0.4 }
};

// This array contains the resolved datarefs
static XPLMDataRef	gColorRefs[xpColor_Count];

// Current alpha levels to blit at.
static float		gAlphaLevel = 1.0;


// Just remember alpha levels for later.
// This routine sets up a color from the above table.  Pass
// in a float[3] to get the color; pass in NULL to have the
// OpenGL color be set immediately.
static void	SetupAmbientColor(int inColorID, float * outColor)
{
	// If we're running the first time, resolve all of our datarefs just once.
	static	bool	firstTime = true;
	if (firstTime)
	{
		firstTime = false;
		for (int n = 0; n <xpColor_Count; ++n)
		{
			gColorRefs[n] = XPLMFindDataRef(kXPlaneColorNames[n]);
		}
	}

	// If being asked to set the color immediately, allocate some storage.
	float	theColor[4];
	float * target = outColor ? outColor : theColor;

	// If we have a dataref, just fetch the color from the ref.
	if (gColorRefs[inColorID])
		XPLMGetDatavf(gColorRefs[inColorID], target, 0, 3);
	else {

		// If we didn't have a dataref, fetch the ambient cabin lighting,
		// since XP6 dims the UI with night.
		static	XPLMDataRef	ambient_r = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_r");
		static	XPLMDataRef	ambient_g = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_g");
		static	XPLMDataRef	ambient_b = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_b");

		// Use a backup color but dim it.
		target[0] = kBackupColors[inColorID][0] * XPLMGetDataf(ambient_r);
		target[1] = kBackupColors[inColorID][1] * XPLMGetDataf(ambient_g);
		target[2] = kBackupColors[inColorID][2] * XPLMGetDataf(ambient_b);
	}

	// If the user passed NULL, set the color now using the alpha level.
	if (!outColor)
	{
		theColor[3] = gAlphaLevel;
		glColor4fv(theColor);
	}
}
static void	SetAlphaLevels(float inAlphaLevel)
{
	gAlphaLevel = inAlphaLevel;
}
static void XPListBoxAddItem(XPListBoxData_t *pListBoxData, char *pBuffer, int Width)
{
	std::string	Item(pBuffer);

	pListBoxData->Items.push_back(Item);
	pListBoxData->Lefts.push_back(0);
	pListBoxData->Rights.push_back(Width);
}

static void XPListBoxClear(XPListBoxData_t *pListBoxData)
{
	pListBoxData->Items.clear();
	pListBoxData->Lefts.clear();
	pListBoxData->Rights.clear();
}

// This routine finds the item that is in a given point, or returns -1 if there is none.
// It simply trolls through all the items.
static void XPListBoxFillWithData(XPListBoxData_t *pListBoxData, const char *inItems, int Width)
{
	std::string	Items(inItems);
	while (!Items.empty())
	{
		std::string::size_type split = Items.find(';');
		if (split == Items.npos)
		{
			split = Items.size();
		}

		std::string	Item = Items.substr(0, split);

		pListBoxData->Items.push_back(Item);
		pListBoxData->Lefts.push_back(0);
		pListBoxData->Rights.push_back(Width);

		if (Item.size() == Items.size())
			break;
		else
			Items = Items.substr(split + 1);
	}
}
static int XPListBoxGetItemNumber(XPListBoxData_t * pListBoxData, int inX, int inY)
{
	for (unsigned int n = 0; n < pListBoxData->Items.size(); ++n)
	{
		if ((inX >= pListBoxData->Lefts[n]) && (inX < pListBoxData->Rights[n]) &&
			(inY >= (n * LISTBOX_ITEM_HEIGHT)) && (inY < ((n * LISTBOX_ITEM_HEIGHT) + LISTBOX_ITEM_HEIGHT)))
		{
			return n;
		}
	}
	return -1;
}

static void XPListBoxInsertItem(XPListBoxData_t *pListBoxData, char *pBuffer, int Width, int CurrentItem)
{
	std::string	Item(pBuffer);

	pListBoxData->Items.insert(pListBoxData->Items.begin() + CurrentItem, Item);
	pListBoxData->Lefts.insert(pListBoxData->Lefts.begin() + CurrentItem, 0);
	pListBoxData->Rights.insert(pListBoxData->Rights.begin() + CurrentItem, Width);
}

static void XPListBoxDeleteItem(XPListBoxData_t *pListBoxData, int CurrentItem)
{
	pListBoxData->Items.erase(pListBoxData->Items.begin() + CurrentItem);
	pListBoxData->Lefts.erase(pListBoxData->Lefts.begin() + CurrentItem);
	pListBoxData->Rights.erase(pListBoxData->Rights.begin() + CurrentItem);
}
static int		XPListBoxProc(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2)
{
	static int ScrollBarSlop;

	// Select if we're in the background.
	if (XPUSelectIfNeeded(inMessage, inWidget, inParam1, inParam2, 1/*eat*/))	return 1;

	int Left, Top, Right, Bottom, x, y, ListBoxDataOffset, ListBoxIndex;
	char Buffer[4096];

	int IsVertical, DownBtnSize, DownPageSize, ThumbSize, UpPageSize, UpBtnSize;
	bool UpBtnSelected, DownBtnSelected, ThumbSelected, UpPageSelected, DownPageSelected;

	XPGetWidgetGeometry(inWidget, &Left, &Top, &Right, &Bottom);

	int	SliderPosition = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, NULL);
	int	Min = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMin, NULL);
	int	Max = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, NULL);
	int	ScrollBarPageAmount = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarPageAmount, NULL);
	int	CurrentItem = XPGetWidgetProperty(inWidget, xpProperty_ListBoxCurrentItem, NULL);
	int	MaxListBoxItems = XPGetWidgetProperty(inWidget, xpProperty_ListBoxMaxListBoxItems, NULL);
	int	Highlighted = XPGetWidgetProperty(inWidget, xpProperty_ListBoxHighlighted, NULL);
	XPListBoxData_t	*pListBoxData = (XPListBoxData_t*)XPGetWidgetProperty(inWidget, xpProperty_ListBoxData, NULL);

	switch (inMessage)
	{
	case xpMsg_Create:
		// Allocate mem for the structure.
		pListBoxData = new XPListBoxData_t;
		XPGetWidgetDescriptor(inWidget, Buffer, sizeof(Buffer));
		XPListBoxFillWithData(pListBoxData, Buffer, (Right - Left - 20));
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxData, (intptr_t)pListBoxData);
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxCurrentItem, 0);
		Min = 0;
		Max = pListBoxData->Items.size();
		ScrollBarSlop = 0;
		Highlighted = false;
		SliderPosition = Max;
		MaxListBoxItems = (Top - Bottom) / LISTBOX_ITEM_HEIGHT;
		ScrollBarPageAmount = MaxListBoxItems;
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMin, Min);
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, Max);
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarPageAmount, ScrollBarPageAmount);
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxMaxListBoxItems, MaxListBoxItems);
		XPSetWidgetProperty(inWidget, xpProperty_ListBoxHighlighted, Highlighted);
		return 1;

	case xpMsg_DescriptorChanged:
		return 1;

	case xpMsg_PropertyChanged:
		if (XPGetWidgetProperty(inWidget, xpProperty_ListBoxAddItem, NULL))
		{
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxAddItem, 0);
			XPGetWidgetDescriptor(inWidget, Buffer, sizeof(Buffer));
			XPListBoxAddItem(pListBoxData, Buffer, (Right - Left - 20));
			Max = pListBoxData->Items.size();
			SliderPosition = Max;
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, Max);
		}

		if (XPGetWidgetProperty(inWidget, xpProperty_ListBoxAddItemsWithClear, NULL))
		{
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxAddItemsWithClear, 0);
			XPGetWidgetDescriptor(inWidget, Buffer, sizeof(Buffer));
			XPListBoxClear(pListBoxData);
			XPListBoxFillWithData(pListBoxData, Buffer, (Right - Left - 20));
			Max = pListBoxData->Items.size();
			SliderPosition = Max;
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, Max);
		}

		if (XPGetWidgetProperty(inWidget, xpProperty_ListBoxClear, NULL))
		{
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxClear, 0);
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxCurrentItem, 0);
			XPListBoxClear(pListBoxData);
			Max = pListBoxData->Items.size();
			SliderPosition = Max;
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, Max);
		}

		if (XPGetWidgetProperty(inWidget, xpProperty_ListBoxInsertItem, NULL))
		{
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxInsertItem, 0);
			XPGetWidgetDescriptor(inWidget, Buffer, sizeof(Buffer));
			XPListBoxInsertItem(pListBoxData, Buffer, (Right - Left - 20), CurrentItem);
		}

		if (XPGetWidgetProperty(inWidget, xpProperty_ListBoxDeleteItem, NULL))
		{
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxDeleteItem, 0);
			if ((pListBoxData->Items.size() > 0) && (pListBoxData->Items.size() > CurrentItem))
				XPListBoxDeleteItem(pListBoxData, CurrentItem);
		}
		return 1;

	case xpMsg_Draw:
	{
		int	x, y;
		XPLMGetMouseLocation(&x, &y);

		XPDrawWindow(Left, Bottom, Right - 20, Top, xpWindow_ListView);
		XPDrawTrack(Right - 20, Bottom, Right, Top, Min, Max, SliderPosition, xpTrack_ScrollBar, Highlighted);

		XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
		XPLMBindTexture2d(XPLMGetTexture(xplm_Tex_GeneralInterface), 0);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		unsigned int ItemNumber;
		XPLMSetGraphicsState(0, 0, 0, 0, 0, 0, 0);

		// Now draw each item.
		ListBoxIndex = Max - SliderPosition;
		ItemNumber = 0;
		while (ItemNumber < MaxListBoxItems)
		{
			if (ListBoxIndex < pListBoxData->Items.size())
			{
				// Calculate the item rect in global coordinates.
				int ItemTop = Top - (ItemNumber * LISTBOX_ITEM_HEIGHT);
				int ItemBottom = Top - ((ItemNumber * LISTBOX_ITEM_HEIGHT) + LISTBOX_ITEM_HEIGHT);

				// If we are hilited, draw the hilite bkgnd.
				if (CurrentItem == ListBoxIndex)
				{
					SetAlphaLevels(0.25);
					XPLMSetGraphicsState(0, 0, 0, 0, 1, 0, 0);
					SetupAmbientColor(xpColor_MenuHilite, NULL);
					SetAlphaLevels(1.0);
					glBegin(GL_QUADS);
					glVertex2i(Left, ItemTop);
					glVertex2i(Right - 20, ItemTop);
					glVertex2i(Right - 20, ItemBottom);
					glVertex2i(Left, ItemBottom);
					glEnd();
				}

				float	text[3];
				SetupAmbientColor(xpColor_ListText, text);

				char	Buffer[512];
				int		FontWidth, FontHeight;
				int		ListBoxWidth = (Right - 20) - Left;
				strcpy(Buffer, pListBoxData->Items[ListBoxIndex++].c_str());
				XPLMGetFontDimensions(xplmFont_Basic, &FontWidth, &FontHeight, NULL);
				int		MaxChars = ListBoxWidth / FontWidth;
				Buffer[MaxChars] = 0;

				XPLMDrawString(text,
					Left, ItemBottom + 2,
					const_cast<char *>(Buffer), NULL, xplmFont_Basic);
			}
			ItemNumber++;
		}
	}
		return 1;

	case xpMsg_MouseUp:
		if (IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, Top, Right, Bottom))
		{
			Highlighted = false;
			XPSetWidgetProperty(inWidget, xpProperty_ListBoxHighlighted, Highlighted);
		}

		if (IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Left, Top, Right - 20, Bottom))
		{
			if (pListBoxData->Items.size() > 0)
			{
				if (CurrentItem != -1)
					XPSetWidgetDescriptor(inWidget, pListBoxData->Items[CurrentItem].c_str());
				else
					XPSetWidgetDescriptor(inWidget, "");
				XPSendMessageToWidget(inWidget, xpMessage_ListBoxItemSelected, xpMode_UpChain, (intptr_t)inWidget, (intptr_t)CurrentItem);
			}
		}
		return 1;

	case xpMsg_MouseDown:
		if (IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Left, Top, Right - 20, Bottom))
		{
			if (pListBoxData->Items.size() > 0)
			{
				XPLMGetMouseLocation(&x, &y);
				ListBoxDataOffset = XPListBoxGetItemNumber(pListBoxData, x - Left, Top - y);
				if (ListBoxDataOffset != -1)
				{
					ListBoxDataOffset += (Max - SliderPosition);
					if (ListBoxDataOffset < pListBoxData->Items.size())
						XPSetWidgetProperty(inWidget, xpProperty_ListBoxCurrentItem, ListBoxDataOffset);
				}
			}
		}

		if (IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, Top, Right, Bottom))
		{
			XPGetTrackMetrics(Right - 20, Bottom, Right, Top, Min, Max, SliderPosition, xpTrack_ScrollBar, &IsVertical, &DownBtnSize, &DownPageSize, &ThumbSize, &UpPageSize, &UpBtnSize);
			int	Min = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMin, NULL);
			int	Max = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, NULL);
			if (IsVertical)
			{
				UpBtnSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, Top, Right, Top - UpBtnSize);
				DownBtnSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, Bottom + DownBtnSize, Right, Bottom);
				UpPageSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, (Top - UpBtnSize), Right, (Bottom + DownBtnSize + DownPageSize + ThumbSize));
				DownPageSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, (Top - UpBtnSize - UpPageSize - ThumbSize), Right, (Bottom + DownBtnSize));
				ThumbSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, (Top - UpBtnSize - UpPageSize), Right, (Bottom + DownBtnSize + DownPageSize));
			}
			else
			{
				DownBtnSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, Top, Right - 20 + UpBtnSize, Bottom);
				UpBtnSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20 - DownBtnSize, Top, Right, Bottom);
				DownPageSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20 + DownBtnSize, Top, Right - UpBtnSize - UpPageSize - ThumbSize, Bottom);
				UpPageSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20 + DownBtnSize + DownPageSize + ThumbSize, Top, Right - UpBtnSize, Bottom);
				ThumbSelected = IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20 + DownBtnSize + DownPageSize, Top, Right - UpBtnSize - UpPageSize, Bottom);
			}

			if (UpPageSelected)
			{
				SliderPosition += ScrollBarPageAmount;
				if (SliderPosition > Max)
					SliderPosition = Max;
				XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			}
			else if (DownPageSelected)
			{
				SliderPosition -= ScrollBarPageAmount;
				if (SliderPosition < Min)
					SliderPosition = Min;
				XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			}
			else if (UpBtnSelected)
			{
				SliderPosition++;
				if (SliderPosition > Max)
					SliderPosition = Max;
				XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			}
			else if (DownBtnSelected)
			{
				SliderPosition--;
				if (SliderPosition < Min)
					SliderPosition = Min;
				XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			}
			else if (ThumbSelected)
			{
				if (IsVertical)
					ScrollBarSlop = Bottom + DownBtnSize + DownPageSize + (ThumbSize / 2) - MOUSE_Y(inParam1);
				else
					ScrollBarSlop = Right - 20 + DownBtnSize + DownPageSize + (ThumbSize / 2) - MOUSE_X(inParam1);
				Highlighted = true;
				XPSetWidgetProperty(inWidget, xpProperty_ListBoxHighlighted, Highlighted);

			}
			else
			{
				Highlighted = false;
				XPSetWidgetProperty(inWidget, xpProperty_ListBoxHighlighted, Highlighted);
			}
		}
		return 1;

	case xpMsg_MouseDrag:
		if (IN_RECT(MOUSE_X(inParam1), MOUSE_Y(inParam1), Right - 20, Top, Right, Bottom))
		{
			XPGetTrackMetrics(Right - 20, Bottom, Right, Top, Min, Max, SliderPosition, xpTrack_ScrollBar, &IsVertical, &DownBtnSize, &DownPageSize, &ThumbSize, &UpPageSize, &UpBtnSize);
			int	Min = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMin, NULL);
			int	Max = XPGetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarMax, NULL);

			ThumbSelected = Highlighted;

			if (ThumbSelected)
			{
				if (inParam1 != 0)
				{
					if (IsVertical)
					{
						y = MOUSE_Y(inParam1) + ScrollBarSlop;
						SliderPosition = round((float)((float)(y - (Bottom + DownBtnSize + ThumbSize / 2)) /
							(float)((Top - UpBtnSize - ThumbSize / 2) - (Bottom + DownBtnSize + ThumbSize / 2))) * Max);
					}
					else
					{
						x = MOUSE_X(inParam1) + ScrollBarSlop;
						SliderPosition = round((float)((float)(x - (Right - 20 + DownBtnSize + ThumbSize / 2)) / (float)((Right - UpBtnSize - ThumbSize / 2) - (Right - 20 + DownBtnSize + ThumbSize / 2))) * Max);
					}

				}
				else
					SliderPosition = 0;

				if (SliderPosition < Min)
					SliderPosition = Min;
				if (SliderPosition > Max)
					SliderPosition = Max;

				XPSetWidgetProperty(inWidget, xpProperty_ListBoxScrollBarSliderPosition, SliderPosition);
			}
		}
		return 1;

	default:
		return 0;
	}
}


