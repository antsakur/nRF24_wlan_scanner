/*
  nrf24_scanner.h - Library for nrf24 wlan scanner
*/
#ifndef NRF24_SCANNER_H
#define NRF24_SCANNER_H

#include "NRF24_config.h"

class NRF24
{

private:
  uint8_t ce_pin;         /* Chip Enable pin */
  uint8_t csn_pin;        /* SPI bus Chip Select pin */
  uint8_t irq_pin;        /* Active LOW IRQ pin */
  uint8_t config_reg;     /* NRF_CONFIG register value */
  uint8_t payload_size;

  /* WLAN channel constants for setting RF channel */
  const uint8_t WLAN_CH[13] = {
    12, 17, 22, 27, 32, 37, 42, 47, 52, 57, 62, 67, 72
  };

  /**
   * Set SPI bus Chip Select (CSN).
   * Active LOW
   * @param state HIGH or LOW
   */
  void set_csn(bool state);

  /**
   * Begin SPI transaction and toggle CSN pin LOW
   */
  inline void begin_transaction();

  /**
   * End SPI transaction and toggle CSN pin HIGH
   */
  inline void end_transaction();

public:
  /**
   * Constructor
   * @param ce_pin Chip enable pin number
   * @param csn_pin Chip select pin number (CSN)
   * @param irq_pin IRQ pin number
   */
  NRF24(uint8_t ce_pin, uint8_t csn_pin, uint8_t irq_pin);

  /**
   * Initializes SPI, MCU pins and NRF.
   * Call this method before doing anything related to NRF!
   */
  void init(void);

  /**
   * Set chip enable.
   * HIGH state enables RX/TX mode
   * @param state HIGH or LOW
   */
  void set_ce(bool state);

  /**
   * Read register value and return it
   * @param reg_addr Register map address
   * @return Register value
   */
  uint8_t read_reg(uint8_t reg_addr);

  /**
   * Read register and write value to pointer destination
   * @param reg_addr Register map address
   * @param buf Pointer to buffer to store data
   */
  void read_reg(uint8_t reg_addr, uint8_t* buf);

  /**
   * Get STATUS register value
   * @return STATUS register value
   */
  uint8_t read_status(void);

  /**
   * Mask interrupts.
   * Masked interrupts are not reflected on IRQ pin
   * @param _MAX_RT '1' to mask, '0' to unmask max_rt irq
   * @param _TX_DS '1' to mask, '0' to unmask tx_ds irq
   * @param _RX_DR '1' to mask, '0' to unmask rx_dr irq
   */
  void mask_irq(bool _MAX_RT, bool _TX_DS, bool _RX_DR);

  /**
   * Read irq values from STATUS register
   * @param _MAX_RT Address to write irq status of 'MAX_RT'
   * @param _TX_DS Address to write irq status of 'TX_DS'
   * @param _RX_DR Address to write irq status of 'RX_DR'
   */
  void read_irqs(bool* _MAX_RT, bool* _TX_DS, bool* _RX_DR);

  /**
   * Write to register
   * @param reg_addr Which register to write
   * @param val Value to write
   */
  void write_reg(uint8_t reg_addr, uint8_t val);

  /**
   * Write arbitary number of bytes to register
   * @param reg_addr Which register to write
   * @param buf Buffer address where to read data to be written
   * @param len Number of bytes to write
   */
  void write_reg(uint8_t reg_addr, uint8_t* buf, uint8_t bytes);

  /**
   * Set power amplifier level
   * @param pa_level See 'NRF24_defines.h' for valid values
   */
  void set_PA_level(uint8_t pa_level);

  /**
   * Set payload size for all data pipes
   * @param _payload_size Number of bytes in each payload
   */
  void set_payload_size(uint8_t _payload_size);

  /**
   * Set RF datarate.
   * @note Only modifies RF_DR_HIGH bit thus valid modes are 1Mbps or 2Mbps 
   * @param data_rate See 'NRF24_defines.h' for valid values
   */
  void set_data_rate(uint8_t data_rate);
  
  /**
   * Set SETUP_RETR register value
   * @param ARD Auto Retransmit Delay, set in 250us intervals.
   *            E.g. '0000' - 250us, '1111' - 1000us
   * @param ARC Auto Retransmit Count.
   *            Number of retransmit if ACK not received, valid values 0-15.
   */
  void set_setup_retr(uint8_t ARD, uint8_t ARC);

  /**
   * Set RF channel frequency to match WLAN channel frequency
   * @param wlan_channel WLAN channel number (1-13)
   */
  void set_wlan_channel(uint8_t wlan_channel);

  /**
   * Set PWR_UP bit to 1 in CONFIG register.
   * NRF moves to standby-I mode
   */
  void set_power_on(void);

  /**
   * Move NRF to RX state 
   */
  void start_receiving(void);

  /**
   * Write payload to TX FIFO
   * @param payload Payload address
   */
  void write_tx_payload(void* payload);

  /**
   * Pulse CE HIGH to transmit one payload from TX FIFO
   */
  void transmit(void);

  /**
   * Read RX FIFO payload to buffer.
   * @note Also resets all irqs
   * @param buffer Buffer address
   */
  void read_rx_payload(void* buf);
 
  /**
   * Flush TX FIFO
   */
  void flush_tx();

  /**
   * Flush RX FIFO
   */
  void flush_rx();

  /**
   * Clear all 3 IRQs on STATUS register
   */
  void clear_irqs();

  /**
   * Read OBSERVE_TX register value to given addresses.
   * PLOS_CNT = packets lost since channel change.
   * ARC_CNT = retransmits of the same packet
   * @param plos_cnt_buf Address to store packet lost count value
   * @param arc_cnt_buf Address to store auto retransmit count value
   */
  void read_observe_tx(uint8_t* plos_cnt_buf, uint8_t* arc_cnt_buf);

  /**
   * Prints nrf's register values to console in pretty format
   */
  void print_reg();

};
#endif // NRF24_SCANNER_H
