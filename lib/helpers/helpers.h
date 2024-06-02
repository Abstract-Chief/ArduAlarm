/**
 * @brief Prints formatted string (including floating point variables) to serial line.
 *
 * @tparam T
 * @param str format string
 * @param args list of arguments to print
 */
template <typename... T>
void print(const char *str, T... args);
