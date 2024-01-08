#ifndef LORA_RN2483_COMMANDS_H
#define LORA_RN2483_COMMANDS_H

/***********************************************************************************************************************
 * Defines
 **********************************************************************************************************************/
#define CMD_LEN(x) (sizeof((x)) - 1)

/***********************************************************************************************************************
 * Commands
 ***********************************************************************************************************************/
const char CMD_TERMINATOR_STR[] = "\r\n";
#define CMD_TERMINATOR ((uint8_t*) CMD_TERMINATOR_STR)
#define CMD_TERMINATOR_LEN CMD_LEN(CMD_TERMINATOR_STR)

const char CMD_SYS_RST_STR[] = "sys reset\r\n";
#define CMD_SYS_RST ((uint8_t*) CMD_SYS_RST_STR)
#define CMD_SYS_RST_LEN CMD_LEN(CMD_SYS_RST_STR)

const char CMD_MAC_PAUSE_STR[] = "mac pause\r\n";
#define CMD_MAC_PAUSE ((uint8_t*) CMD_MAC_PAUSE_STR)
#define CMD_MAC_PAUSE_LEN CMD_LEN(CMD_MAC_PAUSE_STR)

const char CMD_SET_FREQ_STR[] = "radio set freq 869400000\r\n";
#define CMD_SET_FREQ ((uint8_t*) CMD_SET_FREQ_STR)
#define CMD_SET_FREQ_LEN CMD_LEN(CMD_SET_FREQ_STR)

const char CMD_SET_MODE_STR[] = "radio set mod lora\r\n";
#define CMD_SET_MODE ((uint8_t*) CMD_SET_MODE_STR)
#define CMD_SET_MODE_LEN CMD_LEN(CMD_SET_MODE_STR)

const char CMD_SET_PWR_STR[] = "radio set pwr 14\r\n";
#define CMD_SET_PWR ((uint8_t*) CMD_SET_PWR_STR)
#define CMD_SET_PWR_LEN CMD_LEN(CMD_SET_PWR_STR)

const char CMD_SET_SF_STR[] = "radio set sf sf7\r\n";
#define CMD_SET_SF ((uint8_t*) CMD_SET_SF_STR)
#define CMD_SET_SF_LEN CMD_LEN(CMD_SET_SF_STR)

const char CMD_SET_AFC_STR[] = "radio set afcbw 41.7\r\n";
#define CMD_SET_AFC ((uint8_t*) CMD_SET_AFC_STR)
#define CMD_SET_AFC_LEN CMD_LEN(CMD_SET_AFC_STR)

const char CMD_SET_BW_STR[] = "radio set rxbw 250\r\n";
#define CMD_SET_BW ((uint8_t*) CMD_SET_BW_STR)
#define CMD_SET_BW_LEN CMD_LEN(CMD_SET_BW_STR)

const char CMD_SET_PR_STR[] = "radio set prlen 8\r\n";
#define CMD_SET_PR ((uint8_t*) CMD_SET_PR_STR)
#define CMD_SET_PR_LEN CMD_LEN(CMD_SET_PR_STR)

const char CMD_SET_CRC_STR[] = "radio set crc on\r\n";
#define CMD_SET_CRC ((uint8_t*) CMD_SET_CRC_STR)
#define CMD_SET_CRC_LEN CMD_LEN(CMD_SET_CRC_STR)

const char CMD_SET_IQI_STR[] = "radio set iqi off\r\n";
#define CMD_SET_IQI ((uint8_t*) CMD_SET_IQI_STR)
#define CMD_SET_IQI_LEN CMD_LEN(CMD_SET_IQI_STR)

const char CMD_SET_CR_STR[] = "radio set cr 4/5\r\n";
#define CMD_SET_CR ((uint8_t*) CMD_SET_CR_STR)
#define CMD_SET_CR_LEN CMD_LEN(CMD_SET_CR_STR)

const char CMD_SET_WDT_STR[] = "radio set wdt 650\r\n";
#define CMD_SET_WDT ((uint8_t*) CMD_SET_WDT_STR)
#define CMD_SET_WDT_LEN CMD_LEN(CMD_SET_WDT_STR)

const char CMD_SET_SYNC_STR[] = "radio set sync 12\r\n";
#define CMD_SET_SYNC ((uint8_t*) CMD_SET_SYNC_STR)
#define CMD_SET_SYNC_LEN CMD_LEN(CMD_SET_SYNC_STR)

const char CMD_SET_BWD_STR[] = "radio set bw 250\r\n";
#define CMD_SET_BWD ((uint8_t*) CMD_SET_BWD_STR)
#define CMD_SET_BWD_LEN CMD_LEN(CMD_SET_BWD_STR)

const char CMD_GET_EUI_STR[] = "sys get hweui\r\n";
#define CMD_GET_EUI ((uint8_t*) CMD_GET_EUI_STR)
#define CMD_GET_EUI_LEN CMD_LEN(CMD_GET_EUI_STR)

const char CMD_RADIO_RX_STR[] = "radio rx 0\r\n";
#define CMD_RADIO_RX ((uint8_t*) CMD_RADIO_RX_STR)
#define CMD_RADIO_RX_LEN CMD_LEN(CMD_RADIO_RX_STR)

const char CMD_RADIO_TX_STR[] = "radio tx ";
#define CMD_RADIO_TX ((uint8_t*) CMD_RADIO_TX_STR)
#define CMD_RADIO_TX_LEN CMD_LEN(CMD_RADIO_TX_STR)

/***********************************************************************************************************************
 * Return Codes
 ***********************************************************************************************************************/
const char RET_OK[] = "ok";
#define RET_OK_LEN CMD_LEN(RET_OK)

const char RET_ERR[] = "radio err";
#define RET_ERR_LEN CMD_LEN(RET_ERR)

const char RET_INV[] = "invalid_param";
#define RET_INV_LEN CMD_LEN(RET_INV)

const char RET_TX_OK[] = "radio_tx_ok";
#define RET_TX_OK_LEN CMD_LEN(RET_TX_OK)

const char RET_BUSY[] = "busy";
#define RET_BUSY_LEN CMD_LEN(RET_BUSY)

const char RET_RX[] = "radio_rx  ";
#define RET_RX_LEN CMD_LEN(RET_RX)

const char RET_FW[] = "RN2483 1.0.4 Oct 12 2017 14:59:25";
#define RET_FW_LEN CMD_LEN(RET_FW)

#endif
