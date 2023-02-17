
#include "NRF24.h"

//*****************************************
// Implemantations of the header functions
//*****************************************

void NRF24::set_ce(uint8_t level)
{
    digitalWrite(ce_pin, level);
}

void NRF24::set_csn(uint8_t level)
{
    digitalWrite(csn_pin, level);
}

void NRF24::write_reg(uint8_t reg_addr, uint8_t val, uint8_t len = 1)
{
    set_csn(LOW);
    SPI.transaction(W_REGISTER | reg_addr | (REG_MASK & val));
    set_csn_(HIGH);
}

uint8_t NRF24::read_reg(uint8_t reg_addr)
{
    set_csn(LOW);
    SPI.transaction(R_REGISTER | (REG_MASK & reg_addr));
    uint8_t reg_data = SPI.transfer(NOP);

    set_csn(HIGH);
    return reg_data;
}

void NRF24::set_power(uint8_t state)
{
    write_reg(state)

}
