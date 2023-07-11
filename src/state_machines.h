#ifndef STATE_MACHINES_H
#define STATE_MACHINES_H

#include <Arduino.h>

typedef enum{
    PENDULO_STOPPED=1,
    PENDULO_ROTATING_DISC,
    PENDULO_LOWERING_CRANE_INIT,
    PENDULO_HOOK_CRANE,
    PENDULO_RISING_CRANE_INIT,
    PENDULO_CLOSING_DOOR,
    PENDULO_RISING_LEVER,
    PENDULO_RELEASE_LEVER,
    PENDULO_WAITING_TEST,
    PENDULO_BRAKING,
    PENDULO_LOWERING_CRANE_END,
    PENDULO_UNHOOK_CRANE,
    PENDULO_RISING_CRANE_END
} pendulo_states;

void state_machines_trigger_start(int _mass_index, float _altura,float _angulo,float _tiempo_maximo);
bool state_machines_has_ended();
void pendulo_task(void*);
bool state_machines_has_ended();
bool state_machines_pendulum_running();

#endif