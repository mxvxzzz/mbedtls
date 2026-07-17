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

    unsigned char nonce_counter[16] = {
        0xF0, 0xF1, 0xF2, 0xF3,
        0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB,
        0xFC, 0xFD, 0xFE, 0xFF
    };

    unsigned char stream_block[16];
    size_t nc_off = 0;

    unsigned char input[] = "Bonjour, Mbed TLS!";
    unsigned char output[sizeof(input)];

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    if (mbedtls_aes_setkey_enc(&aes, key, 128) != 0) {
        printf("Erreur setkey_enc\n");
        return 1;
    }

    if (mbedtls_aes_crypt_ctr(&aes, sizeof(input), &nc_off,
                              nonce_counter, stream_block,
                              input, output) != 0) {
        printf("Erreur crypt_ctr\n");
        return 1;
    }

    printf("Chiffre CTR:\n");
    for (size_t i = 0; i < sizeof(input); i++) {
        printf("%02X ", output[i]);
    }
    printf("\n");

    mbedtls_aes_free(&aes);
    return 0;
}