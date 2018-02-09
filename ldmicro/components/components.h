#ifndef _COMPONENTS_H
#define _COMPONENTS_H

#define     EVENT_VALUE_CHANGED     0
#define     EVENT_MOUSE_CLICK       1
#define     EVENT_MOUSE_DOWN        2
#define     EVENT_MOUSE_UP          3
#define     EVENT_MOUSE_RDOWN       4
#define     EVENT_MOUSE_RUP         5
#define     EVENT_MOUSE_DBLCLICK    6
#define     EVENT_MOUSE_RCLICK      7



#define     SOURCE_PROGRAM_MAIN     1
#define     SOURCE_PROGRAM_NEW      2
#define     SOURCE_EVENT_MAIN       3
#define     SOURCE_EVENT_NEW        4
#define     SOURCE_FORCE_MAIN       5
#define     SOURCE_REQUEST_MAIN     6

/*#define     EVENT_KEY_DOWN          6
#define     EVENT_KEY_RELEASE       7
#define     EVENT_KEY_PRESS         8*/
#define     V_DIALOG_WIDTH          96
#define     V_DIALOG_HEIGHT         115
#define     H_DIALOG_WIDTH          179
#define     H_DIALOG_HEIGHT         85

//Window styles for dialog box
#define     STYLE_HORIZONTAL        1
#define     STYLE_VERTICAL          2

#define     GND                     0
#define     VOLT_5                  5
#define     V_OPEN                  4196

int NotifyComponent(void *ComponentAddress, void* PinName, int ComponentId, int Event,
    BOOL SimulationStarted, HWND* h, int Index, UINT ImageId, void* ImageLocation);
int InitializeComponentProperties(void *ComponentAddress, int ComponentId);
double VoltSet(void* ComponentAddress, BOOL SimulationStarted, int ImageType, int Index,
    double Volt, int Source, void* ImageLocation);

double VoltRequest(int PinId, void* ComponentAddress);
// void RequestData(void* ComponentAddress);
// double GlobalVoltChange(int PinId, void *ComponentAddress, double volt);
double VoltChange(int PinId, int Index, void* ComponentAddress, double Volt);
size_t GetStructSize(int ComponentId);
size_t GetNameSize(int ComponentId);

#endif
