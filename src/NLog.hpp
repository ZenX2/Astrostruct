/**
 * @file NLog.hpp
 * @brief Handles error messages, info messages, and warning messages.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-07-06
 */
#ifndef NAELSTROF_LOG
#define NAELSTROF_LOG

/**
 * @brief Handles messages and writes them to a file.
 */
class NLog
{
public:
    NLog();
    ~NLog();
    void Send(std::string System, unsigned int Level, std::string Message);
    void Log(std::string Message);
private:
    std::ofstream File;
};

#endif
