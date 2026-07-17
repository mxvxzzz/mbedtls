## clone
git clone https://github.com/Mbed-TLS/mbedtls.git
cd mbedtls
git branch -v
git checkout mbedtls-4.2.0

## clone submodule
tabkioum@lmecxl1194:~/mbedtls$ git submodule update --init --recursive
Submodule 'framework' (https://github.com/Mbed-TLS/mbedtls-framework) registered for path 'framework'
Submodule 'tf-psa-crypto' (https://github.com/Mbed-TLS/TF-PSA-Crypto.git) registered for path 'tf-psa-crypto'
Cloning into '/local/home/tabkioum/mbedtls/framework'...
Cloning into '/local/home/tabkioum/mbedtls/tf-psa-crypto'...
Submodule path 'framework': checked out 'dde0c4a0e448a0552f18817dcea633bb851fd288'
Submodule path 'tf-psa-crypto': checked out '73c5da561c8e5253db7b1fb440eda86fde8d8024'
Submodule 'mldsa-native' (https://github.com/Mbed-TLS/mldsa-native) registered for path 'tf-psa-crypto/drivers/pqcp/mldsa-native'
Submodule 'framework' (https://github.com/Mbed-TLS/mbedtls-framework) registered for path 'tf-psa-crypto/framework'
Cloning into '/local/home/tabkioum/mbedtls/tf-psa-crypto/drivers/pqcp/mldsa-native'...
Cloning into '/local/home/tabkioum/mbedtls/tf-psa-crypto/framework'...
Submodule path 'tf-psa-crypto/drivers/pqcp/mldsa-native': checked out '5772b4f4a0105694b1203abb582273f78fa951b7'
Submodule path 'tf-psa-crypto/framework': checked out 'dde0c4a0e448a0552f18817dcea633bb851fd288'
tabkioum@lmecxl1194:~/mbedtls$

## Puis compilation :

cmake -S . -B build
cmake --build build

donc on a compilé ici : ~/mbedtls/build

Fait ls build/library/ : Tu dois voir quelque chose comme :

libmbedcrypto.a
libmbedtls.a
libmbedx509.a
## Comment compiler ton programme 

gcc -I/path/to/mbedtls/include demo.c \
    /path/to/mbedtls/build/library/libmbedcrypto.a \
    -o demo

# Si tu as compilé Mbed TLS dans build/

Depuis n'importe ou ou bien depuis ~/mbedtls/maho-project : 

gcc -I./mbedtls/include demo.c ~/mbedtls/build/library/libmbedcrypto.a -o demo

Depuis ~/mbedtls/maho-project aussi :

gcc -I../include aes_gcm.c ../build/library/libmbedcrypto.a -o aes_gcm

# Si tu veux plus simple : installation système

Sur certaines distributions Linux, tu peux installer une version empaquetée :
sudo apt install libmbedtls-dev
Puis compiler avec :

gcc demo.c -lmbedcrypto -o demo
Mais attention :

la version des paquets système peut être plus ancienne
pour un test simple, c’est pratique
pour un projet sérieux, mieux vaut souvent compiler la version souhaitée