
/** Transmitter of the nRF24L01+ 2.4GHz wlan channel scanner.
 *  Writes wlan channel information to the serial output.
 *  PRX device must be active alongside the PRX device for things to work.
 */
 
#include "nrf24_scanner.h"
#include "average.h"

// If 1, write extra information to serial output
#define VERBOSE 0

// if 1, add 1ms delay between packed transmissions
// Scanner is more likely to catch those devices which send data
// infrequently if delay is enabled
#define DELAY 0

// Last payload to send on each wlan channel
// !! MUST HAVE MATCHING VALUE WITH PRX SKETCH
#define MAX_PAYLOAD 1000

#define WLAN_CHANNELS 13

#define CE_PIN 7
#define CSN_PIN 8
#define IRQ_PIN 2

// Call NRF24 constructor
NRF24 nrf (CE_PIN, CSN_PIN, IRQ_PIN);

// Payload to send, will be incremented on every transmission
uint16_t payload = 1;

// Current wlan channel indicator
uint8_t wlan_ch = 1;

uint8_t data_buffer;  // Buffer for TX payload
uint8_t plos_cnt_buf; // Packets lost buffer
uint8_t arc_cnt_buf;  // Auto retransmits buffer

bool TX_DS_flag;  // Data sent irq
bool MAX_RT_flag; // Max retransmissions irq
bool RX_DR_flag;  // Dummy
bool volatile irq_flag;

// Counter for MAX_RT IRQs. Repeated MAX_RT IRQs indicate that
// PTX is transmitting on different channel than PRX is receiving
uint8_t MAX_RT_cnt;

// !! wlan_ch == 1 -> index[0]
int32_t averages [WLAN_CHANNELS];     // Cumulative average of retransmissions count
int32_t new_samples [WLAN_CHANNELS];  // Retransmit counts of current iteration
uint32_t tmp_sample;                  // Buffer
uint32_t iteration_cnt;               // Number of elapsed iterations/channel loops

void irqHandler();

// Helper function for wlan channel hopping
void hopChannel();

// Helper function for calculating new average retransmissions
void calcAvg();

void setup() {
  Serial.begin(115200);
  while (!Serial)
  {
    // Wait for serial to start
  }

  nrf.init();
  nrf.mask_irq(0, 0, 1);                  // Mask RX_DR irq
  nrf.set_setup_retr(0x02, 0x0F);         // 750us ARD, 15 ARC
  nrf.set_PA_level(PA_LOW);               // -18dBm
  nrf.set_data_rate(DATARATE_MED);        // 1Mbps
  nrf.set_payload_size(sizeof(payload));
  nrf.set_wlan_channel(wlan_ch);          // Set RF freq to wlan channel 1

  // Call ISR when IRQ is asserted
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), irqHandler, FALLING);

  nrf.print_reg();  // Print values of nrf registers

  nrf.write_tx_payload(payload);  // Write payload to TX FIFO
  nrf.transmit();                 // Send it

} // setup

void loop() {
  // TODO Description of the loop!!
  if (irq_flag) 
  {
    nrf.read_irqs(&MAX_RT_flag, &TX_DS_flag, &RX_DR_flag);

    if (TX_DS_flag) 
    {
      #if VERBOSE
        if (payload % (MAX_PAYLOAD/5) == 0) {
          Serial.print("Data sent: ");
          Serial.println(payload);
        }
      #endif

      TX_DS_flag = 0;
      MAX_RT_cnt = 0; // MAX_RT counter can be reseted because transmission was successful

      // Get the amount of retransmissions of last payload
      nrf.read_observe_tx(&plos_cnt_buf, &arc_cnt_buf);
      tmp_sample += arc_cnt_buf;      

      if (payload >= MAX_PAYLOAD) 
      {
        #if VERBOSE
          Serial.print("Last payload sent, retransmissions: ");
          Serial.println(tmp_sample);
        #endif
        
        hopChannel();
      }
      else
      {
        ++payload;
      }
    }
    else if (MAX_RT_flag) 
    {
        MAX_RT_flag = 0;

        if (++MAX_RT_cnt > 20) 
        {
          // ACK packet to PTX most likely lost during channel switching and
          // PTX device is stuck transmitting last payload of the sequence
          #if VERBOSE
            Serial.println("Connection lost to PRX device");
          #endif

          MAX_RT_cnt = 0;
          hopChannel();
        }

        #if VERBOSE
        else 
        {
          Serial.print("Retrying transmission with payload: ");
          Serial.println(payload);  
        }
        #endif
    }

    irq_flag = 0;

    #if DELAY
      delay(1);
    #endif

    nrf.write_tx_payload(&payload);
    nrf.transmit();

  } // irq_flag
} // loop

void irqHandler() 
{
  irq_flag = 1;
}

void hopChannel() 
{
  // Store retransmissions happened on previous channel
  // Move to next wlan channel
  new_samples[wlan_ch-1] = tmp_sample;
  tmp_sample = 0;
  payload = 1;
  nrf.set_wlan_channel(wlan_ch == WLAN_CHANNELS ? wlan_ch = 1 : ++wlan_ch);
  if (wlan_ch == 1) 
  {
    calcAvg();
  }

  #if VERBOSE
    Serial.print("Current channel: ");
    Serial.println(wlan_ch);
  #endif
}

void calcAvg() 
{
  // Calculate new cumulative average and print it
  ++iteration_cnt;
  AVG.calcNewAvg(averages, new_samples, WLAN_CHANNELS, iteration_cnt);
  AVG.printAvg(averages, WLAN_CHANNELS, iteration_cnt);
}
