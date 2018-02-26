
Advanced Simulation Mode for LDMicro
======

 LDMicro is already equipped with a basic ``Simulation Mode`` under ``Simulation`` menu. The present simulation is very basic making it difficult to understand working of complicated ladder programs/setups. In order to make it easier for the user to visualize end components and their working on a hardware, advanced simulation dialog has been introduced to LDMicro.

######
Summary
######

 The ``Advanced Simulation Window`` comprises of 4 sections namely:
  1. Menu Bar: At the top
  2. Component List: Right pane
  3. Workspace Area: Central black area
  4. IO List: Bottom pane

Menu Bar
^^^^^^

 Most of the menu items in this dialog are similar to that of ``Main LDMicro Window``.

Component List
^^^^^^

 This comprises of list of items that are available for simulation eg. Switch, Relay etc. You may double click any item of your choice in order to draw it on ``Workspace Area`` described below.

Workspace Area
^^^^^^

 The main black area in front of you on which components are drawn automatically is called the workspace area. As soon as you double click on name of component that you wish to work with, that component will be added at a predefined location in the workspace area. Once your component is drawn on workspace area, you may drag and place it at any location of your convenience within the area. The window is not advanced enough to handle drawn connections; so in order to establish a connection between pins of a component, right click on the image displayed for the component, and a pin naming dialog will be displayed for the same. Name the pins accordingly; Components with similar pin names will be virtually considered connected.

IO List
^^^^^^

 This list will replicate the IO list of the main window along with a few components of its own. After assigning pin names to a component, the name is added to this list along with the value representing voltage level of the same.

Voltage Conventions
******

 Since we have standard voltage conventions if you wish to use them instead of direct values so as to make it easier for you to denote the values. A list of conventions is as follows:
  1. V_OPEN: This denotes that the component pin is not connected to any IO from the micro-controller or to any voltage line directly.
  2. GND: This states that the component pin is connected to GND terminal.
  3. VOLT_5: This states that the component pin is connected to 5v Supply.

Developer's Guide
======

######
How to add components to Advanced Simulation Window?
######

Add Images for Simulation
------

   Create .png images with blank background and red ink and place them in ``ldmicro/Img`` folder. Example images can be found at the same location. More than one image can be stored for a single component depending on different states of your component.

``componentimages.h`` file
^^^^^^

After you have copied your images into the appropriate folder/s, open ``components/componentimages.h`` and add a #define directive and assign a unique number in reference for your image according to the list specified.

eg.
``#define     SWITCH_DISCONNECTED     8001``

``ldmicro.rc`` file
^^^^^^

Next, we need to locate the image that is referred in ``componentimages.h`` file. Using the directive we just defined, set a location to the image in resources file ``ldmicro.rc``. Syntax for the same is as under

<#define Directive>   <Image Extension>  "<ImageLocation>"

eg.
``SWITCH_DISCONNECTED     PNG         "img\\switch_disconnected.png"``


An entry to the list of components on Advanced Window Dialog is required in order for the end user to select the component you are about to create. The next topic will give a detailed description to do so.

Add Entry to the ComponentList
------

``componentimages.h`` file
^^^^^^

We have already modified this file to create a #define directive to register images that are to be drawn for the new component. Now we will add a #define directive for the component for which these images will be created.

eg. ``#define COMPONENT_SWITCH 6000``


Structure ``rgCompData``:
******

Add an entry to ``rgCompData`` struct. Make sure that every entry is separated by a new line, so as to make it easier for the other programmers to modify the list.

Elements of the structure are as follows:

