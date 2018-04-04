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
static HWND StateOut1;
static HWND StateOut2;
static HWND ModeLatched;
static HWND ModeTemp;
HWND* SettingsDialogSPDT;
 
///Global variables
enum SPDT_Pins {in = 0, out1, out2};

///Function definitions
void SetSpdtIds(int* id, void* ComponentAddress)
{
	SpdtStruct* s = (SpdtStruct*)ComponentAddress;
	s->PinId[in] = *id++;
	s->PinId[out1] = *id++;
	s->PinId[out2] = *id++;
}

int InitSpdt(void * ComponentAddress)
{
	SpdtStruct* s = (SpdtStruct*)ComponentAddress;
	s->image = SPDT_1;
	s->latched = TRUE;
	s->NO1 = TRUE;
	s->Volt[in] = V_OPEN;
	s->Volt[out1] = V_OPEN;
	s->Volt[out2] = V_OPEN;

	return SPDT_1;
}

void MakeSettingsDialogSPDT()
{
	///Switch action mode
	HWND InitLatched = CreateWindowEx(0, WC_BUTTON, ("Action mode"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *SettingsDialogSPDT, NULL, NULL, NULL);
	FontNice(InitLatched);

	ModeLatched = CreateWindowEx(0, WC_BUTTON, ("Latched"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *SettingsDialogSPDT, NULL, NULL, NULL);
	FontNice(ModeLatched);

	ModeTemp = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *SettingsDialogSPDT, NULL, NULL, NULL);
	FontNice(ModeTemp);

	///Switch initial status
	HWND InitOut = CreateWindowEx(0, WC_BUTTON, ("Initial output"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		140, 3, 120, 65, *SettingsDialogSPDT, NULL, NULL, NULL);
	FontNice(InitOut);

	StateOut1 = CreateWindowEx(0, WC_BUTTON, ("Output 1"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		149, 21, 100, 20, *SettingsDialogSPDT, NULL, NULL, NULL);
	FontNice(StateOut1);

	StateOut2 = CreateWindowEx(0, WC_BUTTON, ("Output 2"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		149, 41, 100, 20, *SettingsDialogSPDT, NULL, NULL, NULL);
	FontNice(StateOut2);
}

void LoadSettings(SpdtStruct* s)
{
	if (s->latched)
		Button_SetCheck(ModeLatched, BST_CHECKED);
	else
		Button_SetCheck(ModeTemp, BST_CHECKED);
	if (s->NO1)
		Button_SetCheck(StateOut1, BST_CHECKED);
	else
		Button_SetCheck(StateOut2, BST_CHECKED);
}

BOOL SaveSettings(SpdtStruct* s, void* ImageLocation)
{	
	if (Button_GetState(ModeLatched) == BST_CHECKED)
		s->latched = TRUE;
	else if (Button_GetState(ModeTemp) == BST_CHECKED)
		s->latched = FALSE;
	else
	{
		MessageBox(*SettingsDialogSPDT,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if (Button_GetState(StateOut1) == BST_CHECKED)
		s->NO1 = TRUE;
	else if (Button_GetState(StateOut2) == BST_CHECKED)
		s->NO1 = FALSE;
	else
	{
		MessageBox(*SettingsDialogSPDT,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}


	if (s->NO1)
		s->image = SPDT_1;
	else
		s->image = SPDT_2;
	
	SetImage(s->image, ImageLocation);
	RefreshImages();

	return TRUE;
}

void SpdtSettingsDialog(void* ComponentAddress, void* ImageLocation)
{
	SpdtStruct* s = (SpdtStruct*)ComponentAddress;
	BOOL exitStatus;

	//Create dialog window instance
	SettingsDialogSPDT = CreateDialogWindow("SPDT Settings Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
	
	//Make the settings dialog
	MakeSettingsDialogSPDT();
	
	//Load settings
	LoadSettings(s);

	//Show dialog window
	ShowDialogWindow();

	exitStatus = ProcessDialogWindow();
	while (exitStatus == FALSE)
	{
		exitStatus = SaveSettings(s, ImageLocation);
		if (exitStatus == TRUE)
			break;
		else
		{
			exitStatus = TRUE;
			exitStatus = ProcessDialogWindow();
		}
	}

	DestroyWindow(*SettingsDialogSPDT);

}

//Dynamically check and equalise the voltage on all pins that are connected to SPDT at runtime
double EqualiseRuntimeVoltageSPDT(void* ComponentAdderss, int index = 0)
{
	SpdtStruct* s = (SpdtStruct*)ComponentAdderss;

	///Check if input and output 1 are connected
	if (s->NO1)
	{
		///If the input pin is connected to output 1 then output 2 will be open
		s->Volt[out2] = VoltChange(s->PinId[out2], out2, ComponentAdderss, V_OPEN);

		///Get voltages at the connected pins
		double volt1 = VoltRequest(s->PinId[in], ComponentAdderss);
		double volt2 = VoltRequest(s->PinId[out1], ComponentAdderss);
		
		///If either pin is grounded then all pins are set to GND
		if (volt1 == GND || volt2 == GND)
		{
			s->Volt[out1] = VoltChange(s->PinId[out1], out1, ComponentAdderss, GND);
			s->Volt[in] = VoltChange(s->PinId[in], in, ComponentAdderss, GND);
		}
		///If no pin is grounded then all pins are set to the max voltage of the pins
		else
		{
			s->Volt[out1] = VoltChange(s->PinId[out1], out1, ComponentAdderss, max(volt1, volt2));
			s->Volt[in] = VoltChange(s->PinId[in], in, ComponentAdderss, max(volt1, volt2));
		}
	}
	///If input and output 2 are connected
	else
	{
		///If the input pin is connected to output 2 then output 1 will be open
		s->Volt[out1] = VoltChange(s->PinId[out1], out1, ComponentAdderss, V_OPEN);

		///Get voltages at the connected pins
		double volt1 = VoltRequest(s->PinId[0], ComponentAdderss);
		double volt2 = VoltRequest(s->PinId[out2], ComponentAdderss);

		///If either pin is grounded then all pins are set to GND (Static event)
		if (volt1 == GND || volt2 == GND)
		{
			s->Volt[out2] = VoltChange(s->PinId[out2], out2, ComponentAdderss, GND);
			s->Volt[in] = VoltChange(s->PinId[in], in, ComponentAdderss, GND);
		}
		///If no pin is grounded then all pins are set to the max voltage of the pins (Dynamic event)
		else
		{
			s->Volt[out2] = VoltChange(s->PinId[out2], out2, ComponentAdderss, max(volt1, volt2));
			s->Volt[in] = VoltChange(s->PinId[in], in, ComponentAdderss, max(volt1, volt2));
		}
	}

	return s->Volt[index];
}

double SpdtVoltChanged(void * ComponentAddress, BOOL SimulationStarted, int index, double Volt, int Source, void * ImageLocation)
{
	if (SimulationStarted)
		return EqualiseRuntimeVoltageSPDT(ComponentAddress, index);

	return Volt;
}

void ToggleState(SpdtStruct* s, void* ImageLocation)
{
	s->image = (s->image == SPDT_1) ? SPDT_2 : SPDT_1;
	s->NO1 = (s->NO1 == TRUE) ? FALSE : TRUE;
	SetImage(s->image, ImageLocation);
	RefreshImages();
}

void HandleSpdtEvent(void * ComponentAddress, int Event, BOOL SimulationStarted, void * ImageLocation, UINT ImageId, HWND * h)
{
	SpdtStruct* s = (SpdtStruct*)ComponentAddress;

	if (SimulationStarted)
	{
		switch (Event)
		{
		case EVENT_MOUSE_DOWN:
			ToggleState(s, ImageLocation);
			EqualiseRuntimeVoltageSPDT(ComponentAddress);
			break;
		case EVENT_MOUSE_UP:
			if (!s->latched)
			{
				ToggleState(s, ImageLocation);
				EqualiseRuntimeVoltageSPDT(ComponentAddress);
			}
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
			SpdtSettingsDialog(ComponentAddress, ImageLocation);
			break;
		default:
			break;
		}
	}	
}