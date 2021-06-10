/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#ifndef HASHMAP_H
#define HASHMAP_H

#ifdef __HASHMAP_C
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <limits.h>

    #include <common/base_types.h>
    #include <common/cerr_def.h>
    #include <common/common.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __HashData {
    void *data;
    void *key;
    size_t key_len;
} __HashData;

typedef struct __Hashmap {
    __HashData *map_data;
    size_t *indices;
    size_t map_size;
    size_t used_size;
} Hashmap;

#ifdef __HASHMAP
    /* 
     * Hashing function for hashmapping that returns index
     * This function is based on mostly Jenkins one at time hashing algorihm
     * Steps to finding index are following:
     * 1. Find crc32_key from key data
     * 2. Perform Jenkins one at time bitwise operations
     * 3. Perform three other Jenkins operations
     * 4. Multiply bit-shifted out_key with constant 0x9E3779B1
     * 5. Return hash % n
     */
    static size_t __hashfunc (
        void *key, 
        size_t n_key, 
        size_t map_size
    ); 


    /* 
     * Reallocate more memory for buckets in hashmap 
     */
    static void __reallocateHashmap(Hashmap *p_hm);

    
    /*
     * Key comparisson method
     * Returns 0 if keys are equal, 1 if key1 is longer than key2 and -1 if 
     * key2 is longer than key1 then returns 1 and 2 if keys are the same length
     * but their memory areas do not match
     */
    static int __keycmp(void *key1, size_t n1, void *key2, size_t n2);


    /// Search for the bucket index of the key element
    static size_t __findIndex(Hashmap *p_hm, void *key, size_t key_size);
#endif

    /// Create a new hashmap instance
    void newHashmap (
        Hashmap *p_hashmap, 
        size_t n_len
    );


    /// Push value to the hashmap
    void pushToHashmap (
        Hashmap *p_hm,
        void *key,
        size_t key_size,
        void *val
    );


    /// Pop the value from hashmap that is specified with the key
    void *popFromHashmap (
        Hashmap *p_hm,
        void *key,
        size_t key_size
    );


    /// Find the list of all elements' pointers in hashmap
    void **getHashmapList(Hashmap *p_hm);


    /// Find value from the map by specified key
    void *findValue (
        Hashmap *p_hm, 
        void *key, 
        size_t key_len
    );

    
    /// Destroy the given hashmap instance
    void destroyHashmap(Hashmap *p_hm);

#ifdef __cplusplus
}
#endif

#endif
