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
#include <math.h>
#include <vector>
#include <windows.h>
#include <GL/gl.h>

#pragma comment(lib, "OpenGL32.lib")
#pragma once

using namespace std;

#define LISTBOX_ITEM_HEIGHT 12
#define	xpWidgetClass_ListBox 10

class XPListBoxWidget															 
{
public:
	XPListBoxWidget();
	~XPListBoxWidget();
	XPWidgetID  XPCreateListBox(
		int                  inLeft,
		int                  inTop,
		int                  inRight,
		int                  inBottom,
		int                  inVisible,
		const char *         inDescriptor,
		XPWidgetID           inContainer);
private:
};
