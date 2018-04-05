#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"
#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

// Declaring the required components in UI.
static HWND TemporaryRadio_dpst;
static HWND LatchedRadio_dpst;
static HWND ClosedRadio_dpst;
static HWND OpenRadio_dpst;
static HWND OutNameTextbox_dpst;
HWND* DPSTDialog;

// UI for the DPST switch.
void MakeDPSTControls()
{
	HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Action"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *DPSTDialog, NULL, NULL, NULL);
	FontNice(ActionGrouper);

	LatchedRadio_dpst = CreateWindowEx(0, WC_BUTTON, ("Latched"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *DPSTDialog, NULL, NULL, NULL);
	FontNice(LatchedRadio_dpst);

	TemporaryRadio_dpst = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *DPSTDialog, NULL, NULL, NULL);
	FontNice(TemporaryRadio_dpst);

	HWND PositionGrouper = CreateWindowEx(0, WC_BUTTON, ("Position"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
		140, 3, 120, 65, *DPSTDialog, NULL, NULL, NULL);
	FontNice(PositionGrouper);

	ClosedRadio_dpst = CreateWindowEx(0, WC_BUTTON, ("Closed"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		149, 21, 100, 20, *DPSTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_dpst);

	OpenRadio_dpst = CreateWindowEx(0, WC_BUTTON, ("Open"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
		149, 41, 100, 20, *DPSTDialog, NULL, NULL, NULL);
	FontNice(OpenRadio_dpst);
}

void DPSTLoadState(DPSTStruct* Data)
{
	if (Data->latched)
	{
		Button_SetCheck(LatchedRadio_dpst, BST_CHECKED);
	}
	/*else
	{
		Button_SetCheck(TemporaryRadio_dpst, BST_CHECKED);
	}*/
	if (Data->init_pos)
	{
		Button_SetCheck(ClosedRadio_dpst, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(OpenRadio_dpst, BST_CHECKED);
	}
}

BOOL SaveDPSTDialog(DPSTStruct* Data)
{
	BOOL latched, init_pos;
	if (Button_GetState(LatchedRadio_dpst) == BST_CHECKED)
	{
		latched = TRUE;
	}
	/*else if (Button_GetState(TemporaryRadio_dpst) == BST_CHECKED)
	{
		latched = FALSE;
	}*/
	else
	{
		MessageBox(*DPSTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if (Button_GetState(ClosedRadio_dpst) == BST_CHECKED)
	{
		init_pos = TRUE;
	}
	else if (Button_GetState(OpenRadio_dpst) == BST_CHECKED)
	{
		init_pos = FALSE;
	}
	else
	{
		MessageBox(*DPSTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	Data->latched = latched;
	Data->init_pos = init_pos;
	if (init_pos)
	{
		Data->image = DPST_switch_connected_1;
	}
	else
	{
		Data->image = DPST_switch_disconnected_1;
	}
	return TRUE;
}

void ShowDPSTDialog(void* ComponentAddress)
{
	DPSTStruct* Data = (DPSTStruct*)ComponentAddress;
	BOOL Canceled, Complete = TRUE;
	DPSTDialog = CreateDialogWindow("DPST Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
	MakeDPSTControls();
	ShowDialogWindow();
	DPSTLoadState(Data);
	Canceled = ProcessDialogWindow();
	while (Canceled == FALSE)
	{
		Complete = SaveDPSTDialog(Data);
		if (Complete == TRUE)
		{
			// MessageBox(*SwitchDialog,
			//     ("Saved"), ("Mouse click"), MB_OK | MB_ICONWARNING);
			break;
		}
		else
		{
			Complete = TRUE;
			Canceled = ProcessDialogWindow();
		}
	}
	DestroyWindow(*DPSTDialog);
}

// Initializing the required variables.
int Init_DPST(void* ComponentAddress)
{
	DPSTStruct* temp = (DPSTStruct*)ComponentAddress;
	temp->image = DPST_switch_disconnected_1;
	temp->latched = TRUE;                   // Varibale for latching action
	temp->open = TRUE;
	temp->init_pos = TRUE;
	temp->volt[0] = V_OPEN;
	temp->volt[1] = V_OPEN;
	temp->volt[2] = V_OPEN;
	temp->volt[3] = V_OPEN;

	return DPST_switch_disconnected_1;
}

// Updating the voltages at each pin.
void DPSTUpdateValues(DPSTStruct* dpstData, void* ComponentAddress)
{
	DPSTStruct* temp = (DPSTStruct*)dpstData;
	double v0, v1, v2, v3;
	v0 = VoltRequest(temp->pinId[0], ComponentAddress);
	v1 = VoltRequest(temp->pinId[1], ComponentAddress);
	v2 = VoltRequest(temp->pinId[2], ComponentAddress);
	v3 = VoltRequest(temp->pinId[3], ComponentAddress);
	if (temp->open)				// Open condition.
	{
		temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
		temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
		temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
		temp->volt[3] = VoltChange(temp->pinId[3], 3, ComponentAddress, V_OPEN);
	}
	// Closed condition
	else if (!temp->open) {
		/*if ((v0 > v1) && (v2 > v3))
		{
			temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, v0);
			temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, v2);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, V_OPEN);
			temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, V_OPEN);
		}
		else
		{
			temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, v1);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, v3);
			temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, V_OPEN);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, V_OPEN);
		}*/
		if ((v0 != GND && v1 != GND) && (v2 != GND && v3 != GND)) {
			if ((v0 != V_OPEN && v1 != V_OPEN) && (v2 != V_OPEN && v3 != V_OPEN)) {
				if ((v0 > v1) && (v2 > v3))
				{
					temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, v0);
					temp->volt[3] = VoltChange(temp->pinId[3], 3, ComponentAddress, v2);
				}
				else
				{
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v1);
					temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, v3);
				}
			}
			else if (v1 == V_OPEN && v3 == V_OPEN) {
				temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, v0);
				temp->volt[3] = VoltChange(temp->pinId[3], 3, ComponentAddress, v2);
			}
			else if (v0 == V_OPEN && v2 == V_OPEN) {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v1);
				temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, v3);
			}
		}
		else {
			temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, GND);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, GND);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, GND);
			temp->volt[3] = VoltChange(temp->pinId[3], 3, ComponentAddress, GND);
		}
	}
}

void HandleDPSTEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
	void* ImageLocation, UINT ImageId, HWND* h)
{
	DPSTStruct *temp = (DPSTStruct*)ComponentAddress;
	if (SimulationStarted)
	{
		switch (Event) {
		case EVENT_MOUSE_CLICK:
			if (temp->latched)
			{
				temp->open = !temp->open;
			}
			else
			{
				temp->open = temp->init_pos;
			}
			// Setting the image according to the click.
			SetImage(!temp->open ? DPST_switch_disconnected_1 : DPST_switch_connected_1,
				ImageLocation);
			RefreshImages();
			DPSTUpdateValues(temp, ComponentAddress);
			break;
		}
	}
	else
	{
		switch (Event) {
		case EVENT_MOUSE_DBLCLICK:
			ShowDPSTDialog(ComponentAddress);
			SetImage(temp->image, ImageLocation);
			RefreshImages();
			break;
		}
	}
}

// Voltage change at each pin
double DPSTVoltChanged(void* dpstData, BOOL SimulationStarted, int Index,
	double Volt, int Source, void* ImageLocation)
{
	DPSTStruct* temp = (DPSTStruct*)dpstData;
	if (SimulationStarted)
	{
		// Fetching the voltages at each pin.
		double v0,v1,v2,v3;
		v0 = VoltRequest(temp->pinId[0], dpstData);
		v1 = VoltRequest(temp->pinId[1], dpstData);
		v2 = VoltRequest(temp->pinId[2], dpstData);
		v3 = VoltRequest(temp->pinId[3], dpstData);
		if (temp->open)				// Open condition.
		{
			temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, V_OPEN);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, V_OPEN);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, V_OPEN);
			temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, V_OPEN);
			return temp->volt[Index];
		}
		// Closed condition
		else if (!temp->open) {
			if ((v0 != GND && v1 != GND) && (v2 != GND && v3 != GND)) {
				if ((v0 != V_OPEN && v1 != V_OPEN) && (v2 != V_OPEN && v3 != V_OPEN)) {
					if ((v0 > v1) && (v2 > v3))
					{
						temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, v0);
						temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, v2);
						return temp->volt[Index];
					}
					else
					{
						temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, v1);
						temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, v3);
						return temp->volt[Index];
					}
				}
				else if (v1 == V_OPEN && v3 == V_OPEN) {
					temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, v0);
					temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, v2);
					return (temp->volt[Index]);
				}
				else if (v0 == V_OPEN && v2 == V_OPEN) {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, v1);
					temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, v3);
					return (temp->volt[Index]);
				}
			}
			else {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, GND);
				temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, GND);
				temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, GND);
				temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, GND);
				return (temp->volt[Index]);
			}
		}
	}
	else
	{
		temp->volt[Index] = Volt;
		return Volt;
	}
	return 0;
}

// Setting the pin Ids of each pin.
void SetDPSTIds(int* id, void* ComponentAddress)
{
	DPSTStruct *s = (DPSTStruct*)ComponentAddress;
	s->pinId[0] = *id++;
	s->pinId[1] = *id++;
	s->pinId[2] = *id++;
	s->pinId[3] = *id;
}