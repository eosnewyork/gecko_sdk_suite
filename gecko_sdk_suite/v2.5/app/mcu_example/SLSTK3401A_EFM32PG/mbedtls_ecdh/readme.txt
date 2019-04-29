mbedTLS ECDH example using the CRYPTO module in the EFM32PG to
accelerate the ECDH API functions of mbedTLS.

This example project uses the EFM32 CMSIS including EMLIB to run
mbedTLS libraries on the EFM32PG.

The example use the CTR-DRBG, a pseudo random number generator (PRNG)
included in mbedTLS to generate random private keys for both peers
(client and server) that will share the ECDH secret. If the example is
running on a device including a TRNG hw module (True Random Number Generator)
the TRNG will be used as entropy source to seed the CTR-DRBG. If the device
does not incorporate a TRNG, the example will use dummy data to seed the
CTR-DRBG. The entropy accumulator of mbedTLS will use SHA256 to hash the
the entropy data pool which is filled with data from the entropy sources.

The CTR-DRBG involve use of AES which is accelerated by the CRYPTO AES
accelerator.

The entropy accumulator of mbedTLS is setup to use SHA-256 which is
accelerated by the CRYPTO SHA-256 accelerator.

The example redirects standard I/O to the VCOM virtual serial port of the
SLSTK3401A_EFM32PG kit. The user can use a terminal emulator program to
connect to the serial port with the default settings of 115200-8-N-1.

The example has been instrumented with code to count the number of clock
cycles spent inside the ecdh api calls, mbedtls_ecdh_gen_public and
mbedtls_ecdh_compute_shared. The results are printed to stdout, i.e.
the VCOM serial port console.

To check the performance gain of CRYPTO acceleration, the user
can switch off CRYPTO hardware acceleration by compiling with
-DNO_CRYPTO_ACCELERATION in the compiler command line.

The user can change the specific ECC curve used in the example
by modifying the MBEDTLS_ECC_ID macro definition. Available curves with
CRYPTO acceleration support are:
MBEDTLS_ECP_DP_SECP256R1
MBEDTLS_ECP_DP_SECP224R1
MBEDTLS_ECP_DP_SECP192R1

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256
