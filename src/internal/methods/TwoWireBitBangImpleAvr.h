/*-------------------------------------------------------------------------
NeoPixel library helper functions for DotStars using general Pins (APA102/LPD8806).

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by donating (see https://github.com/Makuna/NeoPixelBus)

-------------------------------------------------------------------------
This file is part of the Makuna/NeoPixelBus library.

NeoPixelBus is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoPixelBus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#pragma once


class TwoWireBitBangImple
{
public:
    typedef NeoNoSettings SettingsObject;

    TwoWireBitBangImple(uint8_t pinClock, uint8_t pinData) :
        _pinClock(pinClock),
        _pinData(pinData)
    {
        pinMode(pinClock, OUTPUT);
        pinMode(pinData, OUTPUT);

        _portClock = portOutputRegister(digitalPinToPort(_pinClock));
        _pinMaskClock = digitalPinToBitMask(_pinClock);
        _portData = portOutputRegister(digitalPinToPort(_pinData));
        _pinMaskData = digitalPinToBitMask(_pinData);
    }

    ~TwoWireBitBangImple()
    {
        pinMode(_pinClock, INPUT);
        pinMode(_pinData, INPUT);
    }

    void begin()
    {
        digitalWrite(_pinClock, LOW);
        digitalWrite(_pinData, LOW);
    }

    void beginTransaction()
    {

    }

    void endTransaction()
    {
        digitalWrite(_pinData, LOW);
    }

    void transmitBit(uint8_t bit)
    {
        // set data bit on pin
        // digitalWrite(_pinData, bit); // HIGH : LOW
        if (bit)
        {
            *_portData |= _pinMaskData;
        }
        else
        {
            *_portData &= ~_pinMaskData;
        }

        // set clock high as data is ready
        // digitalWrite(_pinClock, HIGH);
        *_portClock |= _pinMaskClock;

        // set clock low as data pin is changed
        // digitalWrite(_pinClock, LOW);
        *_portClock &= ~_pinMaskClock;
    }

    void transmitByte(uint8_t data)
    {
        for (int bit = 7; bit >= 0; bit--)
        {
            // set data bit on pin
            // digitalWrite(_pinData, (data & 0x80) == 0x80 ? HIGH : LOW);
            if (data & 0x80)
            {
                *_portData |= _pinMaskData;
            }
            else
            {
                *_portData &= ~_pinMaskData;
            }

            // set clock high as data is ready
            // digitalWrite(_pinClock, HIGH);
            *_portClock |= _pinMaskClock;

            data <<= 1;

            // set clock low as data pin is changed
            // digitalWrite(_pinClock, LOW);
            *_portClock &= ~_pinMaskClock;
        }
    }

    void transmitBytes(const uint8_t* data, size_t dataSize)
    {
        const uint8_t* endData = data + dataSize;
        while (data < endData)
        {
            transmitByte(*data++);
        }
    }

    void applySettings([[maybe_unused]] const SettingsObject& settings)
    {
    }

private:
    const uint8_t  _pinClock;     // output pin number for clock line
    const uint8_t  _pinData;      // output pin number for data line

    volatile uint8_t* _portData;  // Output PORT register
    uint8_t  _pinMaskData;        // Output PORT bitmask
    volatile uint8_t* _portClock;  // Output PORT register
    uint8_t  _pinMaskClock;        // Output PORT bitmask
};