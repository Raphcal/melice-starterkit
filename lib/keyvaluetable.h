//
//  keyvaluetable.h
//  melice
//
//  Created by Raphaël Calabro on 28/11/2021.
//

#ifndef keyvaluetable_h
#define keyvaluetable_h

#include "melstd.h"

#include "list.h"

#define LOAD_FACTOR 0.75
#define DEFAULT_BUCKET_COUNT 16

// Definition macro

// Remplacer les buckets par un typedef ? typedef MELList(K##V##TableEntry) K##V##TableBucket; ?

#define MELKeyValueTableDefine(K, V) typedef struct {\
    K key;\
    V value;\
} K##V##TableEntry;\
\
MELListDefine(K##V##TableEntry);\
\
typedef struct {\
    MELList(K##V##TableEntry) entries;\
} K##V##TableBucket;\
\
MELListDefine(K##V##TableBucket);\
\
/** Table with string keys and V values. */ typedef struct {\
    MELList(K##V##TableBucket) buckets;\
    unsigned int count;\
} K##V##Table;\
\
extern const K##V##Table K##V##TableEmpty;\
void K##V##TableDeinit(K##V##Table * _Nonnull self);\
void K##V##TableClear(K##V##Table * _Nonnull self);\
void K##V##TablePut(K##V##Table * _Nonnull self, K key, V value);\
MELBoolean K##V##TablePutAndGetOldValue(K##V##Table * _Nonnull self, K key, V value, V * _Nullable oldValue);\
MELBoolean K##V##TableGet(K##V##Table self, K key, V * _Nonnull value);\
MELBoolean K##V##TableContains(K##V##Table self, K key);\
void K##V##TableRemove(K##V##Table * _Nonnull self, K key);\
MELBoolean K##V##TableRemoveAndGetOldValue(K##V##Table * _Nonnull self, K key, V * _Nullable oldValue);\
K##V##TableEntryList K##V##TableEntries(K##V##Table * _Nonnull self);

// Implementation macro

#define MELKeyValueTableImplement(K, V) const K##V##Table K##V##TableEmpty = {};\
\
MELListImplement(K##V##TableBucket);\
MELListImplement(K##V##TableEntry);\
\
void K##V##TableDeinit(K##V##Table * _Nonnull self) {\
    for (unsigned int bucketIndex = 0; bucketIndex < self->buckets.capacity; bucketIndex++) {\
        K##V##TableBucket bucket = self->buckets.memory[bucketIndex];\
        if (bucket.entries.memory != NULL) {\
            K##V##TableEntryListDeinit(&self->buckets.memory[bucketIndex].entries);\
        }\
    }\
    K##V##TableBucketListDeinit(&self->buckets);\
    self->count = 0;\
}\
\
void K##V##TableClear(K##V##Table * _Nonnull self) {\
    MELList(K##V##TableBucket) buckets = self->buckets;\
    for (unsigned int bucketIndex = 0; bucketIndex < buckets.capacity; bucketIndex++) {\
        buckets.memory[bucketIndex].entries.count = 0;\
    }\
    self->count = 0;\
}\
\
void K##V##TableGrowAndRehash(K##V##Table * _Nonnull self) {\
    K##V##TableBucketList oldBuckets = self->buckets;\
    const unsigned int newCapacity = oldBuckets.capacity == 0 ? DEFAULT_BUCKET_COUNT : oldBuckets.capacity * 2;\
    K##V##TableBucketList newBuckets = K##V##TableBucketListMakeWithInitialCapacity(newCapacity);\
    if (newBuckets.memory == NULL) {\
        playdate->system->error("Unable to grow dictionary of V to capacity %lu\n", newCapacity);\
        return;\
    }\
    memset(newBuckets.memory, 0, sizeof(K##V##TableBucketList) * newBuckets.capacity);\
    newBuckets.count = oldBuckets.count;\
    for (unsigned int bucketIndex = 0; bucketIndex < oldBuckets.capacity; bucketIndex++) {\
        K##V##TableBucket oldBucket = oldBuckets.memory[bucketIndex];\
        for (unsigned int entryIndex = 0; entryIndex < oldBucket.entries.count; entryIndex++) {\
            K##V##TableEntry entry = oldBucket.entries.memory[entryIndex];\
\
            unsigned int newBucketIndex = entry.key % newBuckets.capacity;\
            K##V##TableEntryListPush(&newBuckets.memory[newBucketIndex].entries, entry);\
        }\
        K##V##TableEntryListDeinit(&oldBucket.entries);\
    }\
    self->buckets = newBuckets;\
    K##V##TableBucketListDeinit(&oldBuckets);\
}\
\
void K##V##TablePut(K##V##Table * _Nonnull self, K key, V value) {\
    K##V##TablePutAndGetOldValue(self, key, value, NULL);\
}\
\
MELBoolean K##V##TablePutAndGetOldValue(K##V##Table * _Nonnull self, K key, V value, V * _Nullable oldValue) {\
    if (self->buckets.memory == NULL) {\
        self->buckets.capacity = 0;\
        K##V##TableGrowAndRehash(self);\
        if (!self->buckets.capacity) {\
            return false;\
        }\
    }\
    const unsigned int bucketIndex = key % self->buckets.capacity;\
    K##V##TableBucket bucket = self->buckets.memory[bucketIndex];\
    if (bucket.entries.count > 0 && bucket.entries.memory != NULL) {\
        for (unsigned int entryIndex = 0; entryIndex < bucket.entries.count; entryIndex++) {\
            K##V##TableEntry entry = bucket.entries.memory[entryIndex];\
            if (entry.key == key) {\
                if (oldValue) {\
                    *oldValue = entry.value;\
                }\
                entry.value = value;\
                return true;\
            }\
        }\
    }\
    if (bucket.entries.count == 0 || bucket.entries.memory == NULL) {\
        self->buckets.count++;\
    }\
    self->count++;\
    K##V##TableEntryListPush(&self->buckets.memory[bucketIndex].entries, (K##V##TableEntry) {key, value});\
\
    const double loadFactor = self->count / (double) self->buckets.capacity;\
    if (loadFactor >= LOAD_FACTOR) {\
        K##V##TableGrowAndRehash(self);\
    }\
    return false;\
}\
\
MELBoolean K##V##TableGet(K##V##Table self, K key, V * _Nonnull value) {\
    if (self.buckets.memory == 0 || self.buckets.memory == NULL) {\
        return false;\
    }\
    unsigned int bucketIndex = key % self.buckets.capacity;\
    K##V##TableBucket bucket = self.buckets.memory[bucketIndex];\
    for (unsigned int entryIndex = 0; entryIndex < bucket.entries.count; entryIndex++) {\
        K##V##TableEntry entry = bucket.entries.memory[entryIndex];\
        if (entry.key == key) {\
            *value = entry.value;\
            return true;\
        }\
    }\
    return false;\
}\
\
MELBoolean K##V##TableContains(K##V##Table self, K key) {\
    if (self.buckets.memory == 0 || self.buckets.memory == NULL) {\
        return false;\
    }\
    unsigned int bucketIndex = key % self.buckets.capacity;\
    K##V##TableBucket bucket = self.buckets.memory[bucketIndex];\
    for (unsigned int entryIndex = 0; entryIndex < bucket.entries.count; entryIndex++) {\
        K##V##TableEntry entry = bucket.entries.memory[entryIndex];\
        if (entry.key == key) {\
            return true;\
        }\
    }\
    return false;\
}\
\
void K##V##TableRemove(K##V##Table * _Nonnull self, K key) {\
    K##V##TableRemoveAndGetOldValue(self, key, NULL);\
}\
\
MELBoolean K##V##TableRemoveAndGetOldValue(K##V##Table * _Nonnull self, K key, V * _Nullable oldValue) {\
    if (self->buckets.memory == 0 || self->buckets.memory == NULL) {\
        return false;\
    }\
    unsigned int bucketIndex = key % self->buckets.capacity;\
    K##V##TableBucket bucket = self->buckets.memory[bucketIndex];\
    for (unsigned int entryIndex = 0; entryIndex < bucket.entries.count; entryIndex++) {\
        K##V##TableEntry entry = bucket.entries.memory[entryIndex];\
        if (entry.key == key) {\
            K##V##TableEntryListRemove(&self->buckets.memory[bucketIndex].entries, entryIndex);\
            self->count--;\
            if (oldValue) {\
                *oldValue = entry.value;\
            }\
            return true;\
        }\
    }\
    return false;\
}\
\
K##V##TableEntryList K##V##TableEntries(K##V##Table * _Nonnull self) {\
    K##V##TableEntryList entries = K##V##TableEntryListMake();\
\
    K##V##TableBucketList buckets = self->buckets;\
    for (unsigned int bucketIndex = 0; bucketIndex < buckets.capacity; bucketIndex++) {\
        K##V##TableBucket bucket = buckets.memory[bucketIndex];\
        for (unsigned int entryIndex = 0; entryIndex < bucket.entries.count; entryIndex++) {\
            K##V##TableEntry entry = bucket.entries.memory[entryIndex];\
            K##V##TableEntryListPush(&entries, entry);\
        }\
    }\
    return entries;\
}

#endif /* keyvaluetable_h */
