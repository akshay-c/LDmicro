/* DPST component file
* Code version: 2.1
* Version description: Fixes minor bugs in v2.0
* Version steability:
*	GUI -> Stable
*	Functionality -> Bugs found.
* Bugs:
*	1. Voltages dont load properly during simulation when the "Switch" component controls input pin of the DPST component.
*
*/

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
static HWND State1;
static HWND State2;
static HWND ModeLatchedDPST;
static HWND ModeTempDPST;
HWND* SettingsDialogDPST;

///Global variables
enum DPST_Pins { in1 = 0, in2, out1, out2 };

///Function definitions
int InitDpst(void * ComponentAddress)
{
	DpstStruct* d = (DpstStruct*)ComponentAddress;
	d->image = DPST_1;
	d->NO = FALSE;
	d->latched = TRUE;
	d->Volt[in1] = V_OPEN;
	d->Volt[in2] = V_OPEN;
	d->Volt[out1] = V_OPEN;
	d->Volt[out2] = V_OPEN;

	return DPST_1;
}

void SetDpstIds(int* id, void* ComponentAddress)
{
	DpstStruct* d = (DpstStruct*)ComponentAddress;
	d->PinId[in1] = *id++;
	d->PinId[in2] = *id++;
	d->PinId[out1] = *id++;
	d->PinId[out2] = *id++;
}

void MakeSettingsDialogDPST()
{
	///Switch action mode
	HWND InitLatched = CreateWindowEx(0, WC_BUTTON, ("Action mode"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *SettingsDialogDPST, NULL, NULL, NULL);
	FontNice(InitLatched);

	ModeLatchedDPST = CreateWindowEx(0, WC_BUTTON, ("Latched"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *SettingsDialogDPST, NULL, NULL, NULL);
	FontNice(ModeLatchedDPST);

	ModeTempDPST = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *SettingsDialogDPST, NULL, NULL, NULL);
	FontNice(ModeTempDPST);
	
	///Switch initial status
	HWND InitOut = CreateWindowEx(0, WC_BUTTON, ("Initial output state"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		140, 3, 120, 65, *SettingsDialogDPST, NULL, NULL, NULL);
	FontNice(InitOut);

	State1 = CreateWindowEx(0, WC_BUTTON, ("State 1"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		149, 21, 100, 20, *SettingsDialogDPST, NULL, NULL, NULL);
	FontNice(State1);

	State2 = CreateWindowEx(0, WC_BUTTON, ("State 2"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		149, 41, 100, 20, *SettingsDialogDPST, NULL, NULL, NULL);
	FontNice(State2);
}

void LoadSettings(DpstStruct* d)
{
	
	if (d->latched)
		Button_SetCheck(ModeLatchedDPST, BST_CHECKED);
	else
		Button_SetCheck(ModeTempDPST, BST_CHECKED);
	
	if (!d->NO)
		Button_SetCheck(State1, BST_CHECKED);
	else
		Button_SetCheck(State2, BST_CHECKED);
}

BOOL SaveSettings(DpstStruct* d, void* ImageLocation)
{	
	if (Button_GetState(ModeLatchedDPST) == BST_CHECKED)
		d->latched = TRUE;
	else if (Button_GetState(ModeTempDPST) == BST_CHECKED)
		d->latched = FALSE;
	else
	{
		MessageBox(*SettingsDialogDPST,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if (Button_GetState(State1) == BST_CHECKED)
		d->NO = FALSE;
	else if (Button_GetState(State2) == BST_CHECKED)
		d->NO = TRUE;
	else
	{
		MessageBox(*SettingsDialogDPST,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if (!d->NO)
		d->image = DPST_1;
	else
		d->image = DPST_2;

	SetImage(d->image, ImageLocation);
	RefreshImages();

	return TRUE;
}

void DpstSettingsDialog(void* ComponentAddress, void* ImageLocation)
{
	DpstStruct* d = (DpstStruct*)ComponentAddress;
	BOOL exitStatus;

	//Create dialog window instance
	SettingsDialogDPST = CreateDialogWindow("SPDT Settings Dialog", 100, 100, 263, 145, STYLE_VERTICAL);

	//Make the settings dialog
	MakeSettingsDialogDPST();

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

	DestroyWindow(*SettingsDialogDPST);
}

//Dynamically check and equalise the voltage on all pins that are connected to DPST at runtime
double EqualiseRuntimeVoltageDPST(void* ComponentAdderss, int index, double volt)
{
	DpstStruct* d = (DpstStruct*)ComponentAdderss;
	
	///Check if the switch is open	
	if (d->NO)
	{
		///If switch is open then all terminals will be floating/open
		switch (index)
		{
		case in1:
			d->Volt[in1] = V_OPEN;
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, V_OPEN);
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, V_OPEN);
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, V_OPEN);
			break;
		case in2:
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, V_OPEN);
			d->Volt[in2] = V_OPEN;
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, V_OPEN);
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, V_OPEN);
		case out1:
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, V_OPEN);
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, V_OPEN);
			d->Volt[out1] = V_OPEN;
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, V_OPEN);
			break;
		case out2:
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, V_OPEN);
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, V_OPEN);
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, V_OPEN);
			d->Volt[out2] = V_OPEN;
			break;
		default:
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, V_OPEN);
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, V_OPEN);
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, V_OPEN);
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, V_OPEN);
			break;
		}
	}
	///If the switch is connected
	else
	{
		double Vin;
		double Vout;

		///Get voltages at the connected pins
		if (index == in1)
		{
			Vin = volt;
			Vout = VoltRequest(d->PinId[out1], ComponentAdderss);
		}
		else if (index == out1)
		{
			Vin = VoltRequest(d->PinId[in1], ComponentAdderss);
			Vout = volt;
		}
		else
		{
			Vin = VoltRequest(d->PinId[in1], ComponentAdderss);
			Vout = VoltRequest(d->PinId[out1], ComponentAdderss);
		}

		///Equalise voltage for input 1 output 1 pair
		///If either pin is grounded then all pins are set to GND (Static event)
		if (Vin == GND || Vout == GND)
		{
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, GND);
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, GND);
		}
		///If Vin is set as open
		else if (Vin == V_OPEN)
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, Vout);
		///If Vout is set as open
		else if (Vout == V_OPEN)
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, Vin);
		///If no pin is grounded then all pins are set to the max voltage of the pins (Dynamic event)
		else
		{
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, max(Vin, Vout));
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, max(Vin, Vout));
		}

		///Get voltages at the connected pins
		if (index == in2)
		{
			Vin = volt;
			Vout = VoltRequest(d->PinId[out2], ComponentAdderss);
		}
		else if (index == out2)
		{
			Vin = VoltRequest(d->PinId[in2], ComponentAdderss);
			Vout = volt;
		}
		else
		{
			Vin = VoltRequest(d->PinId[in2], ComponentAdderss);
			Vout = VoltRequest(d->PinId[out2], ComponentAdderss);
		}

		///Equalise voltage for input 2 output 2 pair
		///If either pin is grounded then all pins are set to GND (Static event)
		if (Vin == GND || Vout == GND)
		{
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, GND);
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, GND);
		}
		///If Vin is set as open
		else if (Vin == V_OPEN)
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, Vout);
		///If Vout is set as open
		else if (Vout == V_OPEN)
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, Vin);
		///If no pin is grounded then all pins are set to the max voltage of the pins (Dynamic event)
		else
		{
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, max(Vin, Vout));
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, max(Vin, Vout));
		}
	}

	return d->Volt[index];
}

