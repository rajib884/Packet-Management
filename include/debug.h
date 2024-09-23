#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
    #define debug(fmt, ...) printf("%s:%d %s => " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#else
    #define debug(fmt, ...)
#endif

#ifdef DEBUG
    #define if_debug_call(func, ...) func(__VA_ARGS__)
#else
    #define if_debug_call(func, ...)
#endif

#endif /* __DEBUG_H__ */