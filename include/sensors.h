/**
 * Sensors initialization.
 */
void sensors_init();


/**
 * Get the temperature
 *
 * @return The temperature in °C.
 */
float get_temperature();


/**
 * Get the humidity
 *
 * @return Value of humidity as integer in the range from 0 (dry) to 100 (wet).
 */
int get_humidity();
