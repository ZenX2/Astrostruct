/**
 * @file NSignal.hpp
 * @brief Intercepts signals and reacts to them, so we don't exit uncleanly on sigterms and don't immediately crash on segfaults.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-29
 */
#ifndef NAELSTROF_SIGNAL
#define NAELSTROF_SIGNAL

/**
 * @brief Takes a signal and reacts to it accordignly.
 *
 * @param Signal The signal.
 */
void SignalHandler(int Signal);
/**
 * @brief Converts a signal(int) into a human-readable string.
 *
 * @param Signal The signal.
 *
 * @return A human-readable signal.
 */
std::string SignalToString(int Signal);
/**
 * @brief Class used to initialize and deinitialize the signal interceptor.
 */
class NSignalInterceptor
{
public:
    /**
     * @brief Reroutes all signals to SignalHandler(int).
     */
    NSignalInterceptor();
    /**
     * @brief Routes all signals back to their default functions.
     */
    ~NSignalInterceptor();
};

#endif
