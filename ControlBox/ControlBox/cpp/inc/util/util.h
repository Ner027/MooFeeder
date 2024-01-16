#ifndef FEEDINGSTATION_UTIL_H
#define FEEDINGSTATION_UTIL_H

/// \brief This function is a custom version of the "printf" function, mainly used for logging purposes
/// \param func Tag to place in the beginning of the log
/// \param format printf like formatters
/// \param ... Variadic arguments
void print(const char *func, const char *format, ...);

#endif
