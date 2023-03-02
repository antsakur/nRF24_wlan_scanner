#include "NRF24.h"
#include "NRF24_config.h"
#include "NRF24_defines.h"

//*****************************************
// Implemantations of the header functions
//*****************************************

NRF24::NRF24(uint8_t _ce_pin, uint8_t _csn_pin, uint8_t _irq_pin):
    ce_pin(_ce_pin), csn_pin(_csn_pin), irq_pin(_irq_pin)
{

}

bool NRF24::init(void)
{   
    SPI.begin();

    pinMode(ce_pin, OUTPUT);
    pinMode(csn_pin, OUTPUT);
    pinMode(irq_pin, INPUT_PULLUP);

    set_ce(LOW);
    set_csn(HIGH);

    return true;
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
    uint8_t val = SPI.transfer(NOP);
    end_transaction();

    return val;
}

uint8_t NRF24::read_reg(uint8_t reg_addr, uint8_t* buf)
{
    begin_transaction();
    uint8_t status = SPI.transfer(R_REGISTER | reg_addr);
    *buf = SPI.transfer(NOP);
    end_transaction();

    return status;
}

void NRF24::read_status(uint8_t* buf)
{
    begin_transaction();
    *buf = SPI.transfer(NOP);
    end_transaction();
}

void NRF24::write_reg(uint8_t reg_addr, uint8_t val)
{
    begin_transaction();
    SPI.transfer(W_REGISTER | reg_addr);
    SPI.transfer(val);
    end_transaction();
}

void NRF24::write_status_bit(uint8_t bit, uint8_t val)
{   
    uint8_t status = 0;
    read_status(&status);
    uint8_t current_bit_value = status & (0x1 << bit);
    
    if (val != current_bit_value) {
        (current_bit_value ? (status &= (0x0 << bit)) : (status |= (0x1 << bit)));
    }

    write_reg(STATUS, status);
}

void NRF24::write_reg_bit(uint8_t reg_addr, uint8_t bit, uint8_t val)
{   
    uint8_t reg_val = read_reg(reg_addr);
    uint8_t reg_bit_val = reg_val & (0x1 << bit);
    
    if (val != reg_bit_val) {
        (reg_bit_val ? (reg_val &= (0x0 << bit)) : (reg_val |= (0x1 << bit)));
    }

    write_reg(reg_addr, reg_val);
}

void NRF24::set_setup_retr(uint8_t ARD, uint8_t ARC)
{
    write_reg(SETUP_RETR, ((ARD << 4) | ARC));
}

void NRF24::set_channel(uint8_t channel)
{
    write_reg(RF_CH, channel);
}

void NRF24::set_wlan_channel(uint8_t _wlan_channel)
{
    write_reg(RF_CH, NRF24::wlan_ch[_wlan_channel]);
}

void NRF24::set_power_on(void)
{   
    uint8_t val;
    read_reg(NRF_CONFIG, &val);
    val |= (0x1 << 0x1);
    write_reg(NRF_CONFIG, val);
}

void NRF24::transmit(void)
{
    set_ce(HIGH);
}

bool NRF24::write_tx_payload(uint8_t payload)
{
    begin_transaction();
    uint8_t status = SPI.transfer(W_TX_PAYLOAD);

    if (status & 0x1)   /** Check if TX FIFO full */
    {
        end_transaction(); 
        return false;
    }
    else
    {
        SPI.transfer(payload);
        end_transaction();
        return true;
    }
}

uint8_t NRF24::read_observe_tx(uint8_t* buf)
{
    read_reg(OBSERVE_TX, *buf);
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
    write_reg(STATUS, 0x70);
}

