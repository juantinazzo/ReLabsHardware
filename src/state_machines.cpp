#include "state_machines.h"
#include <ESP_FlexyStepper.h>
#include "board.h"
#include <ESP32Servo.h>

static Servo servo;

static bool should_start=false;
static int mass_index=0;
static float altura=0, angulo=0, tiempo_maximo=0;

extern unsigned long _start_time;

static int estado=1;


#define ALTURA_CERO 75
#define ALTURA_HOLD_ 25
#define ALTURA_MAX_STEPS
#define RELAY_PIN -1
#define MM_TO_STEPS -100
#define ANG_TO_STEPS 530

// IO pin assignments
const int MOTOR_STEP_PIN = 3;
const int MOTOR_DIRECTION_PIN = 4;

extern int index_use_array;
extern int index_array_pos;

// create the stepper motor object
ESP_FlexyStepper height_stepper, angle_stepper;

bool rotate_to_mass(int _mass_index){
    return true;
}

void state_machines_trigger_start(int _mass_index, float _altura,float _angulo,float _tiempo_maximo){
   should_start=true; 
   altura=_altura;
   angulo=_angulo;
   tiempo_maximo=_tiempo_maximo;
   mass_index=_mass_index;
}

bool state_machines_has_ended(){
 return (estado==PENDULO_STOPPED);
}

bool state_machines_pendulum_running(){
 return (estado==PENDULO_WAITING_TEST);
}

void pendulo_task(void*){
    
    static unsigned long cheap_timer=0;


    height_stepper.connectToPins(SLOT0_CS0, SLOT0_CS1);


    height_stepper.setSpeedInMillimetersPerSecond(20);
    height_stepper.setAccelerationInMillimetersPerSecondPerSecond(1000);
    height_stepper.setDecelerationInMillimetersPerSecondPerSecond(1000);
    height_stepper.startAsService(1);

    angle_stepper.connectToPins(SLOT1_CS0, SLOT1_CS1);
    angle_stepper.setSpeedInMillimetersPerSecond(100);
    angle_stepper.setAccelerationInMillimetersPerSecondPerSecond(1000);
    angle_stepper.setDecelerationInMillimetersPerSecondPerSecond(1000);
    angle_stepper.startAsService(1);

    servo.setPeriodHertz(50);
    servo.attach(SPARE_IO0, 1000, 2000);



    while(true){
        switch(estado){
            case PENDULO_STOPPED:
            if(should_start){
                should_start=false;
                estado=PENDULO_ROTATING_DISC;
                
                Serial.println("MEF: a PENDULO_ROTATING_DISC");
            }
            break;
            case PENDULO_ROTATING_DISC:
                if(rotate_to_mass(mass_index)){
                    estado=PENDULO_LOWERING_CRANE_INIT;
                    height_stepper.setTargetPositionInSteps(ALTURA_CERO*MM_TO_STEPS);
                    Serial.println("MEF: a PENDULO_LOWERING_CRANE_INIT");
                }
            break;
            case PENDULO_LOWERING_CRANE_INIT:
                if(height_stepper.getDistanceToTargetSigned() == 0){
                    estado=PENDULO_HOOK_CRANE;
                    //cheap_timer=millis();
                    //digitalWrite(RELAY_PIN, HIGH);
                    Serial.println("MEF: a PENDULO_HOOK_CRANE");
                }
            break;
            case PENDULO_HOOK_CRANE:
                if(millis()>cheap_timer+1000){
                    //digitalWrite(RELAY_PIN, LOW);
                    estado=PENDULO_RISING_CRANE_INIT;
                    height_stepper.setTargetPositionInSteps(altura*MM_TO_STEPS);
                    Serial.println("MEF: a PENDULO_RISING_CRANE_INIT");
                }
            break;
            case PENDULO_RISING_CRANE_INIT:
                if(height_stepper.getDistanceToTargetSigned() == 0){
                    estado=PENDULO_CLOSING_DOOR;
                    servo.write(90);
                    cheap_timer=millis();
                    //digitalWrite(RELAY_PIN, HIGH);
                    Serial.println("MEF: a PENDULO_CLOSING_DOOR");
                }
            break;
            case PENDULO_CLOSING_DOOR:
                if(millis()>cheap_timer+500){
                    estado=PENDULO_RISING_LEVER;
                    angle_stepper.setTargetPositionInSteps(angulo*ANG_TO_STEPS);
                    Serial.println("MEF: a PENDULO_RISING_LEVER");
                }
            break;
            case PENDULO_RISING_LEVER:
                if(angle_stepper.getDistanceToTargetSigned() == 0){
                    estado=PENDULO_RELEASE_LEVER;
                    servo.write(0);
                    cheap_timer=millis();
                    //digitalWrite(RELAY_PIN, HIGH);
                    Serial.println("MEF: a PENDULO_RELEASE_LEVER");
                }
            break;
            case PENDULO_RELEASE_LEVER:
                if(millis()>cheap_timer+500){
                    estado=PENDULO_WAITING_TEST;
                    angle_stepper.setTargetPositionInSteps(0);
                    cheap_timer=millis();
                    _start_time=millis();
                    index_array_pos=0;
                    index_use_array=index_use_array==0?1:0;
                    Serial.println("MEF: a PENDULO_WAITING_TEST");
                }
            break;
            case PENDULO_WAITING_TEST:
                if(millis()>cheap_timer+tiempo_maximo*1000){
                    servo.write(90);
                    estado=PENDULO_BRAKING;
                     cheap_timer=millis();
                    Serial.println("MEF: a PENDULO_BRAKING");
                }
            break;
            case PENDULO_BRAKING:
                if(millis()>cheap_timer+1000){
                    height_stepper.setTargetPositionInSteps(ALTURA_CERO*MM_TO_STEPS);
                    estado=PENDULO_LOWERING_CRANE_END;
                    Serial.println("MEF: a PENDULO_LOWERING_CRANE_END");
                }
            break;
            case PENDULO_LOWERING_CRANE_END:
                if(height_stepper.getDistanceToTargetSigned() == 0){
                    estado=PENDULO_UNHOOK_CRANE;
                    cheap_timer=millis();
                    digitalWrite(RELAY_PIN, HIGH);
                    Serial.println("MEF: a PENDULO_UNHOOK_CRANE");
                }
            break;
            case PENDULO_UNHOOK_CRANE:
                if(millis()>cheap_timer+1000){
                    height_stepper.setTargetPositionInSteps(ALTURA_HOLD_*MM_TO_STEPS);
                    estado=PENDULO_RISING_CRANE_END;
                    Serial.println("MEF: a PENDULO_RISING_CRANE_END");
                }
            break;
            case PENDULO_RISING_CRANE_END:
                if(height_stepper.getDistanceToTargetSigned() == 0){
                    estado=PENDULO_STOPPED;
                    digitalWrite(RELAY_PIN, LOW);
                    Serial.println("MEF: a PENDULO_STOPPED");
                }
            break;
        }
        delay(50);
    }
}