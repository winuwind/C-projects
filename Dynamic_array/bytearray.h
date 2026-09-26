#ifndef DYNAMIC_ARRAY_BYTEARRAY_H
#define DYNAMIC_ARRAY_BYTEARRAY_H


#include <stdio.h>
#include <malloc.h>


typedef unsigned char byte;


struct ByteArray {
    byte* data;
    size_t size;
    size_t capacity;
};


// Инициализировать данные нулями до указанной ёмкости
struct ByteArray* ByteArray_Init(struct ByteArray* ba, size_t capacity);


// Добавить один байт в конец, возможно перевыделяя память, возвращает текущее количество байт, либо 0, если добавление не удалось
size_t ByteArray_AddByte(struct ByteArray* ba, byte b);


// Добавить много байт, возможно перевыделяя память, возвращает текущее количество байт, либо 0, если добавление не удалось
size_t ByteArray_AddBytes(struct ByteArray* ba, const byte* bytes, size_t data_len);


// Получить указатель на данные, NULL если структура не валидна; указатель валиден до следующего вызова AddByte/AddBytes
byte* ByteArray_GetData(struct ByteArray* ba);


// Сбросить данные, возвращает NULL если не удалось выделить память для нового хранилища
struct ByteArray* ByteArray_Reset(struct ByteArray* ba);


// Освободить структуру
void ByteArray_Destroy(struct ByteArray* ba);


#endif //DYNAMIC_ARRAY_BYTEARRAY_H