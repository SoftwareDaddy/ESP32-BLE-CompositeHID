#include "GamepadConfiguration.h"

GamepadConfiguration::GamepadConfiguration() : 
    CompositeDeviceConfiguration(GAMEPAD_REPORT_ID),
    _controllerType(CONTROLLER_TYPE_GAMEPAD),
    _buttonCount(16),
    _hatSwitchCount(1),
    _whichSpecialButtons{false, false, false, false, false, false, false, false},
    _whichAxes{true, true, true, true, true, true, true, true},
    _whichSimulationControls{false, false, false, false, false},
    _axesMin(0x0000),
    _axesMax(0x7FFF),
    _simulationMin(0x0000),
    _simulationMax(0x7FFF)
{
}

uint8_t GamepadConfiguration::getDeviceReportSize()
{
    uint8_t buttonPaddingBits = 8 - (this->getButtonCount() % 8);
    if (buttonPaddingBits == 8)
    {
        buttonPaddingBits = 0;
    }
    uint8_t specialButtonPaddingBits = 8 - (this->getTotalSpecialButtonCount() % 8);
    if (specialButtonPaddingBits == 8)
    {
        specialButtonPaddingBits = 0;
    }

    uint8_t numOfAxisBytes = this->getAxisCount() * 2;
    uint8_t numOfSimulationBytes = this->getSimulationCount() * 2;

    uint8_t numOfButtonBytes = this->getButtonCount() / 8;
    if (buttonPaddingBits > 0)
    {
        numOfButtonBytes++;
    }

    uint8_t numOfSpecialButtonBytes = this->getTotalSpecialButtonCount() / 8;
    if (specialButtonPaddingBits > 0)
    {
        numOfSpecialButtonBytes++;
    }

    reportSize = numOfButtonBytes + numOfSpecialButtonBytes + numOfAxisBytes + numOfSimulationBytes + this->getHatSwitchCount();
    return reportSize;
}

