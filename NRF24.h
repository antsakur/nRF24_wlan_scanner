
// Define the header file only once
#ifndef NRF24_H
#define NRF24_H

#include <stdint.h>
#include "NRF24_defines.h"

class NRF24
{

private:
  uint8_t ce_pin;    /** Chip Enable pin. Used to select RX/TX mode*/
  uint8_t csn_pin;   /** SPI bus Chip Select pin */
  uint8_t irq_pin;  /** Active LOW IRQ pin*/

  /** WLAN channel constants for setting RF channel */
  const uint8_t wlan_ch[14] = {
    0, 12, 17, 22, 27, 32, 37, 42, 47, 52, 57, 62, 67, 72
  };

public:
  /**
   * Constructor
   * @param ce_pin Chip enable pin number
   * @param csn_pin Chip select pin number (CSN)
   * @param irq_pin IRQ pin number
   */
  NRF24(uint8_t ce_pin, uint8_t csn_pin, uint8_t irq_pin);

  /**
   * Initialize SPI bus, ce pin and csn pin
   * @return - 'true' Initializing is complete
   * @return - 'false' Initializing in progress
   */
  bool init();

  /**
   * Begin SPI transaction.
   * Includes CSN pin toggle LOW
   */
  inline void begin_transaction();

  /**
   * End SPI transaction.
   * Includes CSN pin toggle HIGH
   */
  inline void end_transaction();

  /**
   * Set SPI bus Chip Select (CSN).
   * Active LOW
   * @param level HIGH or LOW
   */
  void set_csn(uint8_t level);

  /**
   * Set Chip Select pin
   * @param level HIGH or LOW
   */
  void set_ce(uint8_t level);

  /**
   * Read register
   * @param reg_addr Register map address
   * @return uint8_t Register value
   */
  uint8_t read_reg(uint8_t reg_addr);

  /**
   * Read register
   * @param reg_addr Register map address
   * @param buf Pointer to buffer to store read data
   * @return uint8_t STATUS register value
   */
  uint8_t read_reg(uint8_t reg_addr, uint8_t* buf);

  /**
   * Read status register
   * @return Status register value
   */
  void read_status(uint8_t* buf);

  /**
   * Write to register
   * @param reg_addr Register address
   * @param val Value to write
   */
  void write_reg(uint8_t reg_addr, uint8_t val);

  /**
   * Write bit of STATUS register to '1' or '0'
   * @param bit Which bit (0-7) to manipulate
   * @param value Bit value '1' or '0' 
   */
  void write_status_bit(uint8_t bit, uint8_t val);

  /**
   * Write bit of register to '1' or '0'
   * @param reg_addr Register map address
   * @param bit Which bit (0-7) to manipulate
   * @param val Bit value '1' or '0'
   */
  void write_reg_bit(uint8_t reg_addr, uint8_t bit, uint8_t val);

  /**
   * Set SETUP_RERT register value
   * @param ARD Auto Retransmit Delay, set in 250us intervals.
   *            E.g. '0000' - 250us, '1111' - 1000us
   * @param ARC Auto Retransmit Count.
   *            Number of retransmit if ACK not received.
   */
  void set_setup_retr(uint8_t ARD, uint8_t ARC);

  /**
   * Set RF channel frequency, 2400 + channel (MHz)
   * @param channel RF channel between 0-127
   */
  void set_channel(uint8_t channel);

  /**
   * Set RF channel frequency with WLAN channel number
   * @param wlan_channel WLAN channel number (1-13)
   */
  void set_wlan_channel(uint8_t wlan_channel);

  /**
   * Set PWR_UP bit to 1 in CONFIG register.
   * NRF moves to standby-I mode
   */
  void set_power_on(void);

  /**
   * Set NRF to TX mode.
   * Sets CE pin to HIGH
   */
  void transmit(void);

  /**
   * Write payload to TX FIFO
   * @param payload Payload to TX FIFO
   * @return - 'true' Write to TX FIFO successful.
   * @return - 'false' TX FIFO full
   */
  bool write_tx_payload(uint8_t payload);

  /**
   * Read OBSERVE_TX register.
   * Bits 7:4 contain lost packet count.
   * Bits 3:0 contain retransmitted packet count.
   * @return uint8_t register value
   */
  uint8_t read_observe_tx(uint8_t* buf);
  

  /**
   * Flush TX FIFO when in PTX mode
   */
  void flush_tx();

  /**
   * Flush RX FIFO when in PRX mode
   */
  void flush_rx();

  /**
   * Clear all 3 IRQs on STATUS register
   */
  void clear_irqs();

};
#endif // NRF24_H