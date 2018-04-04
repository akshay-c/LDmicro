#ifndef __COMPONENTIMAGES_H
#define __COMPONENTIMAGES_H

#define     SWITCH_CONNECTED        8000
#define     SWITCH_DISCONNECTED     8001
#define     RELAY_NC                8002
#define     RELAY_NO                8003
#define		SPDT_1					8004
#define		SPDT_2					8005
#define		DPST_1					8006
#define		DPST_2					8007
#define		DPDT_1					8008
#define		DPDT_2					8009



#ifndef RC_INVOKED                       //Used to hide code from resource file(Guess)

#define TOTAL_COMPONENTS          5
#define COMPONENT_NAME_MAX_LENGTH 50

// Try to keep ComponentID's  between 6000 - 6999

#define     COMPONENT_SWITCH    6000
#define     COMPONENT_RELAY     6001
#define		COMPONENT_SPDT		6002
#define		COMPONENT_DPST		6003
#define		COMPONENT_DPDT		6004


#define     MAX_PIN_COUNT       10

typedef struct ComponentDataTag{
    int     Index;
    int     ComponentId;
    TCHAR   ComponentName[COMPONENT_NAME_MAX_LENGTH];
    int     PinCount; 
    LPCTSTR PinNames[MAX_PIN_COUNT];                                                 //Valid Number of images from below property
}ComponentData;

void RefreshImages();
void SetImage(int Component, void *il);

static ComponentData rgCompData[TOTAL_COMPONENTS] = {
    {0, COMPONENT_SWITCH, TEXT("Switch"), 2, {"Input:", "Output:"}},
    {1, COMPONENT_RELAY, TEXT("Relay"), 5, {"Coil1:", "Coil2:", "NO:", "COM:", "NC:"}},
	{2, COMPONENT_SPDT,	TEXT("SPDT"), 3, {"Input:", "Output1:", "Output2:"}},
	{3, COMPONENT_DPST, TEXT("DPST"), 4, {"Input1:", "Input2:", "Output1:", "Output2:"}},
	{4, COMPONENT_DPDT, TEXT("DPDT"), 6, {"Input1:", "Input2:", "Output11:", "Output12:", "Output21:", "Output22:"}}
};

#endif
#endif
