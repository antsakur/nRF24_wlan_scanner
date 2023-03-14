
#include "NRF24.h"
#include "NRF24_config.h"
#include "printBin.h"

NRF24::NRF24(uint8_t _ce_pin, uint8_t _csn_pin, uint8_t _irq_pin):
    ce_pin(_ce_pin), csn_pin(_csn_pin), irq_pin(_irq_pin)
{}

void NRF24::init(void)
{   
    SPI.begin();

    pinMode(ce_pin, OUTPUT);
    pinMode(csn_pin, OUTPUT);
    pinMode(irq_pin, INPUT_PULLUP);

    set_ce(LOW);
    set_csn(HIGH);

    delay(5);

    // Reset NRF_CONFIG register to reset state
    write_reg(NRF_CONFIG, static_cast<uint8_t>(CONFIG_RST_STATE));
    NRF24::config_reg = static_cast<uint8_t>(CONFIG_RST_STATE);

    flush_rx();
    flush_tx();
    clear_irqs();

    set_power_on();
}

inline void NRF24::begin_transaction(void)
{
    SPI.beginTransaction(SPISettings(NRF_SPI_SCK_F, MSBFIRST, SPI_MODE0));
    set_csn(LOW);
}

inline void NRF24::end_transaction(void)
{   
    set_csn(HIGH);
    SPI.endTransaction();
}

void NRF24::set_ce(uint8_t level)
{
    digitalWrite(ce_pin, level);
}

void NRF24::set_csn(uint8_t level)
{
    digitalWrite(csn_pin, level);
}

uint8_t NRF24::read_reg(uint8_t reg_addr)
{
    begin_transaction();
    SPI.transfer(R_REGISTER | reg_addr);
    uint8_t reg_val = SPI.transfer(NOP);
    end_transaction();

    return reg_val;
}

void NRF24::read_reg(uint8_t reg_addr, uint8_t* buf)
{
    begin_transaction();
    SPI.transfer(R_REGISTER | reg_addr);
    *buf = SPI.transfer(NOP);
    end_transaction();
}

uint8_t NRF24::read_status(void)
{
    begin_transaction();
    uint8_t status = SPI.transfer(NOP);
    end_transaction();

    return status;
}

void NRF24::read_irqs(bool* _max_rt, bool* _tx_ds, bool* _rx_dr)
{
    uint8_t status = read_status();
    *_max_rt = status & (1 << MAX_RT);
    *_tx_ds = status & (1 << TX_DS);
    *_rx_dr = status & (1 << RX_DR);
}

void NRF24::write_reg(uint8_t reg_addr, uint8_t val)
{
    begin_transaction();
    SPI.transfer(W_REGISTER | reg_addr);
    SPI.transfer(val);
    end_transaction();
}

void NRF24::write_reg(uint8_t reg_addr, uint8_t* buf, uint8_t bytes)
{
    begin_transaction();
    SPI.transfer(W_REGISTER | reg_addr);
    while (bytes--) {
        SPI.transfer(*buf++);
    }
    end_transaction();
}

void NRF24::set_PA_level(uint8_t pa_level)
{   
    // Set bits 2:1 (RF_PWR) to '0'
    uint8_t setup_reg_val = read_reg(RF_SETUP) & static_cast<uint8_t>(0xF9);
    setup_reg_val |= (pa_level << RF_PWR);
    write_reg(RF_SETUP, setup_reg_val);
}

void NRF24::set_payload_size(uint8_t _payload_size)
{   
    // Set same payload size for all data pipes
    NRF24::payload_size = _payload_size;
    for (uint8_t i = 0; i < 6; ++i) {
        write_reg(static_cast<uint8_t>(RX_PW_P0 + i), _payload_size);
    }
}

void NRF24::set_data_rate(uint8_t data_rate)
{   
    // Clear RF_DR_HIGH bit
    uint8_t reg_val = read_reg(RF_SETUP) & static_cast<uint8_t>(0xF7);
    reg_val |= (0 << 3);
    write_reg(RF_SETUP, reg_val);
}

