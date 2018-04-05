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
static HWND TemporaryRadio_spdt;
static HWND LatchedRadio_spdt;
static HWND ClosedRadio_1_spdt;
static HWND ClosedRadio_2_spdt;
static HWND NameTextbox_spdt;
static HWND InNameTextbox_spdt;
static HWND OutNameTextbox_spdt;
HWND* SPDTDialog;

// UI for the SPDT switch.
void MakeSPDTControls()
{
	HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Action"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ActionGrouper);

	LatchedRadio_spdt = CreateWindowEx(0, WC_BUTTON, ("Latched"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(LatchedRadio_spdt);

	TemporaryRadio_spdt = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(TemporaryRadio_spdt);

	HWND PositionGrouper = CreateWindowEx(0, WC_BUTTON, ("Position"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
		140, 3, 120, 65, *SPDTDialog, NULL, NULL, NULL);
	FontNice(PositionGrouper);

	ClosedRadio_1_spdt = CreateWindowEx(0, WC_BUTTON, ("Pin 1"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		149, 21, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_1_spdt);

	ClosedRadio_2_spdt = CreateWindowEx(0, WC_BUTTON, ("Pin 2"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
		149, 41, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_2_spdt);
}

void SPDTLoadState(SPDTStruct* Data)
{
	if (Data->latched) 
	{
		Button_SetCheck(LatchedRadio_spdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(TemporaryRadio_spdt, BST_CHECKED);
	}
	if (!Data->init_pos)
	{
		Button_SetCheck(ClosedRadio_2_spdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(ClosedRadio_1_spdt, BST_CHECKED);
	}
}

BOOL SaveSPDTDialog(SPDTStruct* Data)
{
	BOOL latched, init_pos;
	if (Button_GetState(LatchedRadio_spdt) == BST_CHECKED)
	{
		latched = TRUE;
	}
	else if (Button_GetState(TemporaryRadio_spdt) == BST_CHECKED)
	{
		latched = FALSE;
	}
	else
	{
		MessageBox(*SPDTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if (Button_GetState(ClosedRadio_1_spdt) == BST_CHECKED)
	{
		init_pos = TRUE;
	}
	else if (Button_GetState(ClosedRadio_2_spdt) == BST_CHECKED)
	{
		init_pos = FALSE;
	}
	else
	{
		MessageBox(*SPDTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	Data->latched = latched;
	Data->init_pos = init_pos;
	if (init_pos)
	{
		Data->image = SPDT_switch_connected_1;
	}
	else
	{
		Data->image = SPDT_switch_connected_2;
	}
	return TRUE;
}

// Initializing the required variables.
int Init_SPDT(void * ComponentAddress)
{
	SPDTStruct* temp = (SPDTStruct*)ComponentAddress;
	temp->image = SPDT_switch_connected_1;
	temp->latch_1 = TRUE;							// Connection between pin 1 and common pin
	temp->latched = TRUE;							// Varibale for latching action
	temp->init_pos = TRUE;
	temp->volt[0] = V_OPEN;
	temp->volt[1] = V_OPEN;
	temp->volt[2] = V_OPEN;
	return SPDT_switch_connected_1;
}

void ShowSPDTDialog(void* ComponentAddress)
{
	SPDTStruct* Data = (SPDTStruct*)ComponentAddress;
	BOOL Canceled, Complete = TRUE;
	SPDTDialog = CreateDialogWindow("SPDT Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
	MakeSPDTControls();
	ShowDialogWindow();
	SPDTLoadState(Data);
	Canceled = ProcessDialogWindow();
	while (Canceled == FALSE)
	{
		Complete = SaveSPDTDialog(Data);
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
	DestroyWindow(*SPDTDialog);
}

void SPDTUpdateValues(SPDTStruct* spdt, void* ComponentAddress)
{
	SPDTStruct* temp = (SPDTStruct*)spdt;
	double v0, v1, v2;
	if (temp->latch_1 == TRUE) 
	{
		// Fetching the voltages at each pin.
		v0 = VoltRequest(temp->pinId[0], ComponentAddress);
		v1 = VoltRequest(temp->pinId[1], ComponentAddress);

		// Updating the voltages according to priority order mentioned in the documentation.
		if (v0 != GND && v1 != GND) {
			if (v0 != V_OPEN && v1 != V_OPEN) {
				if (v0 > v1) {
					temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, v0);
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
				}
				else {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v1);
					temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
				}
			}
			else if (v1 == V_OPEN) {
				temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, v0);
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
			}
			else if (v0 == V_OPEN) {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v1);
				temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
			}
		}
		else {
			temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, GND);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, GND);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
		}
	}
	else if (temp->latch_1 == FALSE) {
		v0 = VoltRequest(temp->pinId[0], ComponentAddress);
		v2 = VoltRequest(temp->pinId[2], ComponentAddress);
		if (v0 != GND && v2 != GND) {
			if (v0 != V_OPEN && v2 != V_OPEN) {
				if (v0 > v2) {
					temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, v0);
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
				}
				else {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v2);
					temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
				}
			}
			else if (v2 == V_OPEN) {
				temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, v0);
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
			}
			else if (v0 == V_OPEN) {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v2);
				temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
			}
		}
		else {
			temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, GND);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, GND);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
		}
	}
}

void HandleSPDTEvent(void * ComponentAddress, int Event, BOOL SimulationStarted, void * ImageLocation, UINT ImageId, HWND * h)
{
	SPDTStruct *temp = (SPDTStruct*)ComponentAddress;

	if (SimulationStarted)
	{
		if (temp->latched)
		{
			switch (Event) {
			case EVENT_MOUSE_UP:
				temp->latch_1 = !temp->latch_1;
				SetImage(temp->latch_1 ? SPDT_switch_connected_2 : SPDT_switch_connected_1,
					ImageLocation);
				RefreshImages();
				SPDTUpdateValues(temp, ComponentAddress);
				break;
			}
		}
		else
		{
			switch (Event)
			{
			case EVENT_MOUSE_DOWN:
				temp->latch_1 = !temp->init_pos;

				// Updating the values according to the click.
				SPDTUpdateValues(temp, ComponentAddress);

				// Setting the image according to the click.
				SetImage(!temp->latch_1 ? SPDT_switch_connected_2 : SPDT_switch_connected_1,
					ImageLocation);
				RefreshImages();
				break;

			case EVENT_MOUSE_UP:
				temp->latch_1 = temp->init_pos;

				// Setting the image according to the click.
				SetImage(!temp->latch_1 ? SPDT_switch_connected_2 : SPDT_switch_connected_1,
					ImageLocation);
				RefreshImages();

				// Updating the values according to the click.
				SPDTUpdateValues(temp, ComponentAddress);
				break;
			}
		}
	}
	else
	{
		switch (Event) {
		case EVENT_MOUSE_DBLCLICK:
			ShowSPDTDialog(ComponentAddress);
			SetImage(temp->image, ImageLocation);
			RefreshImages();
			break;
		}
	}
}

double SPDTVoltChanged(void* spdtData, BOOL SimulationStarted, int Index,
	double Volt, int Source, void* ImageLocation) {
	// char Debug[256];
	SPDTStruct* temp = (SPDTStruct*)spdtData;
	if (SimulationStarted)
	{
		double v0, v1, v2;
		if (temp->latch_1 == TRUE) {

			// Fetching the voltages at each pin.
			v0 = VoltRequest(temp->pinId[0], spdtData);
			v1 = VoltRequest(temp->pinId[1], spdtData);

			// Changing the voltages according to the priority order mentioned.
			if (v0 != GND && v1 != GND) {
				if (v0 != V_OPEN && v1 != V_OPEN) {
					if (v0 > v1) {
						temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, v0);
						return temp->volt[Index];
					}
					else {
						temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, v0);
						return (temp->volt[Index]);
					}
				}
				else if (v1 == V_OPEN) {
					temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, v0);
					return (temp->volt[Index]);
				}
				else if (v0 == V_OPEN) {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, v1);
					return (temp->volt[Index]);
				}
			}
			else {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, GND);
				temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, GND);
				return (temp->volt[Index]);
			}
		}
		else if (temp->latch_1 == FALSE) {
			v0 = VoltRequest(temp->pinId[0], spdtData);
			v2 = VoltRequest(temp->pinId[2], spdtData);
			if (v0 != GND && v2 != GND) {
				if (v0 != V_OPEN && v2 != V_OPEN) {
					if (v0 > v2) {
						temp->volt[2] = VoltChange(temp->pinId[2], 2, spdtData, v0);
						return temp->volt[2];
					}
					else {
						temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, v2);
						return (temp->volt[Index]);
					}
				}
				else if (v2 == V_OPEN) {
					temp->volt[2] = VoltChange(temp->pinId[2], 2, spdtData, v0);
					return (temp->volt[Index]);
				}
				else if (v0 == V_OPEN) {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, v2);
					return (temp->volt[Index]);
				}
			}
			else {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, GND);
				temp->volt[2] = VoltChange(temp->pinId[2], 2, spdtData, GND);
				return (temp->volt[Index]);
			}
		}
		else
		{
			temp->volt[Index] = Volt;
			return Volt;
		}
		return 0;
	}
}

void SetSPDTIds(int *id, void * ComponentAddress)
{
	SPDTStruct *s = (SPDTStruct*)ComponentAddress;
	s->pinId[0] = *id++;
	s->pinId[1] = *id++;
	s->pinId[2] = *id;
}