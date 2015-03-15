#include "EV2_CAN.h"

void MC_setup(void)
{   
    // enable drive
    // 1. RFE (p37)
    pinMode(37, OUTPUT);
    digitalWrite(37, LOW);
    // 2. RFE (p35)
    pinMode(35, OUTPUT);
    digitalWrite(35, LOW);
    set_rfe_frg(true,true);

    // 3. tractive system shutdown relay
    pinMode(33, OUTPUT);
    digitalWrite(33, LOW);
    set_tracsys_relay(false);

    // hardware interrupts for inputs  
    // 1. battery fault (p45)
    pinMode(45, INPUT);
    attachInterrupt(0, inputChanged, CHANGE);
    // 2. isolation fault (p33)
    pinMode(33, INPUT);
    attachInterrupt(1, inputChanged, CHANGE);
    // 3. TSA (p45)
    pinMode(45, INPUT);
    attachInterrupt(2, inputChanged, CHANGE);

    inputChanged();
}

void request_temperatures(void)
{
    CAN_FRAME MCtempRequest;
    createMCTempRequestFrame(MCtempRequest);
    CAN.sendFrame(MCtempRequest); 

    CAN_FRAME MCmotortempRequest;
    createMotorTempRequestFrame(MCmotortempRequest);
    CAN.sendFrame(MCmotortempRequest);  

}

void request_MC_speed(void)
{
    CAN_FRAME speedRequest;
    int repetition = 100; // 100ms
    createSpeedRequestFrame(speedRequest,repetition);
    CAN.sendFrame(speedRequest);
}

void request_MC_torque(void)
{
    CAN_FRAME torqueRequest;
    int repetition = 100; // 100ms
    createTorqueRequestFrame(torqueRequest,repetition);
    CAN.sendFrame(torqueRequest);
}

void request_MC_current(void)
{
    CAN_FRAME currentRequest;
    int repetition = 100; // 100ms
    createCurrentRequestFrame(currentRequest,repetition);
    CAN.sendFrame(currentRequest);
}

void request_MC_voltage(void)
{
    CAN_FRAME voltageRequest;
    int repetition = 100; // 100ms
    createVoltageRequestFrame(voltageRequest,repetition);
    CAN.sendFrame(voltageRequest);
}

void setup() {
    
    MC_setup();

    Serial.begin(115200);
	CAN_setup();

    delay(100);
    request_MC_speed();
    delay(100);
    request_MC_torque();
    delay(100);
    request_MC_current();
    delay(100);
    request_MC_voltage();
    delay(100);

	// adc setup
	adc_setup();

    Timer3.attachInterrupt(checkBrake).setFrequency(2).start();

	//set up hardware interrupt for reading throttle
	// Timer3.attachInterrupt(sendThrottle).setFrequency(100).start();
    Timer4.attachInterrupt(request_temperatures).setFrequency(1).start();
    // Timer5.attachInterrupt(updateDB).setFrequency(1).start();

    Timer5.attachInterrupt(updateDB_Processing).setFrequency(10).start();
}

void loop() {

    CAN_FRAME incoming;

    if (CAN.rx_avail()) {
        CAN.get_rx_buff(incoming); 
        parseFrame(incoming);
    }
    if (CAN2.rx_avail()) {
        CAN2.get_rx_buff(incoming); 
        parseFrame(incoming);
    }

}