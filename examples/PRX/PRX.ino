
/** Receiver of the nRF24L01+ 2.4GHz wlan channel scanner.
 */

#include "nrf24_scanner.h"

// If 1, write info to the serial output
#define DEBUG 0

// Last payload to send on each wlan channel
// !! MUST HAVE MATCHING VALUE WITH PTX SKETCH
#define MAX_PAYLOAD 1000

#define CE_PIN 7
#define CSN_PIN 8
#define IRQ_PIN 2

// Call NRF24 constructor
NRF24 nrf (CE_PIN, CSN_PIN, IRQ_PIN);

uint16_t payload_buf;      // Data buffer for received payload
uint8_t wlan_ch = 1;       // Current wlan channel indicator
volatile bool RX_DR_flag;  // Data ready in RX FIFO irq flag

void irqHandler();

void setup() {
  // Call ISR when IRQ is asserted
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), irqHandler, FALLING);

  #if DEBUG
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

  #if DEBUG
    nrf.print_reg();
    Serial.println("----------RECEIVING------------");
  #endif

  nrf.start_receiving(); // NRF to RX state
} // setup

void loop() {
  // Wait for RX_DR irq in loop
  if (RX_DR_flag) {
    nrf.read_rx_payload(&payload_buf);

    #if DEBUG
      if (payload_buf % (MAX_PAYLOAD/5) == 0) {
        Serial.print("Data received: ");
        Serial.println(payload_buf);
      }
    #endif

    if (payload_buf == MAX_PAYLOAD) {
      delayMicroseconds(200); // Wait for the PRX device to send ACK
      nrf.set_ce(LOW);
      nrf.set_wlan_channel(wlan_ch == 13 ? wlan_ch = 1 : ++wlan_ch);
      nrf.set_ce(HIGH);

      #if DEBUG
        Serial.print("Switching to channel: ");
        Serial.println(wlan_ch);
      #endif
    }

    RX_DR_flag = 0;
  }
} // loop

void irqHandler() {
  RX_DR_flag = 1;

  #if DEBUG
    Serial.println("IRQ raised");
  #endif
}
