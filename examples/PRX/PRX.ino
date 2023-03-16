
/** Receiver of the nRF24L01+ 2.4GHz wlan channel scanner.
 */

#include "nrf24_scanner.h"

// If 1, write info to the serial output
#define VERBOSE 0

// Last payload to send on each wlan channel
// !! MUST HAVE MATCHING VALUE WITH PTX SKETCH
#define MAX_PAYLOAD 1000

#define CE_PIN 7
#define CSN_PIN 8
#define IRQ_PIN 2

// Call NRF24 constructor
NRF24 nrf (CE_PIN, CSN_PIN, IRQ_PIN);

// Data buffer for received payload
uint16_t payload_buf;

// Current wlan channel indicator
uint8_t wlan_ch = 1;

volatile bool RX_DR_flag;  // Data ready in RX FIFO irq flag
volatile bool TX_DS_flag;  // Dummy
volatile bool MAX_RT_flag; // Dummy

void irqHandler();

void setup() {
  // Call ISR when IRQ is asserted
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), irqHandler, FALLING);

  #if VERBOSE
    Serial.begin(115200);
    while (!Serial){
      // Wait for serial to start
    }
  #endif

  nrf.init();
  nrf.mask_irq(1, 1, 0);            // Mask MAX_RT & TX_DS
  nrf.set_PA_level(PA_HIGH);        // -6dBm
  nrf.set_data_rate(DATARATE_MED);  // 1Mbps
  nrf.set_wlan_channel(wlan_ch);    // Set RF freq to wlan channel 1
  nrf.set_payload_size(sizeof(payload_buf));  // Set correct payload size

  #if VERBOSE
    nrf.print_reg();
  #endif

  nrf.start_receiving(); // NRF to RX state

  #if VERBOSE
    Serial.println("----------RECEIVING------------");
  #endif

} // setup

void loop() {
  // Wait for RX_DR irq in loop
  if (RX_DR_flag) {
    delayMicroseconds(200); // Wait for the PRX device to send ACK
    nrf.read_rx_payload(&payload_buf);

    #if VERBOSE
      if (payload_buf % (MAX_PAYLOAD/5) == 0) {
        Serial.print("Data received: ");
        Serial.println(payload_buf);
      }
    #endif

    if (payload_buf == MAX_PAYLOAD) {
      nrf.set_ce(LOW);
      nrf.set_wlan_channel(wlan_ch == 13 ? wlan_ch = 1 : ++wlan_ch);
      nrf.set_ce(HIGH);

      #if VERBOSE
        Serial.print("Switching to channel: ");
        Serial.println(wlan_ch);
      #endif
    }

    RX_DR_flag = 0;
  }
} // loop

void irqHandler() {
  nrf.read_irqs(&MAX_RT_flag, &TX_DS_flag, &RX_DR_flag);
}
