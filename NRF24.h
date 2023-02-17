
#include <cstdint>
#include "NRF24_defines.h"

// Define the header file only once
#ifndef NRF24_H
#define NRF24_H

/**
 * RF data rate
 */
typedef enum {
  NRF24_1MBPS = 0,
  NRF24_2MBPS = 1,
  NRF24_250KBPS = 2
} nrf24_rf_datarate_e;

/**
 * Power amplifier level
 * LOW = -18dbm, MED = -12dBm, HIGH = -6dBm, MAX = 0dBm
 */
typedef enum {
  NRF24_PA_LOW = 0,
  NRF24_PA_MED = 1,
  NRF24_PA_HIGH = 2,
  NRF24_PA_MAX = 3
} nrf24_pa_dbm_e;

/**
 * CRC length in bits or CRC disabled
 */
typedef enum {
  NRF24_CRC_DISABLE = 0,
  NRF24_CRC_8 = 1,
  NRF24_CRC_16 = 2
} nrf24_crclength_e;


class NRF24
{

private:
  uint8_t ce_pin;
  uint8_t csn_pin;
  uint8_t payload_size;
  uint8_t ack_payload_length;
  uint64_t pipe0_reading_address;
  bool ack_payload_available;

public:
  /**
   * Constructor
   * @param ce_pin Chip enable pin number
   * @param cs_pin Chip select pin number (CSN)
   * @param irq_pin IRQ pin number
   */
  NRF24(uint8_t ce_pin, uint8_t csn_pin, uint8_t irq_pin, uint16_t spi_speed = NRF24_SPI_SPEED);

  /**
   * SPI bus chip select 
   * @param mode Active LOW to put chip on SPI bus, HIGH to disable SPI bus
   */
  void set_csn(uint8_t level);

  /**
   * Power on the device
   * @param state Boolean value, 1=POWER UP, 0=POWER DOWN
   */
  void set_power(uint8_t state);

  /**
   * SET chip enable
   * set HIGH to move to TX mode, LOW to move to Standby-I state
   * @param level HIGH=1, LOW=0
   */
  void set_ce(uint8_t level);

  /**
   * Set RF channel frequency 2400 + channel
   * @param channel RF channel between 0-127
   */
  void set_channel(uint8_t channel);

  /**
   * Set power amplifier level to one of the four levels.
   * @param level PA level
   */
  void set_pa_level(nrf24_pa_dbm_e level);

  /**
   * RX/TX control
   * @param mode 1=RX mode, 0=TX mode
  */
 void set_prim_rx(uint8_t mode);

  /**
   * Read register
   * @param reg_addr Register address to read from
   * @return Byte value of the register
   */
  uint8_t read_reg(uint8_t reg_addr);

  /**
   * Write to register
   * @param reg_addr Register to write
   * @param val Payload to write
   * @param len Number of bytes to write
   */
  void write_reg(uint8_t reg_addr, uint8_t val, uint8_t len = 1);

  /**
   * Write payload to be transmitted
   * Payload length is fixed, see getPayloadSize()
   * 
   * @param buf Get data from here
   * @param len Number of bytes to write
   */
  void write_payload(void* buf, uint8_t len);

  /**
   * Gets the static payload size in bytes from reg RX_PW_Px reg
   * @return Static payload size in bytes
   */
  uint8_t get_payload_size(void);

  /**
   * Flush TX FIFO
   * @return Value of status register
   */
  uint8_t flush_tx(void);

  /**
   * Flush RX FIFO
   * @return Value of status register
   */
  uint8_t flush_rx(void);





};
#endif // NRF24_H