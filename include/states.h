/**
 * List of available states.
 */
enum state {
    ALARM,
    CLOCK,
    FACTORY_RESET,
    SHOW_DATE,
    SHOW_ENV
};


/**
 * Individual implementation of states.
 */

enum state state_clock();
enum state state_factory_reset();
enum state state_alarm();
enum state state_show_date();
enum state state_show_env();
