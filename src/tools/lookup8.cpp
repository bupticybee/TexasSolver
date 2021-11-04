//
// Created by icybee&freezer on 2021/4/7.
//

/*
--------------------------------------------------------------------
lookup8.c, by Bob Jenkins, January 4 1997, Public Domain.
hash(), hash2(), hash3, and mix() are externally useful functions.
Routines to test the hash are included if SELF_TEST is defined.
You can use this free for any purpose.  It has no warranty.
2009: This is obsolete.  I recently timed lookup3.c as being faster
at producing 64-bit results.
--------------------------------------------------------------------
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "include/tools/lookup8.h"


ub8 hash1(ub1* k,ub8 length,ub8 level)
{
    ub8 a,b,c,len;

    /* Set up the internal state */
    len = length;
    a = b = level;                         /* the previous hash value */
    c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

    /*---------------------------------------- handle most of the key */
    while (len >= 24)
    {
        a += (k[0]        +((ub8)k[ 1]<< 8)+((ub8)k[ 2]<<16)+((ub8)k[ 3]<<24)
              +((ub8)k[4 ]<<32)+((ub8)k[ 5]<<40)+((ub8)k[ 6]<<48)+((ub8)k[ 7]<<56));
        b += (k[8]        +((ub8)k[ 9]<< 8)+((ub8)k[10]<<16)+((ub8)k[11]<<24)
              +((ub8)k[12]<<32)+((ub8)k[13]<<40)+((ub8)k[14]<<48)+((ub8)k[15]<<56));
        c += (k[16]       +((ub8)k[17]<< 8)+((ub8)k[18]<<16)+((ub8)k[19]<<24)
              +((ub8)k[20]<<32)+((ub8)k[21]<<40)+((ub8)k[22]<<48)+((ub8)k[23]<<56));
        mix64(a,b,c);
        k += 24; len -= 24;
    }

    /*------------------------------------- handle the last 23 bytes */
    c += length;
    switch(len)              /* all the case statements fall through */
    {
        case 23: c+=((ub8)k[22]<<56);
        case 22: c+=((ub8)k[21]<<48);
        case 21: c+=((ub8)k[20]<<40);
        case 20: c+=((ub8)k[19]<<32);
        case 19: c+=((ub8)k[18]<<24);
        case 18: c+=((ub8)k[17]<<16);
        case 17: c+=((ub8)k[16]<<8);
            /* the first byte of c is reserved for the length */
        case 16: b+=((ub8)k[15]<<56);
        case 15: b+=((ub8)k[14]<<48);
        case 14: b+=((ub8)k[13]<<40);
        case 13: b+=((ub8)k[12]<<32);
        case 12: b+=((ub8)k[11]<<24);
        case 11: b+=((ub8)k[10]<<16);
        case 10: b+=((ub8)k[ 9]<<8);
        case  9: b+=((ub8)k[ 8]);
        case  8: a+=((ub8)k[ 7]<<56);
        case  7: a+=((ub8)k[ 6]<<48);
        case  6: a+=((ub8)k[ 5]<<40);
        case  5: a+=((ub8)k[ 4]<<32);
        case  4: a+=((ub8)k[ 3]<<24);
        case  3: a+=((ub8)k[ 2]<<16);
        case  2: a+=((ub8)k[ 1]<<8);
        case  1: a+=((ub8)k[ 0]);
            /* case 0: nothing left to add */
    }
    mix64(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}

