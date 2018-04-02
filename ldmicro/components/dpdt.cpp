///Includes
#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"	
#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

///Window handles
static HWND DPDTState1;
static HWND DPDTState2;
HWND* SettingsDialogDPDT;

///Global variables
enum DPDT_Pins { in1 = 0, in2, out11, out12, out21, out22 };

///Function definitions
int InitDpdt(void * ComponentAddress)
{
	DpdtStruct* d = (DpdtStruct*)ComponentAddress;
	d->image = DPDT_1;
	d->NS1 = TRUE;
	d->Volt[in1] = V_OPEN;
	d->Volt[in2] = V_OPEN;
	d->Volt[out11] = V_OPEN;
	d->Volt[out12] = V_OPEN;
	d->Volt[out21] = V_OPEN;
	d->Volt[out22] = V_OPEN;

	return DPDT_1;
}

void SetDpdtIds(int* id, void* ComponentAddress)
{
	DpdtStruct* d = (DpdtStruct*)ComponentAddress;
	d->PinId[in1] = *id++;
	d->PinId[in2] = *id++;
	d->PinId[out11] = *id++;
	d->PinId[out12] = *id++;
	d->PinId[out21] = *id++;
	d->PinId[out22] = *id++;
}

void MakeSettingsDialogDPDT()
{
	HWND InitOut = CreateWindowEx(0, WC_BUTTON, ("Initial output state"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *SettingsDialogDPDT, NULL, NULL, NULL);
	FontNice(InitOut);

	DPDTState1 = CreateWindowEx(0, WC_BUTTON, ("State 1"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *SettingsDialogDPDT, NULL, NULL, NULL);
	FontNice(DPDTState1);

	DPDTState2 = CreateWindowEx(0, WC_BUTTON, ("State 2"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *SettingsDialogDPDT, NULL, NULL, NULL);
	FontNice(DPDTState2);
}

void LoadSettings(DpdtStruct* d)
{
	if (d->NS1)
		Button_SetCheck(DPDTState1, BST_CHECKED);
	else
		Button_SetCheck(DPDTState2, BST_CHECKED);
}

BOOL SaveSettings(DpdtStruct* d, void* ImageLocation)
{
	BOOL NState;
	if (Button_GetState(DPDTState1) == BST_CHECKED)
		NState = TRUE;
	else if (Button_GetState(DPDTState2) == BST_CHECKED)
		NState = FALSE;
	else
	{
		MessageBox(*SettingsDialogDPDT,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	d->NS1 = NState;

	if (NState)
		d->image = DPDT_1;
	else
		d->image = DPDT_2;

	SetImage(d->image, ImageLocation);
	RefreshImages();

	return TRUE;
}

void DpdtSettingsDialog(void* ComponentAddress, void* ImageLocation)
{
	DpdtStruct* d = (DpdtStruct*)ComponentAddress;
	BOOL exitStatus;

	//Create dialog window instance
	SettingsDialogDPDT = CreateDialogWindow("SPDT Settings Dialog", 100, 100, 263, 145, STYLE_VERTICAL);

	//Make the settings dialog
	MakeSettingsDialogDPDT();

	//Load settings
	LoadSettings(d);

	//Show dialog window
	ShowDialogWindow();

	exitStatus = ProcessDialogWindow();
	while (exitStatus == FALSE)
	{
		exitStatus = SaveSettings(d, ImageLocation);
		if (exitStatus == TRUE)
			break;
		else
		{
			exitStatus = TRUE;
			exitStatus = ProcessDialogWindow();
		}
	}

	DestroyWindow(*SettingsDialogDPDT);
}

//Dynamically check and equalise the voltage on all pins that are connected to DPST at runtime
double EqualiseRuntimeVoltageDPDT(void* ComponentAdderss, int index = 0)
{
	DpdtStruct* d = (DpdtStruct*)ComponentAdderss;

	//To be finished

	return d->Volt[index];
}

void ToggleState(DpdtStruct* d, void* ImageLocation)
{
	d->image = (d->image == DPDT_1) ? DPDT_2 : DPDT_1;
	d->NS1 = (d->NS1 == TRUE) ? FALSE : TRUE;
	SetImage(d->image, ImageLocation);
	RefreshImages();
}

void HandleDpdtEvent(void * ComponentAddress, int Event, BOOL SimulationStarted, void * ImageLocation, UINT ImageId, HWND * h)
{

	DpdtStruct* d = (DpdtStruct*)ComponentAddress;
	
	if (SimulationStarted)
	{
		switch (Event)
		{
		case EVENT_MOUSE_CLICK:
			ToggleState(d, ImageLocation);
			EqualiseRuntimeVoltageDPDT(ComponentAddress);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (Event)
		{
		case EVENT_MOUSE_DBLCLICK:
			DpdtSettingsDialog(ComponentAddress, ImageLocation);
			break;
		default:
			break;
		}
	}
}

double DpdtVoltChanged(void * ComponentAddress, BOOL SimulationStarted, int index, double Volt, int Source, void * ImageLocation)
{
	
	if (SimulationStarted)
		return EqualiseRuntimeVoltageDPDT(ComponentAddress, index);
		
	return Volt;
}