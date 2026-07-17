#include <stdio.h>
#include <string.h>
#include "mbedtls/gcm.h"

static void print_hex(const unsigned char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

int main(void)
{
    unsigned char key[16] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };

    unsigned char iv[12] = {
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B
    };

    const unsigned char plaintext[] = "Bonjour AES-GCM!";
    unsigned char ciphertext[sizeof(plaintext)];
    unsigned char tag[16];
    unsigned char decrypted[sizeof(plaintext)];

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    if (mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, 128) != 0) {
        printf("Erreur setkey\n");
        return 1;
    }

    if (mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT,
                                  sizeof(plaintext) - 1,
                                  iv, sizeof(iv),
                                  NULL, 0,
                                  plaintext,
                                  ciphertext,
                                  sizeof(tag), tag) != 0) {
        printf("Erreur chiffrement\n");
        return 1;
    }

    printf("Plaintext:\n%s\n", plaintext);
    printf("Ciphertext:\n");
    print_hex(ciphertext, sizeof(plaintext) - 1);
    printf("Tag:\n");
    print_hex(tag, sizeof(tag));

    if (mbedtls_gcm_auth_decrypt(&gcm,
                                 sizeof(plaintext) - 1,
                                 iv, sizeof(iv),
                                 NULL, 0,
                                 tag, sizeof(tag),
                                 ciphertext,
                                 decrypted) != 0) {
        printf("Erreur authentification/dechiffrement\n");
        return 1;
    }

    decrypted[sizeof(plaintext) - 1] = '\0';
    printf("Decrypted:\n%s\n", decrypted);

    mbedtls_gcm_free(&gcm);
    return 0;
}