void GamepadConfiguration::makeDeviceReport(uint8* buffer, size_t size)
{
    if(!buffer || !size)
        return;
    
    uint8_t tempHidReportDescriptor[150];
    int hidReportDescriptorSize = 0;

    // Report description START -------------------------------------------------

    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1); //0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; //Generic Desktop

    // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
    tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = this->getControllerType();

    // COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = COLLECTION(1); //0xa1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // REPORT_ID (Gamepad)
    tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_ID(1);
    tempHidReportDescriptor[hidReportDescriptorSize++] = this->getGamepadHidReportId();

    if (this->getButtonCount() > 0)
    {
        // USAGE_PAGE (Button)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1); //0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;

        // LOGICAL_MINIMUM (0)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(1);//0x15;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(1); //0x25;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // REPORT_SIZE (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE_MINIMUM (Button 1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_MINIMUM(1);//0x19;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE_MAXIMUM (Up to 128 buttons possible)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_MAXIMUM(1);//0x29;
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getButtonCount();

        // REPORT_COUNT (# of buttons)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getButtonCount();

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); //0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        uint8_t buttonPaddingBits = getButtonPaddingBits();
        if (buttonPaddingBits > 0)
        {
            // REPORT_SIZE (1)
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (# of padding bits)
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = buttonPaddingBits;

            // INPUT (Const,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] =  HIDINPUT(1); //0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

        } // Padding Bits Needed

    } // Buttons

    if (this->getTotalSpecialButtonCount() > 0)
    {
        // LOGICAL_MINIMUM (0)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(1); //0x15;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(1); //;0x25;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // REPORT_SIZE (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        if (this->getDesktopSpecialButtonCount() > 0)
        {

            // USAGE_PAGE (Generic Desktop)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1); // 0x05;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = this->getDesktopSpecialButtonCount();
            if (this->getIncludeStart())
            {
                // USAGE (Start)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3D;
            }

            if (this->getIncludeSelect())
            {
                // USAGE (Select)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3E;
            }

            if (this->getIncludeMenu())
            {
                // USAGE (App Menu)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x86;
            }

            // INPUT (Data,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); //0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
        }

        if (this->getConsumerSpecialButtonCount() > 0)
        {

            // USAGE_PAGE (Consumer Page)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1); //0x05;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x0C;

            // REPORT_COUNT
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = this->getConsumerSpecialButtonCount();

            if (this->getIncludeHome())
            {
                // USAGE (Home)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(2); //0x0A;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x23;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
            }

            if (this->getIncludeBack())
            {
                // USAGE (Back)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(2); //0x0A;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x24;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
            }

            if (this->getIncludeVolumeInc())
            {
                // USAGE (Volume Increment)
                tempHidReportDescriptor[hidReportDescriptorSize++] =  USAGE(1); //0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xE9;
            }

            if (this->getIncludeVolumeDec())
            {
                // USAGE (Volume Decrement)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xEA;
            }

            if (this->getIncludeVolumeMute())
            {
                // USAGE (Mute)
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xE2;
            }

            // INPUT (Data,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); //0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
        }

        if (specialButtonPaddingBits > 0)
        {

            // REPORT_SIZE (1)
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (# of padding bits)
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = specialButtonPaddingBits;

            // INPUT (Const,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); //0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

        } // Padding Bits Needed

    } // Special Buttons

    if (this->getAxisCount() > 0)
    {
        // USAGE_PAGE (Generic Desktop)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1); 0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; // Generic desktop controls

        // USAGE (Pointer)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // LOGICAL_MINIMUM (-32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(2); //0x16;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(this->getAxesMin());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(this->getAxesMin());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;		// Use these two lines for 0 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		    //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;	// Use these two lines for -32767 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

        // LOGICAL_MAXIMUM (+32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(2);//0x26;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(this->getAxesMax());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(this->getAxesMax());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;	// Use these two lines for 255 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		    //tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;	// Use these two lines for +32767 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

        // REPORT_SIZE (16)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

        // REPORT_COUNT (this->getAxisCount())
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getAxisCount();

        // COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = COLLECTION(1); //0xA1;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        if (this->getIncludeXAxis())
        {
            // USAGE (X)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30;
        }

        if (this->getIncludeYAxis())
        {
            // USAGE (Y)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x31;
        }

        if (this->getIncludeZAxis())
        {
            // USAGE (Z)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x32;
        }

        if (this->getIncludeRzAxis())
        {
            // USAGE (Rz)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
        }

        if (this->getIncludeRxAxis())
        {
            // USAGE (Rx)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x33;
        }

        if (this->getIncludeRyAxis())
        {
            // USAGE (Ry)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x34;
        }

        if (this->getIncludeSlider1())
        {
            // USAGE (Slider)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x36;
        }

        if (this->getIncludeSlider2())
        {
            // USAGE (Slider)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x36;
        }

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); //0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        // END_COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = END_COLLECTION(0); //0xc0;

    } // X, Y, Z, Rx, Ry, and Rz Axis

    if (this->getSimulationCount() > 0)
    {
        // USAGE_PAGE (Simulation Controls)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1); //0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        // LOGICAL_MINIMUM (-32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(2); //0x16;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(this->getSimulationMin());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(this->getSimulationMin());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;		// Use these two lines for 0 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		//tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;	    // Use these two lines for -32767 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

        // LOGICAL_MAXIMUM (+32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(2); //0x26;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(this->getSimulationMax());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(this->getSimulationMax());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;	    // Use these two lines for 255 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		//tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;		// Use these two lines for +32767 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

        // REPORT_SIZE (16)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

        // REPORT_COUNT (this->getSimulationCount())
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getSimulationCount();

        // COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = COLLECTION(1); //0xA1;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        if (this->getIncludeRudder())
        {
            // USAGE (Rudder)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBA;
        }

        if (this->getIncludeThrottle())
        {
            // USAGE (Throttle)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBB;
        }

        if (this->getIncludeAccelerator())
        {
            // USAGE (Accelerator)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC4;
        }

        if (this->getIncludeBrake())
        {
            // USAGE (Brake)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC5;
        }

        if (this->getIncludeSteering())
        {
            // USAGE (Steering)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); //0x09;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC8;
        }

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); 0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        // END_COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = END_COLLECTION(0); //0xc0;

    } // Simulation Controls

    if (this->getHatSwitchCount() > 0)
    {

        // COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = COLLECTION(1); //0xA1;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // USAGE_PAGE (Generic Desktop)
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE (Hat Switch)
        for (int currentHatIndex = 0; currentHatIndex < this->getHatSwitchCount(); currentHatIndex++)
        {
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1);
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x39;
        }

        // Logical Min (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(1); //0x15;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // Logical Max (8)
        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(1); //0x25;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

        // Physical Min (0)
        tempHidReportDescriptor[hidReportDescriptorSize++] = PHYSICAL_MINIMUM(1); //0x35;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // Physical Max (315)
        tempHidReportDescriptor[hidReportDescriptorSize++] = PHYSICAL_MAXIMUM(2); //0x46;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3B;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // Unit (SI Rot : Ang Pos)
        tempHidReportDescriptor[hidReportDescriptorSize++] = UNIT(1); //0x65;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x12;

        // Report Size (8)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1); //0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

        // Report Count (4)
        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1); //0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getHatSwitchCount();

        // Input (Data, Variable, Absolute)
        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1); //0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x42;

        // END_COLLECTION (Physical)
        tempHidReportDescriptor[hidReportDescriptorSize++] = END_COLLECTION(0); //0xc0;
    }

    // End gamepad collection
    tempHidReportDescriptor[hidReportDescriptorSize++] = END_COLLECTION(0); //0xc0;

    if(hidReportDescriptorSize < size){
        memcpy(buffer, tempHidReportDescriptor, hidReportDescriptorSize);
    } else {
        ESP_LOGE("HID Report Descriptor size is larger than provided buffer size");
    }
}


