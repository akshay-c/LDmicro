#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"
#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

//static HWND TemporaryRadio;
static HWND LatchedRadio_dpst;
static HWND ClosedRadio_dpst;
//static HWND ClosedRadio_2_dpst;
static HWND OpenRadio_dpst;
//static HWND NameTextbox_dpst;
//static HWND InNameTextbox_dpst;
static HWND OutNameTextbox_dpst;
HWND* DPSTDialog;

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
	if (!Data->init_pos)
	{
		Button_SetCheck(ClosedRadio_dpst, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(OpenRadio_dpst, BST_CHECKED);
	}
	//Edit_SetText(NameTextbox_spdt, Data->Name);
}

BOOL SaveDPSTDialog(DPSTStruct* Data)
{
	BOOL latched, init_pos;
	if (Button_GetState(LatchedRadio_dpst) == BST_CHECKED)
	{
		latched = TRUE;

	}
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
	//Data->Open = NOpen;
	if (init_pos)
	{
		Data->image = DPST_switch_disconnected_1;
	}
	else
	{
		Data->image = DPST_switch_connected_1;
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

int Init_DPST(void* ComponentAddress)
{
	DPSTStruct* temp = (DPSTStruct*)ComponentAddress;
	temp->image = DPST_switch_disconnected_1;
	temp->latched = TRUE;
	temp->open = TRUE;
	temp->init_pos = TRUE;
	temp->volt[0] = V_OPEN;
	temp->volt[1] = V_OPEN;
	temp->volt[2] = V_OPEN;
	temp->volt[3] = V_OPEN;

	return DPST_switch_disconnected_1;
}

void HandleDPSTEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
	void* ImageLocation, UINT ImageId, HWND* h)
{
	DPSTStruct *temp = (DPSTStruct*)ComponentAddress;

	if (SimulationStarted)
	{
		switch (Event) {
		case EVENT_MOUSE_UP:
			/*if (temp->Latched)
			{
				temp->Open = !temp->Open;
				/*MessageBox(NULL,
				"Latched", "Test", MB_OK | MB_ICONWARNING);
			}*/
			/*else
			{
				temp->Open = temp->NOpen;
				// MessageBox(NULL,
				// "Latched", "Test", MB_OK | MB_ICONWARNING);
			}*/
			/*SwitchStateChanged(temp, ImageLocation);
			UpdateValues(temp, ComponentAddress);
			break;*/
		case EVENT_MOUSE_DOWN:
			/*if (!temp->Latched)
			{
				temp->Open = !temp->NOpen;
				SwitchStateChanged(temp, ImageLocation);
				UpdateValues(temp, ComponentAddress);
			}
			*/
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

double DPSTVoltChanged(void* dpstData, BOOL SimulationStarted, int Index,
	double Volt, int Source, void* ImageLocation)
{
	DPSTStruct* temp = (DPSTStruct*)dpstData;
	if (SimulationStarted)
	{
		double v0,v1,v2,v3;
		v0 = VoltRequest(temp->pinId[0], dpstData);
		v1 = VoltRequest(temp->pinId[1], dpstData);
		v2 = VoltRequest(temp->pinId[2], dpstData);
		v3 = VoltRequest(temp->pinId[3], dpstData);
		if (temp->open)
		{
			temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, V_OPEN);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, V_OPEN);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, V_OPEN);
			temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, V_OPEN);
			/*sprintf_s(Debug, "SwitchVoltChanged: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
			SwitchData, Voltage, Volt);
			OutputDebugString(Debug);*/
			return temp->volt[Index];
		}
		if ((v0 > v1) && (v2 > v3))
		{
			temp->volt[1] = VoltChange(temp->pinId[1], 1, dpstData, v0);
			temp->volt[3] = VoltChange(temp->pinId[3], 3, dpstData, v2);
			return temp->volt[Index];
		}
		else
		{
			// temp->Volt[!Index] = VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);
			temp->volt[0] = VoltChange(temp->pinId[0], 0, dpstData, v1);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, dpstData, v3);			
			return temp->volt[Index];
		}
	}
	else
	{
		temp->volt[Index] = Volt;
		return Volt;
	}
	return 0;
}

void SetDPSTIds(int* id, void* ComponentAddress)
{
	DPSTStruct *s = (DPSTStruct*)ComponentAddress;
	s->pinId[0] = *id++;
	s->pinId[1] = *id++;
	s->pinId[2] = *id++;
	s->pinId[3] = *id;
}