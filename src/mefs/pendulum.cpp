#include <Arduino.h>

#include "utilities/Logger.h"
#include "systemManager.h"

typedef enum
{
    STOP,
    PREPARE,
    OSC,
    RESET
} main_pend_mef_enum;

typedef enum
{
    SELECT_MASS_p,
    LOWER_MASS_p,
    RAISE_MASS_p,
    LAUNCH_ANGLE_p
} prepare_pend_mef_enum;

typedef enum
{
    STOP_MASS_r,
    LOWER_MASS_r,
    WAIT_r,
    RAISE_MASS_r
} reset_pend_mef_enum;


void main_pend_mef(){
    static main_pend_mef_enum main_state = STOP;

    switch (main_state){
        case STOP:
            break;
    }
}