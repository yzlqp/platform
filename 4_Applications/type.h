/**
 * @file type.h
 * @author ylp
 * @brief
 * @version 0.1
 * @date 2021-06-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _MAC_TYPES_H
#define _MAC_TYPES_H

/* types */
typedef unsigned char            u8_t;
typedef unsigned char 			 ui8_t;
typedef unsigned char 			 uint8_t;
typedef unsigned short           u16_t;
typedef unsigned short 			 ui16_t;
typedef unsigned short 			 uint16_t;
typedef unsigned int             u32_t;
typedef unsigned int 			 ui32_t;
typedef unsigned int 			 uint32_t;

typedef signed char              i8_t;
typedef signed char              int8_t;
typedef signed short             i16_t;
typedef signed short             int16_t;
typedef signed int               i32_t;
typedef signed int               int32_t;

#if WORDSIZE == 64
typedef signed long int 		 i64_t;
typedef unsigned long int 		 ui64_t;
#else
typedef signed long long int     i64_t;
typedef signed long long int     int64_t;
typedef unsigned long long int   u64_t;
typedef unsigned long long int   ui64_t;
typedef unsigned long long int   uint64_t;
#endif

typedef float 					 fi32_t;
typedef double 					 fi64_t;

#endif /* _MAC_TYPES_H */

