#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
    #define debug(fmt, ...) printf("%s:%d %s => " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#else
    #define debug(fmt, ...)
#endif

#endif /* __DEBUG_H__ */