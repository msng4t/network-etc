#ifndef _MINA_COMMON_H_INCLUDE_
#define _MINA_COMMON_H_INCLUDE_

#define MINA_NAMESPACE_START namespace mina{
#define MINA_NAMESPACE_END   }
#define  MINA_OK          0
#define  MINA_ERROR      -1
#define  MINA_AGAIN      -2
#define  MINA_BUSY       -3
#define  MINA_DONE       -4
#define  MINA_DECLINED   -5
#define  MINA_ABORT      -6


#define MINA_RETRY(op, num)    \
{                                               \
    int retries = num <= 0 ? INT32_MAX : num;   \
    for(;;)                                     \
    {                                           \
        errno = 0;                              \
        if (!(op))                              \
        {                                       \
            break;                              \
        }                                       \
        if (errno == EINTR && --retries > 0)    \
            continue;                           \
        break;                                  \
    }                                           \
}
#define MAX_NAME_LEN 128
#define MINA_TIME_SEC  1000000
#define MINA_TIME_MSEC 1000
#define MINA_TIME_NSEC 1

#define INT8_MAX 0x7f
#define INT8_MIN (-INT8_MAX - 1)
#define UINT8_MAX (__CONCAT(INT8_MAX, U) * 2U + 1U)
#define INT16_MAX 0x7fff
#define INT16_MIN (-INT16_MAX - 1)
#define UINT16_MAX (__CONCAT(INT16_MAX, U) * 2U + 1U)
#define INT32_MAX 0x7fffffffL
#define INT32_MIN (-INT32_MAX - 1L)
#define UINT32_MAX (__CONCAT(INT32_MAX, U) * 2UL + 1UL)
#define INT64_MAX 0x7fffffffffffffffLL
#define INT64_MIN (-INT64_MAX - 1LL)
#define UINT64_MAX (__CONCAT(INT64_MAX, U) * 2ULL + 1ULL)
//Limits of minimum-width integer types

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST8_MIN INT8_MIN
#define UINT_LEAST8_MAX UINT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST16_MIN INT16_MIN
#define UINT_LEAST16_MAX UINT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST32_MIN INT32_MIN
#define UINT_LEAST32_MAX UINT32_MAX
#define INT_LEAST64_MAX INT64_MAX
#define INT_LEAST64_MIN INT64_MIN
#define UINT_LEAST64_MAX UINT64_MAX

#define MINA_TRYLOCK(lock)    (((*lock) == 0) && MinaAtomicCmpSet(lock, 0, 1))
#define MINA_UNLOCK(lock)      ((*lock) = 0)
#define MINA_UPDATE_TIME           1
#define MINA_POST_EVENTS           2
#define MINA_POST_THREAD_EVENTS    4


#define MINA_INET_ADDRSTRLEN (sizeof("255.255.255.255") + 1)
#define MINA_INET6_ADDRSTRLEN (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") - 1)
#define MINA_SOCKADDR_STRLEN (MINA_INET_ADDRSTRLEN + sizeof(":65535"))
#define MINA_SOCKADDRLEN       512



#define GETEXARRYENTRY(type, cur_size, cur_max, cur_ptr, array, result)    \
do{                                                                          \
    type* new_ptr;                                                         \
                                                                           \
    if (cur_size < cur_max)                                                \
    {                                                                       \
        result = &cur_ptr[cur_size++];                                        \
        break;                                                             \
    }                                                                       \
                                                                           \
    cur_max *= 2;                                                          \
    new_ptr = new type[cur_max];                                           \
    memcpy(new_ptr, cur_ptr, cur_size * sizeof(type));                     \
                                                                           \
    if (cur_ptr != array)                                                  \
        delete[] cur_ptr;                                                  \
                                                                           \
    cur_ptr = new_ptr;                                                     \
                                                                           \
    result = &cur_ptr[cur_size++];                                          \
}while (false);

#endif
