
/** Transmitter of the nRF24L01+ 2.4GHz wlan channel scanner.
 *  Writes wlan channel information to the serial output.
 *  PRX device must be active alongside the PRX device for things to work.
 */
 
#include "nrf24_scanner.h"
#include "average.h"

// If 1, write extra information to serial output
#define DEBUG 0

// if 1, add 1ms delay between packed transmissions
// Scanner is more likely to catch those devices which send data
// infrequently if delay is enabled
#define DELAY 0

// Last payload to send on each wlan channel
// !! MUST HAVE MATCHING VALUE WITH PRX SKETCH
#define PAYLOAD_LAST 1000

#define WLAN_CHANNELS 13

#define CE_PIN 7
#define CSN_PIN 8
#define IRQ_PIN 2

// Call NRF24 constructor
NRF24 nrf (CE_PIN, CSN_PIN, IRQ_PIN);

uint16_t payload = 1; // Payload to send, will be incremented on every transmission
uint8_t wlan_ch = 1;  // Current wlan channel indicator
uint8_t data_buffer;  // Buffer for TX payload
uint8_t plos_cnt_buf; // Packets lost buffer
uint8_t arc_cnt_buf;  // Auto retransmits buffer

bool TX_DS_flag;  // Data sent successfully irq flag
bool MAX_RT_flag; // Max retransmissions reached irq flag
bool RX_DR_flag;  // Dummy
bool volatile irq_flag; // ISR irq flag

// Counter for MAX_RT IRQs. Repeated MAX_RT IRQs indicate that
// PTX is transmitting on different channel than PRX is receiving
uint8_t MAX_RT_cnt;

// Note!! wlan_ch == 1 -> index[0]
int32_t averages [WLAN_CHANNELS];     // Cumulative average of retransmissions
int32_t new_samples [WLAN_CHANNELS];  // Current iteration retransmissions
uint32_t tmp_sample;                  // Sample buffer
uint32_t iteration_cnt;               // Number of elapsed iterations/channel loops

void irqHandler();

// Helper function for wlan channel hopping
void hopChannel();

// Helper function for calculating new average retransmissions
void calcAvg();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // Wait for serial to start
  }

  // Call ISR when IRQ is asserted
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), irqHandler, FALLING);

  nrf.init();
  nrf.mask_irq(0, 0, 1);                  // Mask RX_DR irq
  nrf.set_setup_retr(0x01, 0x0F);         // 500us ARD, 15 ARC
  nrf.set_PA_level(PA_LOW);               // -18dBm
  nrf.set_data_rate(DATARATE_MED);        // 1Mbps
  nrf.set_payload_size(sizeof(payload));  // Set correct payload size
  nrf.set_wlan_channel(wlan_ch);          // Set RF freq to wlan channel 1

  nrf.print_reg();  // Print values of nrf registers

  nrf.write_tx_payload(payload);  // Write payload to TX FIFO
  nrf.transmit();                 // Send it
} // setup

void loop() {
  // Wait for IRQ in loop
  if (irq_flag) {
    irq_flag = 0;
    nrf.read_irqs(&MAX_RT_flag, &TX_DS_flag, &RX_DR_flag);

      if (TX_DS_flag) { // Transmission successful
        TX_DS_flag = 0;
        MAX_RT_cnt = 0; // MAX_RT counter can be reseted because transmission was successful

        #if DEBUG
          if (payload % (PAYLOAD_LAST/5) == 0) {
            Serial.print("Data sent: ");
            Serial.println(payload);
          }
        #endif

        // Get the amount of retransmissions of last payload
        nrf.read_observe_tx(&plos_cnt_buf, &arc_cnt_buf);
        tmp_sample += arc_cnt_buf;      

        if (payload >= PAYLOAD_LAST) {
          #if DEBUG
            Serial.print("Last payload sent, retransmissions: ");
            Serial.println(tmp_sample);
          #endif
          
          hopChannel();
        } else
          ++payload;
        
      } else if (MAX_RT_flag) { // Transmission failed
          MAX_RT_flag = 0;

          if (++MAX_RT_cnt > 20) {
            // ACK packet to PTX most likely lost during channel switching and
            // PTX device is stuck transmitting last payload of the sequence
            #if DEBUG
              Serial.println("Connection lost to PRX device");
            #endif

            MAX_RT_cnt = 0;
            hopChannel();
          }

          #if DEBUG
          else {
            Serial.print("Retrying transmission with payload: ");
            Serial.println(payload);  
          }
          #endif
      }

        #if DELAY
          delay(1);
        #endif

        nrf.write_tx_payload(&payload);
        nrf.transmit();
  } // irq_flag
} // loop

void irqHandler() {
  irq_flag = 1;
}

void hopChannel() {
  // Store retransmission count of previous channel
  // Move to next wlan channel
  new_samples[wlan_ch-1] = tmp_sample;
  tmp_sample = 0;
  payload = 1;
  nrf.set_wlan_channel(wlan_ch == WLAN_CHANNELS ? wlan_ch = 1 : ++wlan_ch);
  if (wlan_ch == 1) {
    calcAvg();
  }

  #if DEBUG
    Serial.print("Current channel: ");
    Serial.println(wlan_ch);
  #endif
}

void calcAvg() {
  // Calculate new cumulative average and print it
  ++iteration_cnt;
  AVG.calcNewAvg(averages, new_samples, WLAN_CHANNELS, iteration_cnt);
  AVG.printAvg(averages, WLAN_CHANNELS, iteration_cnt);
}
