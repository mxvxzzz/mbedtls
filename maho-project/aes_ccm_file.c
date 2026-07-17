#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/ccm.h"

#define CCM_KEY_LEN 16
#define CCM_NONCE_LEN 12
#define CCM_TAG_LEN 16

/*
 Stack Overflow (detecter par le compilateur)
 *** stack smashing detected ***: terminated
	Aborted (core dumped)

 compiler avec -fsanitize=address -g : gcc -I./mbedtls/include aes_ccm_file.c ~/mbedtls/build/library/libmbedcrypto.a -o aes_ccm_file_128 -fsanitize=address -g
 
 */
static int read_file(const char *path, unsigned char **data, size_t *len)
{
    FILE *f = fopen(path, "rb");
    long size;
    size_t read_len;

    if (!f)
        return -1;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return -1;
    }

    size = ftell(f);
    if (size < 0) {
        fclose(f);
        return -1;
    }

    rewind(f);

    *data = (unsigned char *)malloc((size_t)size);
    if (!*data) {
        fclose(f);
        return -1;
    }

    read_len = fread(*data, 1, (size_t)size, f);
    fclose(f);

    if (read_len != (size_t)size) {
        free(*data);
        *data = NULL;
        return -1;
    }

    *len = (size_t)size;
    return 0;
}

static int write_file(const char *path, const unsigned char *data, size_t len)
{
    FILE *f = fopen(path, "wb");
    size_t written;

    if (!f)
        return -1;

    written = fwrite(data, 1, len, f);
    fclose(f);

    if (written != len)
        return -1;

    return 0;
}

static void random_nonce(unsigned char nonce[CCM_NONCE_LEN])
{
    for (size_t i = 0; i < CCM_NONCE_LEN; i++) {
        nonce[i] = (unsigned char)(rand() & 0xFF);
    }
}

int encrypt_file(const char *input_path, const char *output_path)
{
    unsigned char key[CCM_KEY_LEN] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };

    unsigned char nonce[CCM_NONCE_LEN];
    unsigned char tag[CCM_TAG_LEN];
    unsigned char *plaintext = NULL;
    unsigned char *ciphertext = NULL;
    unsigned char *outbuf = NULL;
    size_t pt_len;
    int ret = -1;

    mbedtls_ccm_context ccm;
    mbedtls_ccm_init(&ccm);

    if (read_file(input_path, &plaintext, &pt_len) != 0) {
        fprintf(stderr, "Erreur lecture fichier entree\n");
        goto cleanup;
    }

    ciphertext = (unsigned char *)malloc(pt_len);
    outbuf = (unsigned char *)malloc(CCM_NONCE_LEN + CCM_TAG_LEN + pt_len);
    if (!ciphertext || !outbuf) {
        fprintf(stderr, "Erreur allocation memoire\n");
        goto cleanup;
    }

    random_nonce(nonce);

    if (mbedtls_ccm_setkey(&ccm, MBEDTLS_CIPHER_ID_AES, key, 128) != 0) {
        fprintf(stderr, "Erreur setkey\n");
        goto cleanup;
    }

    if (mbedtls_ccm_encrypt_and_tag(&ccm,
                                    pt_len,
                                    nonce, CCM_NONCE_LEN,
                                    NULL, 0,
                                    plaintext,
                                    ciphertext,
                                    tag, CCM_TAG_LEN) != 0) {
        fprintf(stderr, "Erreur chiffrement\n");
        goto cleanup;
    }

    memcpy(outbuf, nonce, CCM_NONCE_LEN);
    memcpy(outbuf + CCM_NONCE_LEN, tag, CCM_TAG_LEN);
    memcpy(outbuf + CCM_NONCE_LEN + CCM_TAG_LEN, ciphertext, pt_len);

    if (write_file(output_path, outbuf, CCM_NONCE_LEN + CCM_TAG_LEN + pt_len) != 0) {
        fprintf(stderr, "Erreur ecriture fichier sortie\n");
        goto cleanup;
    }

    ret = 0;

cleanup:
    free(plaintext);
    free(ciphertext);
    free(outbuf);
    mbedtls_ccm_free(&ccm);
    return ret;
}

int decrypt_file(const char *input_path, const char *output_path)
{
    unsigned char key[CCM_KEY_LEN] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };

    unsigned char *filedata = NULL;
    unsigned char *plaintext = NULL;
    unsigned char nonce[CCM_NONCE_LEN];
    unsigned char tag[CCM_TAG_LEN];
    size_t file_len;
    size_t ct_len;
    int ret = -1;

    mbedtls_ccm_context ccm;
    mbedtls_ccm_init(&ccm);

    if (read_file(input_path, &filedata, &file_len) != 0) {
        fprintf(stderr, "Erreur lecture fichier chiffre\n");
        goto cleanup;
    }

    if (file_len < CCM_NONCE_LEN + CCM_TAG_LEN) {
        fprintf(stderr, "Fichier chiffre trop petit\n");
        goto cleanup;
    }

    memcpy(nonce, filedata, CCM_NONCE_LEN);
    memcpy(tag, filedata + CCM_NONCE_LEN, CCM_TAG_LEN);

    ct_len = file_len - CCM_NONCE_LEN - CCM_TAG_LEN;
    plaintext = (unsigned char *)malloc(ct_len + 1);
    if (!plaintext) {
        fprintf(stderr, "Erreur allocation memoire\n");
        goto cleanup;
    }

    if (mbedtls_ccm_setkey(&ccm, MBEDTLS_CIPHER_ID_AES, key, 128) != 0) {
        fprintf(stderr, "Erreur setkey\n");
        goto cleanup;
    }

    if (mbedtls_ccm_auth_decrypt(&ccm,
                                 ct_len,
                                 nonce, CCM_NONCE_LEN,
                                 NULL, 0,
                                 filedata + CCM_NONCE_LEN + CCM_TAG_LEN,
                                 plaintext,
                                 tag, CCM_TAG_LEN) != 0) {
        fprintf(stderr, "Erreur authentification/dechiffrement\n");
        goto cleanup;
    }

    plaintext[ct_len] = '\0';

    if (write_file(output_path, plaintext, ct_len) != 0) {
        fprintf(stderr, "Erreur ecriture fichier clair\n");
        goto cleanup;
    }

    ret = 0;

cleanup:
    free(filedata);
    free(plaintext);
    mbedtls_ccm_free(&ccm);
    return ret;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s enc <input_file> <output_file>\n", argv[0]);
        fprintf(stderr, "  %s dec <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "enc") == 0) {
        return encrypt_file(argv[2], argv[3]);
    } else if (strcmp(argv[1], "dec") == 0) {
        return decrypt_file(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Mode inconnu: %s\n", argv[1]);
        return 1;
    }
}