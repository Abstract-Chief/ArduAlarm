/**
 * Sensors initialization.
 */
#include<DHT.h>
#include<lcd_wrapper.h>
#define DHTTYPE DHT11
#define DHTPIN 6

DHT dht(DHTPIN,DHTTYPE);
void sensors_init(){
    dht.begin();
    Serial.println("dht init");
}
/**
 * Get the temperature
 *
 * @return The temperature in °C.
 */
float LAST_T=0;
float get_temperature(){
    float t = dht.readTemperature(false,true);
    return (isnan(t) ? LAST_T : t);
}


/**
 * Get the humidity
 *
 * @return Value of humidity as integer in the range from 0 (dry) to 100 (wet).
 */
int get_humidity(){
    float h = dht.readHumidity(false);
    return h;
}
