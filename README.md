nrf24 wifi channel scanner

Driver flow:
    1. Setup SPI
    2. Configure nrf24 chip over SPI bus
    3. Start nrf24 operations

SPI settings:
    CMD word shifted in in MSBit to LSBit format ( 1 byte total).
    Data shifted LSByte to MSByte, MSBbit first in each byte.

    -Configure CSN_pin and set MOSI, MISO, SCK pins.
    -SPI SCK Frequency MAX 10 MHz
    -SPI bit order = MSBFIRST
    -CPOL=0, CPHA=0 => SPI_MODE0

Configuration registers:

    -set crc length
    -set auto ack on datapipe 0, '1' by default
    -set RX address on datapipe (datapipes 0 & 1 are by default enabled)
    -set address width (default 5 bytes)
    -set auto retransmit delay (SEE NOTES)
    -set auto retransmit count
    -set RF channel
    -set RF datarate
    -set power amplifier (pa) level

PTX Loop:
    1. Write to TX FIFO
    2. Wait for ACK from PRX
    3. Send integers 1-100 in payload per channel
        - IRQ TS_DS asserted if transsmission successful
        - IRQ MAX_RT asserted if retranssmission limit reached
        - IRQ RX_DR asserted if ACK w/ payload received (channel hop ok payload)
    4. Hop to next wlan channel
    5. Repeat


IRQs:
    - MAX_RT, Maximum number of TX retransmits interrupt, STATUS[4]
    - TX_DS, Data Sent TX FIFO interrupt, STATUS[5]
    - RX_DR, Data Ready RX FIFO interrupt, STATUS[6]
