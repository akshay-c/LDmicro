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

///Global variables
enum DPST_Pins { in1 = 0, in2, out1, out2 };


int InitDpst(void * ComponentAddress)
{
	DpstStruct* d = (DpstStruct*)ComponentAddress;
	d->image = DPST_1;
	d->NO = FALSE;
	d->Volt[in1] = V_OPEN;
	d->Volt[in2] = V_OPEN;
	d->Volt[out1] = V_OPEN;
	d->Volt[out2] = V_OPEN;

	return DPST_1;
}

void HandleDpstEvent(void * ComponentAddress, int Event, BOOL SimulationStarted, void * ImageLocation, UINT ImageId, HWND * h)
{
}

double DpstVoltChanged(void * ComponentAddress, BOOL SimulationStarted, int index, double Volt, int Source, void * ImageLocation)
{
	return 0.0;
}

void SetDpstIds(int* id, void* ComponentAddress)
{
	DpstStruct* d = (DpstStruct*)ComponentAddress;
	d->PinId[in1] = *id++;
	d->PinId[in2] = *id++;
	d->PinId[out1] = *id++;
	d->PinId[out2] = *id++;
}