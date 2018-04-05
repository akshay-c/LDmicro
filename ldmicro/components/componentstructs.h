#ifndef _COMPONENT_STRUCTS
#define _COMPONENT_STRUCTS

typedef struct SwitchStructTag
{
    int     id;
    int     Image;
    BOOL    Latched;                   //Temporary/Latched Action
    BOOL    NOpen;                     //Initial Open/Closed position
    BOOL    Open;
    char    Name[15];
    double  Volt[2];
    int     PinId[2];
}SwitchStruct;

typedef struct RelayStructTag
{
    int     id;
    int     Image;
    BOOL    NC;                        //Whether relay is operated
    double  MinOperatingVolt;             //Operating voltage
    double  MaxOperatingVolt;
    double  CoilVolt1;                 //Voltage at input pin
    double  CoilVolt2;                 //Voltage at input pin
    double  COMVolt;                   //Voltage at COM pin
    double  NOVolt;                    //Voltage at NO pin
    double  NCVolt; 
    int     PinId[5];                   //Voltage at NC pin

}RelayStruct;

typedef struct SPDT_switch_struct_Tag
{
	int id;
	int image;                         // Image of the pin
	int pinId[3];                      // Pin IDs to store 3 pins
	BOOL init_pos;                     // Initial open/closed position
	BOOL latch_1;                      // Latched to the output pin 1
	//BOOL latch_2;                      // Latched to the output pin 2
	double volt[3];                    // Voltage at each pin
	char name[15];
}SPDTStruct;

typedef struct DPST_switch_struct_Tag
{
	int id;
	int image;                         // Image of the pin
	int pinId[4];                      // Pin IDs to store 4 pins
	BOOL init_pos;                     // Initial open/closed position
	BOOL open;                         // Open
	double volt[4];                    // Voltage at each pin
	BOOL latched;
}DPSTStruct;

typedef struct DPDT_switch_struct_Tag
{
	int id;
	int image;                         // Image of the pin
	int pinId[6];                      // Pin IDs to store 6 pins
	BOOL init_pos_1;                   // Initial open/closed position of switch 1
	BOOL init_pos_2;                   // Initial open/closed position of switch 2
	BOOL latch_1;                      // Latched to the output pin 1
	BOOL latch_2;                      // Latched to the output pin 2
	BOOL latch_3;                      // Latched to the output pin 3
	BOOL latch_4;                      // Latched to the output pin 4
	//BOOL open;
	double volt[6];                    // Voltage at each pin
}DPDTStruct;
#endif
