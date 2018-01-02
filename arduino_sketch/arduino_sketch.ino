/**
 * @file arduino_sketch.ino
 * @author cwee
 * @brief Car alarm that monitors generic signals.
 */

typedef enum { LED_OFF, LED_SLOW, LED_FAST } led_state_t;

/* IO mapping */
#define DOOR_LOCK_PIN       23
#define DOOR_UNLOCK_PIN     24
#define DOOR_OPEN_PIN       25
#define GLOVEBOX_LIGHT_PIN  26
#define ACCESS_POWER_PIN    27
#define SIREN_PIN           28
#define STATUS_LED_PIN      LED_BUILTIN

/* state variables */
int car_locked = 0;
led_state_t led_state = LED_OFF;
unsigned int led_on = 0;
unsigned long last_led_toggle = 0;

void setup(void)
{
    /* setup IO */
    pinMode(DOOR_LOCK_PIN, INPUT);
    pinMode(DOOR_UNLOCK_PIN, INPUT);
    pinMode(DOOR_OPEN_PIN, INPUT);
    pinMode(GLOVEBOX_LIGHT_PIN, INPUT);
    pinMode(ACCESS_POWER_PIN, INPUT);
    pinMode(SIREN_PIN, OUTPUT);
    pinMode(STATUS_LED_PIN, OUTPUT);

    /* set initial state */
    car_locked = 0;
    led_state = LED_OFF;
    led_on = 0;
    last_led_toggle = 0;
}


void loop(void)
{
    int door_lock, door_unlock, door_open, glovebox_light, access_power;
    unsigned long now, led_delay;

    door_lock = !digitalRead(DOOR_LOCK_PIN);            // active low
    door_unlock = !digitalRead(DOOR_UNLOCK_PIN);        // active low
    door_open = !digitalRead(DOOR_OPEN_PIN);            // active low
    glovebox_light = digitalRead(GLOVEBOX_LIGHT_PIN);   // TODO: check logic voltage
    access_power = digitalRead(ACCESS_POWER_PIN);       // TODO: check logic voltage

    /* alarm logic */
    if (car_locked) {
        if (door_open || glovebox_light) {
            led_state = LED_FAST;
            digitalWrite(SIREN_PIN, HIGH);
        }

        if (door_unlock) {
            led_state = LED_OFF;
            car_locked = 0;
        }

        /* manual override - requires key */
        if (access_power) {
            led_state = LED_OFF;
            car_locked = 0;
            digitalWrite(SIREN_PIN, LOW); 
        }

    } else if (door_lock) {
        led_state = LED_SLOW;
        car_locked = 1;
    }


    /* handle blinking LED */
    now = millis();
    if (led_state == LED_OFF) {
        if (led_on) {
            led_on = 0;
            digitalWrite(STATUS_LED_PIN, led_on);
        }

    } else {
        if (led_state == LED_SLOW)
            led_delay = 666;
        else if (led_state == LED_FAST)
            led_delay = 333;

        if (now - last_led_toggle > led_delay) {
            led_on = !led_on;
            digitalWrite(STATUS_LED_PIN, led_on);
            last_led_toggle = now;
        }
    }
}
