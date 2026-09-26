#include "bytearray.h"


struct ByteArray* ByteArray_Init(struct ByteArray* ba, size_t capacity){
    if(ba == NULL){
        return NULL;
    }
    ba->data = (byte*) malloc(sizeof(byte));
    if(ba->data == NULL){
        return NULL;
    }
    ba->data[0] = 0;
    ba->size = 0;
    ba->capacity = capacity;
    return ba;
}


size_t ByteArray_AddByte(struct ByteArray* ba, byte b){
    byte* new_bytes = (byte*) malloc(sizeof(byte) * (ba->size + 1));
    if(new_bytes == NULL){
        return 0;
    }
    for(int i = 0; i < ba->size; i++){
        new_bytes[i] = ba->data[i];
    }
    free(ba->data);
    new_bytes[ba->size++] = b;
    ba->data = new_bytes;
    return ba->size;
}


size_t ByteArray_AddBytes(struct ByteArray* ba, const byte* bytes, size_t data_len){
    byte* new_bytes = (byte*) malloc(sizeof(byte) * (ba->size + data_len));
    if(new_bytes == NULL){
        return 0;
    }
    for(int i = 0; i < ba->size; i++){
        new_bytes[i] = ba->data[i];
    }
    free(ba->data);
    for(size_t i = 0; i < data_len; i++){
        new_bytes[i + ba->size] = bytes[i];
    }
    ba->data = new_bytes;
    return ba->size += data_len;
}


byte* ByteArray_GetData(struct ByteArray* ba){
    if(ba == NULL){
        return NULL;
    }
    return ba->data;
}


struct ByteArray* ByteArray_Reset(struct ByteArray* ba){
    free(ba->data);
    ba->data = (byte*) malloc(sizeof(byte));
    if(ba->data == 0){
        return NULL;
    }
    ba->data = 0;
    ba->size = 0;
    return ba;
}


void ByteArray_Destroy(struct ByteArray* ba){
    free(ba->data);
    ba = NULL;
}