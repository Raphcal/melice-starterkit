//
//  list.h
//  shmup
//
//  Created by Raphaël Calabro on 26/02/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef list_h
#define list_h

#include "melstd.h"

#include <string.h>

#define MELList(type) type##List

#define MELListReference(type) /** List of type */ typedef struct mellist_##type { \
/** Content of the list. */ \
type * _Nullable memory; \
/** Number of elements in the list. */ \
unsigned int count; \
/** Current capacity. */ \
unsigned int capacity; \
} type##List;

#pragma mark - Definition

#define MELListDefine(type) /** List of type */ typedef struct mellist_##type { \
    /** Content of the list. */ \
    type * _Nullable memory; \
    /** Number of elements in the list. */ \
    unsigned int count; \
    /** Current capacity. */ \
    unsigned int capacity; \
} type##List;\
\
extern const type##List type##ListEmpty;\
type##List type##ListMake(void);\
type##List type##ListMakeWithInitialCapacity(unsigned int initialCapacity);\
type##List type##ListMakeWrappingMemoryAndCount(type * _Nullable memory, unsigned int count);\
type##List type##ListMakeWithList(type##List other);\
type##List type##ListMakeWithListAndCopyFunction(type##List other, type (* _Nonnull copyFunction)(type));\
void type##ListDeinit(type##List * _Nonnull self);\
void type##ListDeinitWithDeinitFunction(type##List * _Nonnull self, void (* _Nonnull deinitFunction)(type * _Nonnull));\
void type##ListGrow(type##List * _Nonnull self, unsigned int size);\
void type##ListEnsureCapacity(type##List * _Nonnull self, unsigned int required);\
void type##ListPush(type##List * _Nonnull self, type element);\
type type##ListPop(type##List * _Nonnull self);\
type type##ListGet(type##List self, unsigned int index);\
type type##ListSet(type##List * _Nonnull self, unsigned int index, type element);\
void type##ListAddAll(type##List * _Nonnull self, type##List other);\
void type##ListInsert(type##List * _Nonnull self, unsigned int index, type element);\
type type##ListRemove(type##List * _Nonnull self, unsigned int index);\
type type##ListRemoveSwap(type##List * _Nonnull self, unsigned int index);

#define MELListDefineIndexOf(type) \
int type##ListIndexOf(type##List self, type entry);\
int type##ListRemoveEntry(type##List * _Nonnull self, type entry);\
int type##ListRemoveSwapEntry(type##List * _Nonnull self, type entry);

#define MELListDefineSaveRef(type) \
void type##RefListSave(type##RefList self, MELOutputStream * _Nonnull outputStream);\
type##RefList type##RefListLoad(MELInputStream * _Nonnull inputStream);

#pragma mark - Implementation

#define MELListImplement(type) const type##List type##ListEmpty = { NULL, 0, 0 };\
type##List type##ListMake(void) {\
    return type##ListMakeWithInitialCapacity(10);\
}\
type##List type##ListMakeWithInitialCapacity(unsigned int initialCapacity) {\
    if (!initialCapacity) {\
        return type##ListEmpty;\
    }\
    return (type##List) { playdate->system->realloc(NULL, initialCapacity * sizeof(type)), 0, initialCapacity };\
}\
type##List type##ListMakeWrappingMemoryAndCount(type * _Nullable memory, unsigned int count) {\
    if (memory == NULL) {\
        count = 0;\
    }\
    return (type##List) {\
        memory,\
        count,\
        count\
    };\
}\
type##List type##ListMakeWithList(type##List other) {\
    type##List self = {playdate->system->realloc(NULL, sizeof(type) * other.count), other.count, other.count};\
    memcpy(self.memory, other.memory, sizeof(type) * other.count);\
    return self;\
}\
type##List type##ListMakeWithListAndCopyFunction(type##List other, type (* _Nonnull copyFunction)(type)) {\
    type##List self = {playdate->system->realloc(NULL, sizeof(type) * other.count), other.count, other.count};\
    for (unsigned int index = 0; index < other.count; index++) {\
        self.memory[index] = copyFunction(other.memory[index]);\
    }\
    return self;\
}\
void type##ListDeinit(type##List * _Nonnull self) {\
    if (self->memory != NULL) {\
        playdate->system->realloc(self->memory, 0); \
        self->memory = NULL; \
    }\
    self->count = 0; \
    self->capacity = 0; \
}\
void type##ListDeinitWithDeinitFunction(type##List * _Nonnull self, void (* _Nonnull deinitFunction)(type * _Nonnull)) {\
    if (self->count) {\
        const type *listEnd = self->memory + self->count;\
        for (type *item = self->memory; item < listEnd; item++) {\
            deinitFunction(item);\
        }\
    }\
    type##ListDeinit(self); \
}\
void type##ListGrow(type##List * _Nonnull self, unsigned int size) {\
    self->memory = playdate->system->realloc(self->memory, size * sizeof(type));\
    self->capacity = size;\
}\
void type##ListEnsureCapacity(type##List * _Nonnull self, unsigned int required) {\
    if (self->capacity < required) { \
        const unsigned int newCapacity = self->capacity + (self->capacity * 2) + 1; \
        type##ListGrow(self, newCapacity > required ? newCapacity : required); \
    } \
}\
void type##ListPush(type##List * _Nonnull self, type element) {\
    type##ListEnsureCapacity(self, self->count + 1); \
    self->memory[self->count++] = element; \
}\
type type##ListPop(type##List * _Nonnull self) {\
    return self->memory[--self->count];\
}\
type type##ListGet(type##List self, unsigned int index) {\
    return self.memory[index];\
}\
type type##ListSet(type##List * _Nonnull self, unsigned int index, type element) {\
    if (index >= self->count) { \
        type##ListEnsureCapacity(self, index + 1); \
        memset(self->memory + self->count, 0, sizeof(type) * (index - self->count)); \
        self->count = index + 1; \
    } \
    type oldValue = self->memory[index]; \
    self->memory[index] = element; \
    return oldValue;\
}\
void type##ListInsert(type##List * _Nonnull self, unsigned int index, type element) {\
    const unsigned int count = self->count;\
    if (index < 0 || index > count) {\
        return;\
    }\
    type##ListEnsureCapacity(self, count + 1);\
    if (index < count) {\
        memmove(self->memory + index + 1, self->memory + index, sizeof(type) * (count - index));\
    }\
    self->memory[index] = element;\
    self->count++;\
}\
void type##ListAddAll(type##List * _Nonnull self, type##List other) {\
    if (other.count == 0) {\
        return;\
    }\
    type##ListEnsureCapacity(self, self->count + other.count); \
    memcpy(self->memory + self->count, other.memory, sizeof(type) * other.count);\
    self->count += other.count;\
}\
type type##ListRemove(type##List * _Nonnull self, unsigned int index) {\
    const type oldValue = self->memory[index];\
    memmove(self->memory + index, self->memory + index + 1, ((self->count--) - index - 1) * sizeof(type));\
    return oldValue;\
}\
type type##ListRemoveSwap(type##List * _Nonnull self, unsigned int index) {\
    const type oldValue = self->memory[index];\
    self->memory[index] = self->memory[--self->count];\
    return oldValue;\
}