void NRF24::set_setup_retr(uint8_t ARD, uint8_t ARC)
{   
    write_reg(SETUP_RETR, static_cast<uint8_t>((ARD << 4 | ARC)));
}

void NRF24::set_wlan_channel(uint8_t ch)
{
    write_reg(RF_CH, NRF24::WLAN_CH[ch - 1]);
}

void NRF24::set_power_on(void)
{   
    NRF24::config_reg |= (1 << PWR_UP);
    write_reg(NRF_CONFIG, NRF24::config_reg);
    delay(5); // Power on delay
}

void NRF24::start_receiving(void)
{
    NRF24::config_reg |= (1 << PRIM_RX);
    write_reg(NRF_CONFIG, NRF24::config_reg);
    flush_rx();
    flush_tx();
    clear_irqs();
    set_ce(HIGH);
    delayMicroseconds(130); // RX settling time
}

void NRF24::write_tx_payload(void* payload)
{   
    uint8_t* tmp_p = reinterpret_cast<uint8_t*>(payload);
    flush_tx(); // Ensure TX FIFO isn't full
    clear_irqs(); // Clear irqs just in case

    uint8_t bytes = NRF24::payload_size;
    begin_transaction();
    SPI.transfer(W_TX_PAYLOAD);
    while (bytes--)  {
        SPI.transfer(*tmp_p++);
    }
    end_transaction();
}

void NRF24::transmit(void)
{
    set_ce(HIGH);
    delayMicroseconds(20);
    set_ce(LOW);
}

void NRF24::read_rx_payload(void* buf)
{   
    uint8_t* tmp_p = reinterpret_cast<uint8_t*>(buf);
    uint8_t bytes = NRF24::payload_size;
    begin_transaction();
    SPI.transfer(R_RX_PAYLOAD);
    while (bytes--) {
        *tmp_p++ = SPI.transfer(NOP);
    }
    end_transaction();
    clear_irqs();
}

void NRF24::flush_tx(void)
{
    begin_transaction();
    SPI.transfer(FLUSH_TX);
    end_transaction();
}

void NRF24::flush_rx(void)
{
    begin_transaction();
    SPI.transfer(FLUSH_RX);
    end_transaction();
}

void NRF24::clear_irqs(void)
{
    write_reg(NRF_STATUS, 0x70);
}

void NRF24::read_observe_tx(uint8_t* plos_cnt_buf, uint8_t* arc_cnt_buf)
{   
    uint8_t reg_val = read_reg(OBSERVE_TX);
    *plos_cnt_buf = (reg_val & 0xF0) >> 4;
    *arc_cnt_buf = (reg_val & 0x0F);
}

void NRF24::print_reg()
{   
    Serial.println("---------REGISTERS-----------");

    // 0x00 - 0x09
    for (int i = 0; i < 10; ++i) {
        uint8_t val = read_reg(i);
        Serial.print("0x0");
        Serial.print(i, HEX);
        Serial.print(": ");
        printBin(val);
    }

    // RX address
    Serial.print("0x0A: ");
    begin_transaction();
    SPI.transfer(R_REGISTER | RX_ADDR_P0);
    for (int i = 0; i < 6; ++i) {
        Serial.print(SPI.transfer(NOP), HEX);
    }
    end_transaction();
    Serial.println();

    // TX address
    Serial.print("0x10: ");
    begin_transaction();
    SPI.transfer(R_REGISTER | TX_ADDR);

    for (int i = 0; i < 6; ++i) {
        Serial.print(SPI.transfer(NOP), HEX);
    }
    end_transaction();
    Serial.println();

    // 0x11 - 0x17
    for (int i = 0x11; i < 0x18 ; ++i) {
        uint8_t val = read_reg(i);
        Serial.print("0x");
        Serial.print(i, HEX);
        Serial.print(": ");
        printBin(val);
    }

    Serial.println("------------------------------");   

}
