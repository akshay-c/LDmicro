#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"
//#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

static HWND TemporaryRadio_dpdt;
static HWND LatchedRadio_dpdt;
static HWND ClosedRadio_dpdt;
static HWND OpenRadio_dpdt;
static HWND NameTextbox_dpdt;
static HWND InNameTextbox_dpdt;
static HWND OutNameTextbox_dpdt;
HWND* DPDTDialog;

void DPDTStateChanged(SPDTStruct* SwitchData, void* ImageLocation);
void MakeSwitchControls()
{
	HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Action"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *DPDTDialog, NULL, NULL, NULL);
	FontNice(ActionGrouper);

	LatchedRadio_dpdt = CreateWindowEx(0, WC_BUTTON, ("Latched"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *DPDTDialog, NULL, NULL, NULL);
	FontNice(LatchedRadio_dpdt);

	TemporaryRadio_dpdt = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *DPDTDialog, NULL, NULL, NULL);
	FontNice(TemporaryRadio_dpdt);

	// SetOnlyRadio = CreateWindowEx(0, WC_BUTTON, _("(S) Set-Only"),
	//     WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
	//     16, 61, 100, 20, CoilDialog, NULL, Instance, NULL);
	// NiceFont(SetOnlyRadio);

	// ResetOnlyRadio = CreateWindowEx(0, WC_BUTTON, _("(R) Reset-Only"),
	//     WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
	//     16, 81, 105, 20, CoilDialog, NULL, Instance, NULL);
	// NiceFont(ResetOnlyRadio);

	HWND PositionGrouper = CreateWindowEx(0, WC_BUTTON, ("Position"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
		140, 3, 120, 65, *DPDTDialog, NULL, NULL, NULL);
	FontNice(PositionGrouper);

	OpenRadio_dpdt = CreateWindowEx(0, WC_BUTTON, ("Open"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		149, 21, 100, 20, *DPDTDialog, NULL, NULL, NULL);
	FontNice(OpenRadio_dpdt);

	ClosedRadio_dpdt = CreateWindowEx(0, WC_BUTTON, ("Closed"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
		149, 41, 100, 20, *DPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_dpdt);

	HWND textLabel = CreateWindowEx(0, WC_STATIC, ("Name:"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
		100, 80, 50, 21, *DPDTDialog, NULL, NULL, NULL);
	FontNice(textLabel);

	NameTextbox_dpdt = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
		WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
		155, 80, 155, 21, *DPDTDialog, NULL, NULL, NULL);
	FontFixed(NameTextbox_dpdt);

	/*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC,
	(LONG_PTR)MyNameProc);*/
}

void DPDTLoadState(DPDTStruct* Data)
{
	if (Data->latch_1)
	{
		Button_SetCheck(LatchedRadio_dpdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(TemporaryRadio_dpdt, BST_CHECKED);
	}
	if (Data->latch_2)
	{
		Button_SetCheck(LatchedRadio_dpdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(TemporaryRadio_dpdt, BST_CHECKED);
	}
	if (Data->latch_3)
	{
		Button_SetCheck(LatchedRadio_dpdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(TemporaryRadio_dpdt, BST_CHECKED);
	}
	if (Data->latch_3)
	{
		Button_SetCheck(LatchedRadio_dpdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(TemporaryRadio_dpdt, BST_CHECKED);
	}
	if (Data->init_pos)
	{
		Button_SetCheck(OpenRadio_dpdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(ClosedRadio_dpdt, BST_CHECKED);
	}
	//Edit_SetText(NameTextbox_spdt, Data->Name);
}

void SetDPDTIds(int* id, void* ComponentAddress)
{
	DPDTStruct *s = (DPDTStruct*)ComponentAddress;
	s->pinId[0] = *id++;
	s->pinId[1] = *id++;
	s->pinId[2] = *id++;
	s->pinId[3] = *id++;
	s->pinId[4] = *id++;
	s->pinId[5] = *id;
	/*char Debug[256];
	sprintf_s(Debug, "SetSwitchIds: \tAddr1: %p \tAddr2: %p \tPin0:%d \t Pin1:%d \tVolt0: %f Volt1: %f\n",
	ComponentAddress, s, s->PinId[0], s->PinId[1],VoltRequest(s->PinId[0], ComponentAddress),
	VoltRequest(s->PinId[0], ComponentAddress));
	OutputDebugString(Debug);
	char vx[4],vy[4];
	_itoa(s->PinId[0],vy,10);
	_itoa(s->PinId[1],vx,10);
	MessageBox(NULL,
	(vy), (vx), MB_OK | MB_ICONWARNING);*/
}