* **Index**: Zero based index of the position of your entry. Add your entry to the end of this list, with the closest greater integer after previous entry 

   eg. if the last entry of your list is

 {``1``, <COMPONENTID>, <TEXT>, <PINCOUNT(n)>,{<PINName1>, <PinName2>, ...<PinName(n)>}``

   your new entry should start with 2 as you first element

 {``2``, <COMPONENTID>, <TEXT>, <PINCOUNT(n)>,{<PINName1>, <PinName2>, ...<PinName(n)>}``

* **ComponentId**:   This points to the #define directive we have just created for the component to be added.

eg. {<Index>, ``COMPONENT_SWITCH``, <TEXT>, <PINCOUNT(n)>,{<PINName1>, <PinName2>, ...<PinName(n)>}``

* **TEXT**:   The name to be displayed on the componentlist for out component.

* **PinCount**: Number of Input/Output Pins on your device. In case of SPST switch, we have one input and one output. Hence the pincount '2' is used here.

* **PinNames**: This is an array of names for the IO pins which will be displayed in the dialogbox after right clicking the component image. Since we have specified ``{"Input:", "Output:"}`` as PinNames for switch; if you rightclick a switch image in non-simulation mode, you will be presented with a dialog asking for variables to be used to denote pin.

Create structure for your component
------

* Now that we have an entry of our component in the dialog, we need to create a structure which we will use to store data for our switch. Create a structure in below mentioned file to maintain code readability

     ``components\componentstructs.h``

 Locate SwitchStruct in the above mentioned file to understand an example of the structure.

``In addition to the variables that will be required by you, create a variable to store PinId of every pin in it.``

* Memory allocation for component-structures is handled by the main program, so after creating this structure; Locate ``GetStructSize(int ComponentId)`` function in ``components\components.cpp`` file. Add new case inside switch construct with component name created above and return sizeof newly added structure.

 eg.``case COMPONENT_SWITCH:  //ComponentId mentioned in rgCompData structure
   return sizeof(SwitchStruct);  //Structure created in componentstructs above.

Create functions for your components
------

Different types of functions are expected for every components. This section will deal with creation, storage and functionality of different functions to be added to the project.

Kindly add a declaration of these functions in ``componentfunctions.h`` file at appropriate locations so as it is easier to spot for modifications if required later.

``Note that the names provided here are just for reference add appropriate prefix of the component that they belong to before function names wherever required to avoid duplicate names.``

1. **Set<Component>Id**: 

Locate the function ``SetPinIds()`` in the file ``component.cpp``. Within the switch-case construct, add new case with your ``ComponentId`` to call your function.Do not forget to add appropriate declaration in ``component.h`` file's ``Program Reference functions`` section.

 * **PinIds**: This is a special array of integers which will contain a list of PinIds chronologically as described in the ``rgCompData`` structure. The number of pins will depend on ``PinCount`` in ``rgCompData`` structure. And their order will depend upon elements of ``PinName`` array in the same structure.
 * **ComponentAddress**: There has to be special mention of this pointer as almost all of our functions are dependent on it. If you recall adding a structure in the previous step, we have already presented the main code with the size of the structure of the component that we are adding. The main code, then assigns a memory of the size requested. When the component faces an event, that event is passed on to you along with the location of the component structure. You can later treat this location as a pointer to the component structure, and set the values required by you on occurrence of next event on this component. In this case, the event is associated with storing PinIds, since during main operations they will play an important role.

   eg. Refer to function ``SetSwitchIds()`` in the file ``Switch.cpp`` to understand how we store ``PinIds`` for the structure ``SwitchStruct``.

2. **Initialization Component**: 

You will requre a component to initialize default state of your structure when your component is selected from ComponentList. To do so, locate function ``InitializeComponentProperties(void *ComponentAddress, int ComponentId)`` and add a new case in switch construct referring to the function that is to be called during initialization. Add appropriate declaration in ``component.h`` file's ``Initialization functions`` section.

  We are now ready to add core functionality functions to our program. Apart from description of function arguments, we will also understand a layman working of these functions. These functions are special in a sense that they do not provide simple operations.

3. **VoltRequest**: This function is already defined in the main code. The  real-time voltage value of every component is stored in the main program and can be accessed using 

    ``double VoltRequest(int PinId,void \*ComponentAddress);``

function. If you are storing previous values in your custom structure, that may not be valid at the time of this function call since same PinId may be used by different components. Hence always make sure to call VoltRequest mentioning PinId and ComponentAddress to get values of PinId. This is a passive function in a sense that it does not update the values of other components, but simply returns you with the Voltage value at the ``PinId`` you have requested.

4. **VoltChange**:

 This function is defined in ``components.cpp`` file. It does not change the voltage to the one requested by you directly. It will request Voltage for the requested PinId from other components before deciding whether or not to accept the Voltage you are requesting to be changed.

  ``double VoltChange(int PinId, int Index, void* ComponentAddress, double Volt)``
 * PinId: The PinId of Pin for which voltage is to be changed.
 * Index: Index at which the voltage is to be changed; Since same component may use same PinIds.
 * ComponentAddress: Pointer to the component for which Voltage is to be changed.
 * Volt: Actual voltage value to be set.

5. **Incoming Request Handlers**: ``eg. SwitchVoltChanged()``

 From now on it will start getting tricky, as the values at the pin may or may not be static values. Before moving forward let us look at scenario's in which static/dynamic values are addressed.

Static Values
^^^^^^

 Consider a situation where switch is directly connected to a static voltage source such as ``GND`` signal at one end and micro-controller at other end. In this scenario, whenever the switch is pressed, micro-controller pin should be forced low. This is a static situation since signal ``GND`` is directly connected to the switch. The signal at this end will not change.

Dynamic Values
^^^^^^

The situation described below is the only one of the few conditions that may arise in designing a pilot circuit.

 Suppose we have two switches connected in series between ``GND`` signal and micro-controller. We will name the connections ``GND = 'Connection at GND end'``, ``MCU = 'Connection at Micro-controller end'`` and ``CONN = 'Connection Between both switches'``.

  Now, when switch connected to MCU is pressed, leaving GND switch open, CONN must read 5V because of internal pull up resistors on Micro-controller.

  If the switch connected to the 'GND' terminal is pressed, ``CONN`` must read ``GND voltage`` irrespective of the condition of ``MCU switch``; at the same time if the ``MCU switch`` is pressed, the ``GND`` signal should be further passed to the ``MCU Pin``.

If you design a component, it is not possible to store state of every other components as they may not have been thought of at the point of creation of your component and number of resources required will be directly proportional to the number of components in the design. The next couple of functions come into picture to address this particular issue.

   The ``VoltRequest()`` function discussed comes to your rescue. It is supposed to present you with a proper, updated value of your pin at runtime.

Role of Incoming Request Handlers
^^^^^^

 These functions are automatically called by the main program when other components with matching PinId request voltage from your component. Incoming request handlers are to be defined in your component's ``.cpp`` file. Appropriate declaration is to be provided at ``Request Handlers`` section of your ``componentfunctions.h`` file. Next create a case for your component in switch construct of

   ``VoltSet(void* ComponentAddress, BOOL SimulationStarted, int ImageType, int Index, double Volt, int Source, void* ImageLocation)``

 function of ``components.cpp`` file.

 * ComponentAddress: This is the pointer to the Structure, that we have created for the component.
 * SimulationStarted: This is boolean type pointer to indicate if the Real Time Simulation is started or not.
 * ImageType: This is required for our switch case construct and not the main program.
 * Index: Zero based index of pin mentioned in rgCompData structure.
 * Volt: Current Voltage value of Pin at index specified above.
 * Source: This is for future use, It mentions source from which this function was called you may ignore this argument for now.
 * ImageLocation: This argument is pointer, pointing to the location of current image being displayed on screen for the component. You may change this value to desired image using ``SetImage(int ImageId, void* ImageLocation)`` function in ``switch.cpp``.
     ImageId: Pass macro of the image that you want to display eg. ``SWITCH_DISCONNECTED`` to display different image for your component at any time.

eg. Refer to ``SwitchVoltChanged()`` function in ``switch.cpp`` file.

6. **Event Handlers**: eg. HandleSwitchEvent()

 These functions are called when mouse events occur on the image representing your component. You may request value change, image change in this function depending on requirements.  ``eg. Whenever a switch is pressed,HandleSwitchEvent() function toggles the image displayed for the switch between SWITCH_CONNECTED and SWITCH_DISCONNECTED  and checks the voltage on both ends to request voltage change for a pin with higher potential``

Event handlers are to be defined in your component's ``.cpp`` file. Appropriate declaration is to be provided at ``Event Handlers`` section of your ``componentfunctions.h`` file. Next create a case for your component in switch construct of

   ``NotifyComponent(void* ComponentAddress, void *PinName, int ComponentId, int Event, BOOL SimulationStarted, HWND*h, int Index, UINT ImageId, void *ImageLocation)``

 function of ``components.cpp`` file.

 * ComponentAddress: This is the pointer to the Structure, that we have created for the component.
 * SimulationStarted: This is boolean type pointer to indicate if the Real Time Simulation is started or not.
 * Event: Event that has occurred for your component, in case you need to handle more than one event at runtime, this function can be used to call different functions as and when required. Possible event values are
  * EVENT_MOUSE_CLICK
  * EVENT_MOUSE_DOWN
  * EVENT_MOUSE_UP
  * EVENT_MOUSE_RDOWN
  * EVENT_MOUSE_RUP
  * EVENT_MOUSE_DBLCLICK
  * EVENT_MOUSE_RCLICK``
 
.. _Readme.rst: ldmicro/README.txt