//Perform a static check and equalise the voltage on all pins that are connected to DPST at runtime
void EqualiseStaticVoltageDPST(void* ComponentAdderss)
{
	DpstStruct* d = (DpstStruct*)ComponentAdderss;

	///Check if the switch is open	
	if (d->NO)
	{
		///If switch is open then all terminals will be floating/open
		d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, V_OPEN);
		d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, V_OPEN);
		d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, V_OPEN);
		d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, V_OPEN);

	}
	///If the switch is connected
	else
	{
		double Vin = VoltRequest(d->PinId[in1], ComponentAdderss);
		double Vout = VoltRequest(d->PinId[out1], ComponentAdderss);

		///Equalise voltage for input 1 output 1 pair
		///If either pin is grounded then all pins are set to GND (Static event)
		if (Vin == GND || Vout == GND)
		{
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, GND);
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, GND);
		}
		///If Vin is set as open
		else if (Vin == V_OPEN)
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, Vout);
		///If Vout is set as open
		else if (Vout == V_OPEN)
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, Vin);
		///If no pin is grounded then all pins are set to the max voltage of the pins (Dynamic event)
		else
		{
			d->Volt[out1] = VoltChange(d->PinId[out1], out1, ComponentAdderss, max(Vin, Vout));
			d->Volt[in1] = VoltChange(d->PinId[in1], in1, ComponentAdderss, max(Vin, Vout));
		}

		Vin = VoltRequest(d->PinId[in2], ComponentAdderss);
		Vout = VoltRequest(d->PinId[out2], ComponentAdderss);

		///Equalise voltage for input 2 output 2 pair
		///If either pin is grounded then all pins are set to GND (Static event)
		if (Vin == GND || Vout == GND)
		{
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, GND);
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, GND);
		}
		///If Vin is set as open
		else if (Vin == V_OPEN)
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, Vout);
		///If Vout is set as open
		else if (Vout == V_OPEN)
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, Vin);
		///If no pin is grounded then all pins are set to the max voltage of the pins (Dynamic event)
		else
		{
			d->Volt[out2] = VoltChange(d->PinId[out2], out2, ComponentAdderss, max(Vin, Vout));
			d->Volt[in2] = VoltChange(d->PinId[in2], in2, ComponentAdderss, max(Vin, Vout));
		}
	}
}

void ToggleState(DpstStruct* d, void* ImageLocation)
{
	d->image = (d->image == DPST_1) ? DPST_2 : DPST_1;
	d->NO = (d->NO == TRUE) ? FALSE : TRUE;
	SetImage(d->image, ImageLocation);
	RefreshImages();
}

void HandleDpstEvent(void * ComponentAddress, int Event, BOOL SimulationStarted, void * ImageLocation, UINT ImageId, HWND * h)
{
	DpstStruct* d = (DpstStruct*)ComponentAddress;
	
	if (SimulationStarted)
	{
		switch (Event)
		{
		case EVENT_MOUSE_DOWN:
			ToggleState(d, ImageLocation);
			EqualiseStaticVoltageDPST(ComponentAddress);
			break;
		case EVENT_MOUSE_UP:
			if (!d->latched)
			{
				ToggleState(d, ImageLocation);
				EqualiseStaticVoltageDPST(ComponentAddress);
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
			DpstSettingsDialog(ComponentAddress, ImageLocation);
			break;
		default:
			break;
		}
	}
}

double DpstVoltChanged(void * ComponentAddress, BOOL SimulationStarted, int index, double Volt, int Source, void * ImageLocation)
{
	if (SimulationStarted)
		return EqualiseRuntimeVoltageDPST(ComponentAddress, index, Volt);
	
	return Volt;
}