#define MELListImplementIndexOf(type) \
int type##ListIndexOf(type##List self, type entry) {\
    for (unsigned int index = 0; index < self.count; index++) {\
        if (self.memory[index] == entry) {\
            return index;\
        }\
    }\
    return -1;\
}\
int type##ListRemoveEntry(type##List * _Nonnull self, type entry) {\
    type *memory = self->memory;\
    const unsigned int count = self->count;\
    for (unsigned int index = 0; index < count; index++) {\
        if (memory[index] == entry) {\
            memmove(memory + index, memory + index + 1, ((self->count--) - index - 1) * sizeof(type));\
            return index;\
        }\
    }\
    return -1;\
}\
int type##ListRemoveSwapEntry(type##List * _Nonnull self, type entry) {\
    type *memory = self->memory;\
    const unsigned int count = self->count;\
    for (unsigned int index = 0; index < count; index++) {\
        if (memory[index] == entry) {\
            memory[index] = memory[--self->count];\
            return index;\
        }\
    }\
    return -1;\
}

#define MELListImplementIndexOfWithEqualsFunction(type, equals) \
int type##ListIndexOf(type##List self, type entry) {\
    for (unsigned int index = 0; index < self.count; index++) {\
        const type value = self.memory[index];\
        if (equals(value, entry)) {\
            return index;\
        }\
    }\
    return -1;\
}\
int type##ListRemoveEntry(type##List * _Nonnull self, type entry) {\
    type *memory = self->memory;\
    const unsigned int count = self->count;\
    for (unsigned int index = 0; index < count; index++) {\
        const type value = memory[index];\
        if (equals(value, entry)) {\
            memmove(memory + index, memory + index + 1, ((self->count--) - index - 1) * sizeof(type));\
            return index;\
        }\
    }\
    return -1;\
}\
int type##ListRemoveSwapEntry(type##List * _Nonnull self, type entry) {\
    type *memory = self->memory;\
    const unsigned int count = self->count;\
    for (unsigned int index = 0; index < count; index++) {\
        const type value = memory[index];\
        if (equals(value, entry)) {\
            memory[index] = memory[--self->count];\
            return index;\
        }\
    }\
    return -1;\
}

#define MELListImplementSaveRef(type) \
void type##RefListSave(type##RefList self, MELOutputStream * _Nonnull outputStream) {\
    MELOutputStreamWriteUInt32(outputStream, self.count);\
    for (unsigned int index = 0; index < self.count; index++) {\
        type *element = self.memory[index];\
        MELSubSpriteSave(element, outputStream);\
    }\
}\
type##RefList type##RefListLoad(MELInputStream * _Nonnull inputStream) {\
    const unsigned int count = MELInputStreamReadUInt32(inputStream);\
    type##RefList self = type##RefListMakeWithInitialCapacity(count);\
    self.count = count;\
    for (unsigned int index = 0; index < count; index++) {\
        self.memory[index] = type##Load(inputStream);\
    }\
    return self;\
}

#endif /* list_h */
