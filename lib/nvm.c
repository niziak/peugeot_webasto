/*
 * nvm.c
 *
 *  Created on: Sep 12, 2013
 *      Author: nizinski_w
 */

#include <stdint.h>
#include <avr/eeprom.h>
#include "nvm.h"
#include <string.h>
#include "globals.h"
#include "log.h"

#define NVM_MAGIC_LEN       5
#define NVM_MAGIC_BYTE      0xA5
#define NVM_VERSION         1

static EEMEM uint8_t                  NVM_aucMagic[NVM_MAGIC_LEN];
static EEMEM uint16_t                 NVM_uiVersion;
static EEMEM NVM_SET_DEF              NVM_stSettings;

/**
 * Invalidate NVM memory block to force initialise to default values
 */
void NVM_vRestoreFactory(void)
{
    uint8_t aucMagic[NVM_MAGIC_LEN];
    memset (&aucMagic[0], 0x0, sizeof (aucMagic));
    eeprom_write_block ( &(aucMagic[0]), &(NVM_aucMagic[0]), sizeof(aucMagic) );
}

/**
 * Load EEPROM to NVM
 */
void NVM_vLoadSettings(void)
{
    BOOL bError = FALSE;
    uint8_t aucMagic[NVM_MAGIC_LEN];
    uint16_t  uiVersion;

    eeprom_read_block ( &(aucMagic[0]),     &(NVM_aucMagic[0]),     sizeof(aucMagic)    );
    eeprom_read_block ( &(uiVersion),       &(NVM_uiVersion),       sizeof(uiVersion)   );

    for (uint8_t a=0; a<NVM_MAGIC_LEN; a++)
    {
        if (aucMagic[a] != NVM_MAGIC_BYTE)
        {
            bError = TRUE;
        }
    }

    if (uiVersion != NVM_VERSION)
    {
        bError = TRUE;
    }

    NVM_vSetDefaultValues();

    if (bError == TRUE)
    {
        LOG_P(PSTR("\n\n!!! No NVM\n\n"));
        return;
    }
    eeprom_read_block ( pstSettings,                  &(NVM_stSettings),               sizeof(stSettings));

    //DEBUG_MEM( pstSettings,                  sizeof(stSettings));
}

/**
 * Save NVM to EEPROM block
 */
void NVM_vSaveSettings(void)
{
    uint8_t  aucMagic[NVM_MAGIC_LEN];
    uint16_t uiVersion = NVM_VERSION;

    //DEBUG_MEM( pstSettings,                  sizeof(stSettings));

    memset (&aucMagic[0], NVM_MAGIC_BYTE, sizeof (aucMagic));
#if (AVRLIB_HAS_EEPROM_UPDATE_BLOCK_FN)
    eeprom_update_block ( &(uiVersion),               &(NVM_uiVersion),                sizeof(uiVersion)          );
    eeprom_update_block ( &(aucMagic[0]),             &(NVM_aucMagic[0]),              sizeof(aucMagic)           );
    eeprom_update_block ( pstSettings,                &(NVM_stSettings),               sizeof(stSettings));
#else
    eeprom_write_block ( &(uiVersion),               &(NVM_uiVersion),                sizeof(uiVersion)          );
    eeprom_write_block ( &(aucMagic[0]),             &(NVM_aucMagic[0]),              sizeof(aucMagic)           );
    eeprom_write_block ( pstSettings,                &(NVM_stSettings),               sizeof(stSettings));
#endif

    //DEBUG_MEM( pstSettings,                  sizeof(stSettings));
}