/*
--------------------------------------------------------------------
 This works on all machines, is identical to hash() on little-endian
 machines, and it is much faster than hash(), but it requires
 -- that the key be an array of ub8's, and
 -- that all your machines have the same endianness, and
 -- that the length be the number of ub8's in the key
--------------------------------------------------------------------
*/
ub8 hash2(ub8* k,ub8 length,ub8 level)
{
    ub8 a,b,c,len;

    /* Set up the internal state */
    len = length;
    a = b = level;                         /* the previous hash value */
    c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

    /*---------------------------------------- handle most of the key */
    while (len >= 3)
    {
        a += k[0];
        b += k[1];
        c += k[2];
        mix64(a,b,c);
        k += 3; len -= 3;
    }

    /*-------------------------------------- handle the last 2 ub8's */
    c += (length<<3);
    switch(len)              /* all the case statements fall through */
    {
        /* c is reserved for the length */
        case  2: b+=k[1];
        case  1: a+=k[0];
            /* case 0: nothing left to add */
    }
    mix64(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}

/*
--------------------------------------------------------------------
 This is identical to hash() on little-endian machines, and it is much
 faster than hash(), but a little slower than hash2(), and it requires
 -- that all your machines be little-endian, for example all Intel x86
    chips or all VAXen.  It gives wrong results on big-endian machines.
--------------------------------------------------------------------
*/

ub8 hash3(ub1* k,ub8 length,ub8 level)
{
    ub8 a,b,c,len;

    /* Set up the internal state */
    len = length;
    a = b = level;                         /* the previous hash value */
    c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

    /*---------------------------------------- handle most of the key */
    if (((size_t)k)&7)
    {
        while (len >= 24)
        {
            a += (k[0]        +((ub8)k[ 1]<< 8)+((ub8)k[ 2]<<16)+((ub8)k[ 3]<<24)
                  +((ub8)k[4 ]<<32)+((ub8)k[ 5]<<40)+((ub8)k[ 6]<<48)+((ub8)k[ 7]<<56));
            b += (k[8]        +((ub8)k[ 9]<< 8)+((ub8)k[10]<<16)+((ub8)k[11]<<24)
                  +((ub8)k[12]<<32)+((ub8)k[13]<<40)+((ub8)k[14]<<48)+((ub8)k[15]<<56));
            c += (k[16]       +((ub8)k[17]<< 8)+((ub8)k[18]<<16)+((ub8)k[19]<<24)
                  +((ub8)k[20]<<32)+((ub8)k[21]<<40)+((ub8)k[22]<<48)+((ub8)k[23]<<56));
            mix64(a,b,c);
            k += 24; len -= 24;
        }
    }
    else
    {
        while (len >= 24)    /* aligned */
        {
            a += *(ub8 *)(k+0);
            b += *(ub8 *)(k+8);
            c += *(ub8 *)(k+16);
            mix64(a,b,c);
            k += 24; len -= 24;
        }
    }

    /*------------------------------------- handle the last 23 bytes */
    c += length;
    switch(len)              /* all the case statements fall through */
    {
        case 23: c+=((ub8)k[22]<<56);
        case 22: c+=((ub8)k[21]<<48);
        case 21: c+=((ub8)k[20]<<40);
        case 20: c+=((ub8)k[19]<<32);
        case 19: c+=((ub8)k[18]<<24);
        case 18: c+=((ub8)k[17]<<16);
        case 17: c+=((ub8)k[16]<<8);
            /* the first byte of c is reserved for the length */
        case 16: b+=((ub8)k[15]<<56);
        case 15: b+=((ub8)k[14]<<48);
        case 14: b+=((ub8)k[13]<<40);
        case 13: b+=((ub8)k[12]<<32);
        case 12: b+=((ub8)k[11]<<24);
        case 11: b+=((ub8)k[10]<<16);
        case 10: b+=((ub8)k[ 9]<<8);
        case  9: b+=((ub8)k[ 8]);
        case  8: a+=((ub8)k[ 7]<<56);
        case  7: a+=((ub8)k[ 6]<<48);
        case  6: a+=((ub8)k[ 5]<<40);
        case  5: a+=((ub8)k[ 4]<<32);
        case  4: a+=((ub8)k[ 3]<<24);
        case  3: a+=((ub8)k[ 2]<<16);
        case  2: a+=((ub8)k[ 1]<<8);
        case  1: a+=((ub8)k[ 0]);
            /* case 0: nothing left to add */
    }
    mix64(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}

