#include <stdio.h>
#include <string.h>
#include "mbedtls/aes.h"

int main(void)
{
    unsigned char key[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
    };

    unsigned char iv[16] = {
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B,
        0x1C, 0x1D, 0x1E, 0x1F
    };

    unsigned char input[16] = "0123456789ABCDEF";
    unsigned char output[16];

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    if (mbedtls_aes_setkey_enc(&aes, key, 128) != 0) {
        printf("Erreur setkey_enc\n");
        return 1;
    }

    if (mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, 16, iv, input, output) != 0) {
        printf("Erreur crypt_cbc\n");
        return 1;
    }

    printf("Chiffre CBC:\n");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", output[i]);
    }
    printf("\n");

    mbedtls_aes_free(&aes);
    return 0;
}