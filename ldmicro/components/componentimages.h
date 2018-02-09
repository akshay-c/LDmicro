#ifndef __COMPONENTIMAGES_H
#define __COMPONENTIMAGES_H

#define     SWITCH_CONNECTED        8000
#define     SWITCH_DISCONNECTED     8001
#define     RELAY_NC                8002
#define     RELAY_NO                8003

#ifndef RC_INVOKED                       //Used to hide code from resource file(Guess)

#define TOTAL_COMPONENTS          2
#define COMPONENT_NAME_MAX_LENGTH 50

// Try to keep ComponentID's  between 6000 - 6999

#define     COMPONENT_SWITCH    6000
#define     COMPONENT_RELAY     6001


#define     MAX_PIN_COUNT       10

typedef struct ComponentDataTag{
    int     Index;
    int     ComponentId;
    TCHAR   ComponentName[COMPONENT_NAME_MAX_LENGTH];
    int     PinCount; 
    LPCTSTR PinNames[MAX_PIN_COUNT];                                                 //Valid Number of images from below property
}ComponentData;

extern  ComponentData   rgCompData[TOTAL_COMPONENTS];

void RefreshImages();
void SetImage(int Component, void *il);

#endif
#endif
