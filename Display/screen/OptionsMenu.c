void Options()
{
    display.clearDisplay();
    strcpy_P(buffer, (char*)pgm_read_word(&(txt_options[0])));
    CenterText(64, 12, false, buffer);
}

void OptionName(uint8_t item)
{
    SetDisplayPart(0, 128, 2, 2, BLACK);
    strcpy_P(buffer, (char*)pgm_read_word(&(nameOpt[item])));
    CenterText(64, 28, false, buffer);
}

void OptionValue(uint8_t item, uint8_t value)
{
    int val;
    uint8_t ypos = 59;
    bool fsize = true;

    if (item == 0)   // Arp Mode
    {
        fsize = false;
        ypos = 54;
        val = map(value, 0, 31, mode::OFF, mode::RANDOM);
        if (val == arp_mode && parequal == false)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (val != arp_mode)
            {
                arp_mode = val;
                needsUpdate = true;
            }
        }
        strcpy_P(buffer, (char*)pgm_read_word(&(nameArp[arp_mode])));
    }
    if (item == 1)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == rx_channel && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (rx_channel != val)
            {
                if (val != trigger_channel)
                {
                    rx_channel = val;
                    needsUpdate = true;
                }
            }
        }
        itoa(rx_channel + 1, buffer, 10);
    }
    if (item == 2)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == tx_channel && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (tx_channel != val)
            {
                tx_channel = val;
                needsUpdate = true;
            }
        }
        itoa(tx_channel + 1, buffer, 10);
    }
    if (item == 3)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == trigger_channel && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (trigger_channel != val)
            {
                if (val != rx_channel)
                {
                    trigger_channel = val;
                    needsUpdate = true;
                }
            }
        }
        itoa(trigger_channel + 1, buffer, 10);
    }

    if (needsUpdate)
    {
        if (parequal)
        {
            SetDisplayPart(0, 128, 4, 4, BLACK);
        }
        else {
            SetDisplayPart(0, 128, 4, 4, WHITE);
            display.setTextColor(BLACK);
        }
        CenterText(64, ypos, fsize, buffer);
        display.setTextColor(WHITE);
    }
}

void LoadOptions()
{
    // Arp Mode
//  arp_mode = EEPROM.read(eeprom::ARP);

    if ((int8_t)EEPROM.read(eeprom::ARP) >= mode::OFF && (int8_t)EEPROM.read(eeprom::ARP) <= mode::RANDOM)
    {
        arp_mode = EEPROM.read(eeprom::ARP);
    }
    else {
        arp_mode = mode::OFF;
    }

    // RX Channel
    if ((int8_t)EEPROM.read(eeprom::RX) >= 0 && (int8_t)EEPROM.read(eeprom::RX) < 16)
    {
        rx_channel = EEPROM.read(eeprom::RX);
    }
    else {
        rx_channel = 0;
    }

    // TX Channel
    if ((int8_t)EEPROM.read(eeprom::TX) >= 0 && (int8_t)EEPROM.read(eeprom::TX) < 16)
    {
        tx_channel = EEPROM.read(eeprom::TX);
    }
    else {
        tx_channel = 0;
    }

    // Trigger channel
    if ((int8_t)EEPROM.read(eeprom::TRIG) >= 0 && (int8_t)EEPROM.read(eeprom::TRIG) < 16)
    {
        trigger_channel = EEPROM.read(eeprom::TRIG);
    }
    else {
        trigger_channel = 2;
    }
}

void SaveOptions()
{
    EEPROM.update(eeprom::ARP, (uint8_t)arp_mode);
    EEPROM.update(eeprom::RX, (uint8_t)rx_channel);
    EEPROM.update(eeprom::TX, (uint8_t)tx_channel);
    EEPROM.update(eeprom::TRIG, (uint8_t)trigger_channel);
}