uint8_t GamepadConfiguration::getTotalSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESPECIALBUTTONS; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getDesktopSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < 3; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getConsumerSpecialButtonCount()
{
    int count = 0;
    for (int i = 3; i < 8; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getAxisCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLEAXES; i++)
    {
        count += (int)_whichAxes[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getSimulationCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESIMULATIONCONTROLS; i++)
    {
        count += (int)_whichSimulationControls[i];
    }

    return count;
}

uint16_t GamepadConfiguration::getButtonCount() { return _buttonCount; }
uint8_t GamepadConfiguration::getHatSwitchCount() { return _hatSwitchCount; }
int16_t GamepadConfiguration::getAxesMin(){ return _axesMin; }
int16_t GamepadConfiguration::getAxesMax(){ return _axesMax; }
int16_t GamepadConfiguration::getSimulationMin(){ return _simulationMin; }
int16_t GamepadConfiguration::getSimulationMax(){ return _simulationMax; }
uint8_t GamepadConfiguration::getControllerType() { return _controllerType; }
bool GamepadConfiguration::getIncludeStart() { return _whichSpecialButtons[START_BUTTON]; }
bool GamepadConfiguration::getIncludeSelect() { return _whichSpecialButtons[SELECT_BUTTON]; }
bool GamepadConfiguration::getIncludeMenu() { return _whichSpecialButtons[MENU_BUTTON]; }
bool GamepadConfiguration::getIncludeHome() { return _whichSpecialButtons[HOME_BUTTON]; }
bool GamepadConfiguration::getIncludeBack() { return _whichSpecialButtons[BACK_BUTTON]; }
bool GamepadConfiguration::getIncludeVolumeInc() { return _whichSpecialButtons[VOLUME_INC_BUTTON]; }
bool GamepadConfiguration::getIncludeVolumeDec() { return _whichSpecialButtons[VOLUME_DEC_BUTTON]; }
bool GamepadConfiguration::getIncludeVolumeMute() { return _whichSpecialButtons[VOLUME_MUTE_BUTTON]; }
const bool *GamepadConfiguration::getWhichSpecialButtons() const { return _whichSpecialButtons; }
bool GamepadConfiguration::getIncludeXAxis() { return _whichAxes[X_AXIS]; }
bool GamepadConfiguration::getIncludeYAxis() { return _whichAxes[Y_AXIS]; }
bool GamepadConfiguration::getIncludeZAxis() { return _whichAxes[Z_AXIS]; }
bool GamepadConfiguration::getIncludeRxAxis() { return _whichAxes[RX_AXIS]; }
bool GamepadConfiguration::getIncludeRyAxis() { return _whichAxes[RY_AXIS]; }
bool GamepadConfiguration::getIncludeRzAxis() { return _whichAxes[RZ_AXIS]; }
bool GamepadConfiguration::getIncludeSlider1() { return _whichAxes[SLIDER1]; }
bool GamepadConfiguration::getIncludeSlider2() { return _whichAxes[SLIDER2]; }
const bool *GamepadConfiguration::getWhichAxes() const { return _whichAxes; }
bool GamepadConfiguration::getIncludeRudder() { return _whichSimulationControls[RUDDER]; }
bool GamepadConfiguration::getIncludeThrottle() { return _whichSimulationControls[THROTTLE]; }
bool GamepadConfiguration::getIncludeAccelerator() { return _whichSimulationControls[ACCELERATOR]; }
bool GamepadConfiguration::getIncludeBrake() { return _whichSimulationControls[BRAKE]; }
bool GamepadConfiguration::getIncludeSteering() { return _whichSimulationControls[STEERING]; }
const bool *GamepadConfiguration::getWhichSimulationControls() const { return _whichSimulationControls; }

void GamepadConfiguration::setWhichSpecialButtons(bool start, bool select, bool menu, bool home, bool back, bool volumeInc, bool volumeDec, bool volumeMute)
{
    _whichSpecialButtons[START_BUTTON] = start;
    _whichSpecialButtons[SELECT_BUTTON] = select;
    _whichSpecialButtons[MENU_BUTTON] = menu;
    _whichSpecialButtons[HOME_BUTTON] = home;
    _whichSpecialButtons[BACK_BUTTON] = back;
    _whichSpecialButtons[VOLUME_INC_BUTTON] = volumeInc;
    _whichSpecialButtons[VOLUME_DEC_BUTTON] = volumeDec;
    _whichSpecialButtons[VOLUME_MUTE_BUTTON] = volumeMute;
}

void GamepadConfiguration::setWhichAxes(bool xAxis, bool yAxis, bool zAxis, bool rxAxis, bool ryAxis, bool rzAxis, bool slider1, bool slider2)
{
    _whichAxes[X_AXIS] = xAxis;
    _whichAxes[Y_AXIS] = yAxis;
    _whichAxes[Z_AXIS] = zAxis;
    _whichAxes[RX_AXIS] = rxAxis;
    _whichAxes[RY_AXIS] = ryAxis;
    _whichAxes[RZ_AXIS] = rzAxis;
    _whichAxes[SLIDER1] = slider1;
    _whichAxes[SLIDER2] = slider2;
}

void GamepadConfiguration::setWhichSimulationControls(bool rudder, bool throttle, bool accelerator, bool brake, bool steering)
{
    _whichSimulationControls[RUDDER] = rudder;
    _whichSimulationControls[THROTTLE] = throttle;
    _whichSimulationControls[ACCELERATOR] = accelerator;
    _whichSimulationControls[BRAKE] = brake;
    _whichSimulationControls[STEERING] = steering;
}

void GamepadConfiguration::setControllerType(uint8_t value) { _controllerType = value; }

void GamepadConfiguration::setButtonCount(uint16_t value) { _buttonCount = value; }
void GamepadConfiguration::setHatSwitchCount(uint8_t value) { _hatSwitchCount = value; }
void GamepadConfiguration::setIncludeStart(bool value) { _whichSpecialButtons[START_BUTTON] = value; }
void GamepadConfiguration::setIncludeSelect(bool value) { _whichSpecialButtons[SELECT_BUTTON] = value; }
void GamepadConfiguration::setIncludeMenu(bool value) { _whichSpecialButtons[MENU_BUTTON] = value; }
void GamepadConfiguration::setIncludeHome(bool value) { _whichSpecialButtons[HOME_BUTTON] = value; }
void GamepadConfiguration::setIncludeBack(bool value) { _whichSpecialButtons[BACK_BUTTON] = value; }
void GamepadConfiguration::setIncludeVolumeInc(bool value) { _whichSpecialButtons[VOLUME_INC_BUTTON] = value; }
void GamepadConfiguration::setIncludeVolumeDec(bool value) { _whichSpecialButtons[VOLUME_DEC_BUTTON] = value; }
void GamepadConfiguration::setIncludeVolumeMute(bool value) { _whichSpecialButtons[VOLUME_MUTE_BUTTON] = value; }
void GamepadConfiguration::setIncludeXAxis(bool value) { _whichAxes[X_AXIS] = value; }
void GamepadConfiguration::setIncludeYAxis(bool value) { _whichAxes[Y_AXIS] = value; }
void GamepadConfiguration::setIncludeZAxis(bool value) { _whichAxes[Z_AXIS] = value; }
void GamepadConfiguration::setIncludeRxAxis(bool value) { _whichAxes[RX_AXIS] = value; }
void GamepadConfiguration::setIncludeRyAxis(bool value) { _whichAxes[RY_AXIS] = value; }
void GamepadConfiguration::setIncludeRzAxis(bool value) { _whichAxes[RZ_AXIS] = value; }
void GamepadConfiguration::setIncludeSlider1(bool value) { _whichAxes[SLIDER1] = value; }
void GamepadConfiguration::setIncludeSlider2(bool value) { _whichAxes[SLIDER2] = value; }
void GamepadConfiguration::setIncludeRudder(bool value) { _whichSimulationControls[RUDDER] = value; }
void GamepadConfiguration::setIncludeThrottle(bool value) { _whichSimulationControls[THROTTLE] = value; }
void GamepadConfiguration::setIncludeAccelerator(bool value) { _whichSimulationControls[ACCELERATOR] = value; }
void GamepadConfiguration::setIncludeBrake(bool value) { _whichSimulationControls[BRAKE] = value; }
void GamepadConfiguration::setIncludeSteering(bool value) { _whichSimulationControls[STEERING] = value; }

void GamepadConfiguration::setAxesMin(int16_t value) { _axesMin = value; }
void GamepadConfiguration::setAxesMax(int16_t value) { _axesMax = value; }
void GamepadConfiguration::setSimulationMin(int16_t value) { _simulationMin = value; }
void GamepadConfiguration::setSimulationMax(int16_t value) { _simulationMax = value; }

void GamepadConfiguration::getButtonPaddingBits()
{
    uint8_t buttonPaddingBits = 8 - (this->getButtonCount() % 8);
    if (buttonPaddingBits == 8)
    {
        buttonPaddingBits = 0;
    }
    return buttonPaddingBits;
}