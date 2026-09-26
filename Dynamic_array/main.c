#include "bytearray.h"
#include <assert.h>
#include <string.h>


void Test_ByteArray() {
    // start
    struct ByteArray ba;
    assert(ByteArray_Init(&ba, 16) != NULL);
    // 1
    byte* data = ByteArray_GetData(&ba);
    assert(data[0] == 0);
    // 2
    assert(ByteArray_AddByte(&ba, 255) == 1);
    data = ByteArray_GetData(&ba);
    assert(data[0] == 255);
    assert(ByteArray_Reset(&ba) == &ba);
    // 3
    byte sample1[] = { 1, 2, 3 };
    assert(ByteArray_AddBytes(&ba, sample1, sizeof(sample1)) == 3);
    assert(ba.size == 3);
    data = ByteArray_GetData(&ba);
    assert(memcmp(data, sample1, sizeof(sample1)) == 0);
    assert(ByteArray_Reset(&ba) == &ba);
    // 4
    byte sample2[] = { 1, 2, 3 };
    assert(ByteArray_AddBytes(&ba, sample2, sizeof(sample2)) == 3);
    assert(ByteArray_AddBytes(&ba, sample2, sizeof(sample2)) == 6);
    assert(ByteArray_AddBytes(&ba, sample2, sizeof(sample2)) == 9);
    assert(ba.size == 9);
    data = ByteArray_GetData(&ba);
    assert(memcmp(data, sample2, sizeof(sample2)) == 0);
    assert(memcmp(data + sizeof(sample2), sample2, sizeof(sample2)) == 0);
    assert(memcmp(data + 2*sizeof(sample2), sample2, sizeof(sample2)) == 0);
    assert(ByteArray_Reset(&ba) == &ba);
    // finish
    ByteArray_Destroy(&ba);
    printf("success\n");
}


int main() {
    Test_ByteArray();
    return 0;
}
