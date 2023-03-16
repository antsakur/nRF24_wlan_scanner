
#ifndef NRF24_DEFINES_H
#define NRF24_DEFINES_H

/**
 * This file contains defines e.g. register map address and platform spesific configurations
 */

#define NRF_SPI_SCK_F 10000000 // 10 MHz

// Power amplifier levels
#define  PA_LOW     0x0 // -18dBm     
#define  PA_MED     0x1 // -12dBm 
#define  PA_HIGH    0x2 // -6dBm
#define  PA_MAX     0x3 // 0dBm

// Datarates
#define DATARATE_MED    0x0 // 1Mbps
#define DATARATE_HIGH   0x1 // 2MBPS

// Memory map addresses
#define NRF_CONFIG  0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define NRF_STATUS  0x07
#define OBSERVE_TX  0x08
#define RPD         0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C
#define FEATURE     0x1D

/******************
 * Bit mnemonics 
*******************/
// NRF_CONFIG
#define MASK_RX_DR  0x6
#define MASK_TX_DS  0x5
#define MASK_MAX_RT 0x4
#define EN_CRC      0x3
#define CRCO        0x2
#define PWR_UP      0x1
#define PRIM_RX     0x0

#define CONFIG_RST_STATE 0x08

// RF_SETUP
#define RF_PWR  0x1

// STATUS
#define RX_DR   0x6
#define TX_DS   0x5
#define MAX_RT  0x4
#define RX_P_NO 0x1 // bits 3:1
#define TX_FULL 0x0

// SPI CMD
#define R_REGISTER          0x00
#define W_REGISTER          0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX            0xE1
#define FLUSH_RX            0xE2
#define REUSE_TX_PL         0xE3
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD       0xA8
#define W_TX_PAYLOAD_NO_ACK 0xB0
#define NOP                 0xFF
#define REG_MASK            0x1F // Register mask for write and read commands (xxxA AAAA)

#endif // NRF24_DEFINES_H
