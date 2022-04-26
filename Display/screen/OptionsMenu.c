
void LoadOptions()
{
    // Arp Mode
//  arp_mode = EEPROM.read(eeprom::ARP);

//    if ((int8_t)EEPROM.read(eeprom::ARP) >= mode::OFF && (int8_t)EEPROM.read(eeprom::ARP) <= mode::RANDOM)
//    {
//        arp_mode = EEPROM.read(eeprom::ARP);
//    }
//    else {
    arp_mode = mode::OFF;
    //    }

        // RX Channel
    //    if ((int8_t)EEPROM.read(eeprom::RX) >= 0 && (int8_t)EEPROM.read(eeprom::RX) < 16)
    //    {
    //        rx_channel = EEPROM.read(eeprom::RX);
    //    }
    //    else {
    rx_channel = 0;
    //    }

        // TX Channel
    //    if ((int8_t)EEPROM.read(eeprom::TX) >= 0 && (int8_t)EEPROM.read(eeprom::TX) < 16)
    //    {
    //        tx_channel = EEPROM.read(eeprom::TX);
    //    }
    //    else {
    tx_channel = 0;
    //    }

        // Trigger channel
    //    if ((int8_t)EEPROM.read(eeprom::TRIG) >= 0 && (int8_t)EEPROM.read(eeprom::TRIG) < 16)
    //    {
    //        trigger_channel = EEPROM.read(eeprom::TRIG);
    //    }
    //    else {
    trigger_channel = 2;
    //    }
}

void SaveOptions()
{
    //    EEPROM.update(eeprom::ARP, (uint8_t)arp_mode);
    //    EEPROM.update(eeprom::RX, (uint8_t)rx_channel);
    //    EEPROM.update(eeprom::TX, (uint8_t)tx_channel);
    //    EEPROM.update(eeprom::TRIG, (uint8_t)trigger_channel);
}