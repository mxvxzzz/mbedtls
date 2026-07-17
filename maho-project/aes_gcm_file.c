#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/gcm.h"

// sudo apt remove libmbedtls-dev (ancienn packets de mb edtls qui contiennt les files gcm et ccm.h .....)

#define GCM_KEY_LEN 16
#define GCM_IV_LEN 12
#define GCM_TAG_LEN 16
#define BUF_SIZE 4096

/*
 Chiffrer
	./aes_gcm_file enc monfichier.txt monfichier.enc
 Déchiffrer
	./aes_gcm_file dec monfichier.enc monfichier.dec

 Créer un fichier de test
	echo "Salut Mbed TLS" > monfichier.txt
 Puis :
	cmp monfichier.txt monfichier.dec (silence = fichiers identiques)
	cmp -s /tmp/test_8192.bin /tmp/monfichier.dec && echo "OK: fichiers identiques" || echo "ERREUR: fichiers differents"
	diff monfichier.txt monfichier.dec

 */
static void print_error(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

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

static void random_iv(unsigned char iv[GCM_IV_LEN])
{
    for (size_t i = 0; i < GCM_IV_LEN; i++) {
        iv[i] = (unsigned char)(rand() & 0xFF);
    }
}

int encrypt_file(const char *input_path, const char *output_path)
{
    unsigned char key[GCM_KEY_LEN] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };

    unsigned char iv[GCM_IV_LEN];
    unsigned char tag[GCM_TAG_LEN];
    unsigned char *plaintext = NULL;
    unsigned char *ciphertext = NULL;
    unsigned char *outbuf = NULL;
    size_t pt_len;
    int ret = -1;

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    if (read_file(input_path, &plaintext, &pt_len) != 0) {
        print_error("Erreur lecture fichier entree");
        goto cleanup;
    }

    ciphertext = (unsigned char *)malloc(pt_len);
    outbuf = (unsigned char *)malloc(GCM_IV_LEN + GCM_TAG_LEN + pt_len);
    if (!ciphertext || !outbuf) {
        print_error("Erreur allocation memoire");
        goto cleanup;
    }

    random_iv(iv);

    if (mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, 128) != 0) {
        print_error("Erreur setkey");
        goto cleanup;
    }

    if (mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT,
                                  pt_len,
                                  iv, GCM_IV_LEN,
                                  NULL, 0,
                                  plaintext,
                                  ciphertext,
                                  GCM_TAG_LEN, tag) != 0) {
        print_error("Erreur chiffrement");
        goto cleanup;
    }

    memcpy(outbuf, iv, GCM_IV_LEN);
    memcpy(outbuf + GCM_IV_LEN, tag, GCM_TAG_LEN);
    memcpy(outbuf + GCM_IV_LEN + GCM_TAG_LEN, ciphertext, pt_len);

    if (write_file(output_path, outbuf, GCM_IV_LEN + GCM_TAG_LEN + pt_len) != 0) {
        print_error("Erreur ecriture fichier sortie");
        goto cleanup;
    }

    ret = 0;

cleanup:
    free(plaintext);
    free(ciphertext);
    free(outbuf);
    mbedtls_gcm_free(&gcm);
    return ret;
}

int decrypt_file(const char *input_path, const char *output_path)
{
    unsigned char key[GCM_KEY_LEN] = {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };

    unsigned char *filedata = NULL;
    unsigned char *plaintext = NULL;
    unsigned char iv[GCM_IV_LEN];
    unsigned char tag[GCM_TAG_LEN];
    size_t file_len;
    size_t ct_len;
    int ret = -1;

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    if (read_file(input_path, &filedata, &file_len) != 0) {
        print_error("Erreur lecture fichier chiffre");
        goto cleanup;
    }

    if (file_len < GCM_IV_LEN + GCM_TAG_LEN) {
        print_error("Fichier chiffre trop petit");
        goto cleanup;
    }

    memcpy(iv, filedata, GCM_IV_LEN);
    memcpy(tag, filedata + GCM_IV_LEN, GCM_TAG_LEN);

    ct_len = file_len - GCM_IV_LEN - GCM_TAG_LEN;
    plaintext = (unsigned char *)malloc(ct_len + 1);
    if (!plaintext) {
        print_error("Erreur allocation memoire");
        goto cleanup;
    }

    if (mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, 128) != 0) {
        print_error("Erreur setkey");
        goto cleanup;
    }

    if (mbedtls_gcm_auth_decrypt(&gcm,
                                 ct_len,
                                 iv, GCM_IV_LEN,
                                 NULL, 0,
                                 tag, GCM_TAG_LEN,
                                 filedata + GCM_IV_LEN + GCM_TAG_LEN,
                                 plaintext) != 0) {
        print_error("Erreur authentication/dechiffrement");
        goto cleanup;
    }

    plaintext[ct_len] = '\0';

    if (write_file(output_path, plaintext, ct_len) != 0) {
        print_error("Erreur ecriture fichier clair");
        goto cleanup;
    }

    ret = 0;

cleanup:
    free(filedata);
    free(plaintext);
    mbedtls_gcm_free(&gcm);
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