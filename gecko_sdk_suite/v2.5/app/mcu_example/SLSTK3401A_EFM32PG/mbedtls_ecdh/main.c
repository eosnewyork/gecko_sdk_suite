/***************************************************************************//**
 * @file
 * @brief ECDH(E) example program
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/*
 * This file is a modified version of the ecdh_curve25519.c example program
 * included in mbedtls-2.2.0 (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/timing.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "retargetserial.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define MBEDTLS_ECC_ID    (MBEDTLS_ECP_DP_SECP256R1)

#define mbedtls_printf     printf

static void dump_buf(const char *title, unsigned char *buf, size_t len)
{
  size_t i;

  mbedtls_printf("%s", title);
  for ( i = 0; i < len; i++ ) {
    mbedtls_printf("%c%c", "0123456789ABCDEF"[buf[i] / 16],
                   "0123456789ABCDEF"[buf[i] % 16]);
  }
  mbedtls_printf("\n");
}

static void dump_mpi(const char *title, mbedtls_mpi *mpi)
{
  unsigned char buf[32];

  if ( mbedtls_mpi_write_binary(mpi, buf, sizeof buf) != 0 ) {
    mbedtls_printf("internal error\n");
    return;
  }

  dump_buf(title, buf, sizeof buf);
}

int main(void)
{
  int ret;
  mbedtls_ecdh_context ctx_cli, ctx_srv;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  unsigned char cli_x_to_srv[32], cli_y_to_srv[32],
                srv_x_to_cli[32], srv_y_to_cli[32];
  const char pers[] = "ecdh";
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
  unsigned long cycles;
  uint32_t freqKHz;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
  freqKHz = CMU_ClockFreqGet(cmuClock_CORE) / 1000;

  /* Enable timestamping */
  if ( (CMU->STATUS & _CMU_STATUS_AUXHFRCOENS_MASK) == 0 ) {
    /* Enable debug clock AUXHFRCO */
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
  }

  /* Clear and enable DWT CYCCNT counter */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  /* Initialize LEUART/USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  mbedtls_ecdh_init(&ctx_cli);
  mbedtls_ecdh_init(&ctx_srv);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  /*
   * Initialize random number generation
   */
  mbedtls_printf("  . Seeding the random number generator...");

  mbedtls_entropy_init(&entropy);
  if ( (ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    (const unsigned char *) pers,
                                    sizeof pers)) != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok\n");

  /*
   * Client: inialize context and generate keypair
   */
  mbedtls_printf("  . Setting up client context...");

  ret = mbedtls_ecp_group_load(&ctx_cli.grp, MBEDTLS_ECC_ID);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_load returned %d\n", ret);
    goto exit;
  }

  cycles = DWT->CYCCNT;
  ret = mbedtls_ecdh_gen_public(&ctx_cli.grp, &ctx_cli.d, &ctx_cli.Q,
                                mbedtls_ctr_drbg_random, &ctr_drbg);
  cycles = DWT->CYCCNT - cycles;

  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_cli.Q.X, cli_x_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_cli.Q.Y, cli_y_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (key size: %d bits, clks: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 (int) ctx_cli.grp.pbits, cycles, cycles / freqKHz);

  /*
   * Server: initialize context and generate keypair
   */
  mbedtls_printf("  . Setting up server context...");

  ret = mbedtls_ecp_group_load(&ctx_srv.grp, MBEDTLS_ECC_ID);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_load returned %d\n", ret);
    goto exit;
  }

  cycles = (DWT->CYCCNT);
  ret = mbedtls_ecdh_gen_public(&ctx_srv.grp, &ctx_srv.d, &ctx_srv.Q,
                                mbedtls_ctr_drbg_random, &ctr_drbg);
  cycles = (DWT->CYCCNT) - cycles;
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_srv.Q.X, srv_x_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_write_binary(&ctx_srv.Q.Y, srv_y_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (key size: %d bits, clks: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 (int) ctx_srv.grp.pbits, cycles, cycles / freqKHz);

  /*
   * Server: read peer's key and generate shared secret
   */
  mbedtls_printf("  . Server reading client key and computing secret...");

  ret = mbedtls_mpi_lset(&ctx_srv.Qp.Z, 1);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_lset returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_srv.Qp.X, cli_x_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_srv.Qp.Y, cli_y_to_srv, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  cycles = (DWT->CYCCNT);

  ret = mbedtls_ecdh_compute_shared(&ctx_srv.grp, &ctx_srv.z,
                                    &ctx_srv.Qp, &ctx_srv.d,
                                    mbedtls_ctr_drbg_random, &ctr_drbg);

  cycles = (DWT->CYCCNT) - cycles;

  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (clks: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 cycles, cycles / freqKHz);

  /*
   * Client: read peer's key and generate shared secret
   */
  mbedtls_printf("  . Client reading server key and computing secret...");

  ret = mbedtls_mpi_lset(&ctx_cli.Qp.Z, 1);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_lset returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_cli.Qp.X, srv_x_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  ret = mbedtls_mpi_read_binary(&ctx_cli.Qp.Y, srv_y_to_cli, 32);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    goto exit;
  }

  cycles = (DWT->CYCCNT);

  ret = mbedtls_ecdh_compute_shared(&ctx_cli.grp, &ctx_cli.z,
                                    &ctx_cli.Qp, &ctx_cli.d,
                                    mbedtls_ctr_drbg_random, &ctr_drbg);

  cycles = (DWT->CYCCNT) - cycles;

  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok  (clks: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 cycles, cycles / freqKHz);

  /*
   * Verification: are the computed secret equal?
   */
  mbedtls_printf("  . Checking if both computed secrets are equal...");

  ret = mbedtls_mpi_cmp_mpi(&ctx_cli.z, &ctx_srv.z);
  if ( ret != 0 ) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret);
    goto exit;
  }

  mbedtls_printf(" ok\n");

  dump_mpi("  + Shared Secret: ", &ctx_cli.z);

  exit:

  mbedtls_ecdh_free(&ctx_srv);
  mbedtls_ecdh_free(&ctx_cli);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);

  while (1) ;
}
