#ifdef _DM_DEBUG
#include <iostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define DEVIOUS_LOG(msg) std::cout << "[LOG] " << msg << std::endl;
#define DEVIOUS_ERR(msg) std::cout << RED << "[ERROR] " << msg << RESET << std::endl;
#define DEVIOUS_WARN(msg) std::cout << YELLOW << "[WARNING] " << msg << RESET << std::endl;
#define DEVIOUS_EVENT(msg) std::cout << GREEN << "[EVENT] " << msg << RESET << std::endl;

#else
#define DEVIOUS_LOG(msg)
#define DEVIOUS_ERR(msg)
#define DEVIOUS_WARN(msg)
#define DEVIOUS_EVENT(msg)
#endif