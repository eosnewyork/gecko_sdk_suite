/***************************************************************************//**
 * @file
 * @brief NVM config implementation
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

#include <stddef.h>
#include "em_common.h"
#include "nvm.h"
#include "nvm_config.h"

/*******************************************************************************
 ***********************   DATA SPECIFICATION START   **************************
 ******************************************************************************/

/* Example object IDs.
 * These IDs should have names that relate to the data objects defined in nvm_config.c. */
typedef enum {
  COUNTER_ID
} NVM_Object_Ids;

/* Example page IDs.
 * These IDs should have names that relate to the pages defined in nvm_config.c. */
typedef enum {
  PAGE_NORMAL_ID,
  PAGE_WEAR_ID
} NVM_Page_Ids;

uint16_t  nvmCounter = 0;

static NVM_Page_t const nvmPageNormal =
{
/*{Pointer to object,          Size of object,         Object ID}, */
  { (uint8_t *) &nvmCounter, sizeof(nvmCounter), COUNTER_ID },
  NVM_PAGE_TERMINATION /* Null termination of table. Do not remove! */
};

static NVM_Page_t const nvmPageWear =
{
/*{Pointer to object,   Size of object,   Object ID}, */
  { (uint8_t *) &nvmCounter, sizeof(nvmCounter), COUNTER_ID },
  NVM_PAGE_TERMINATION /* Null termination of table. Do not remove! */
};

/* Register all pages into the page table.
 * Assosiate each page to the page ID, and define the type of page. */
static NVM_Page_Table_t const nvmPages =
{
/*{Page ID,      Page pointer,   Page type}, */
  { PAGE_NORMAL_ID, &nvmPageNormal, nvmPageTypeNormal },
  { PAGE_WEAR_ID, &nvmPageWear, nvmPageTypeWear }
};

/*******************************************************************************
 ************************   DATA SPECIFICATION END   ***************************
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/** The code below should not be changed.
 *
 *  Set the storage location in memory. This area should be reserved
 *  using the linker and needs to be aligned with the physical page
 *  grouping of the device.
 *
 *  For the internal flash in the Gecko and Tiny Gecko MCUs, the flash pages are
 *  512 bytes long. This means that the start location must be a multiple of
 *  512 bytes, and that an area equal to 512 bytes * the number of pages and
 *  scratch page must be reserved here.
 *
 *  This can be allocated in the IAR linker file using:
 *
 *    place at address mem:0x1f000 { readonly section NVM_PAGES };
 *
 *  and specifying the pages in the C code using:
 *
 *    const uint8_t nvmData[NVM_PAGE_SIZE * NUMBER_OF_PAGES] @ "NVM_PAGES";
 *
 */
#define NUMBER_OF_USER_PAGES  (sizeof(nvmPages) / sizeof(NVM_Page_Descriptor_t))
#define NUMBER_OF_PAGES (NVM_PAGES_SCRATCH + NUMBER_OF_USER_PAGES)

/// @endcond

SL_ALIGN(NVM_PAGE_SIZE)
#ifdef __ICCARM__
static const uint8_t nvmData[NVM_PAGE_SIZE * NUMBER_OF_PAGES] @ ".text";
#else
static const uint8_t nvmData[NVM_PAGE_SIZE * NUMBER_OF_PAGES] SL_ATTRIBUTE_ALIGN(NVM_PAGE_SIZE) = { 0xFF };
#endif

static NVM_Config_t const nvmConfig =
{
  &nvmPages,
  NUMBER_OF_PAGES,
  NUMBER_OF_USER_PAGES,
  nvmData
};

/***************************************************************************//**
 * @brief
 *   Return a pointer to the config data.
 *
 * @return
 *   A pointer to the configuration
 ******************************************************************************/
NVM_Config_t const *NVM_ConfigGet(void)
{
  return(&nvmConfig);
}
