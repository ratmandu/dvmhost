/**
* Digital Voice Modem - Host Software (Test Suite)
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Host Software / Test Suite
*
*/
/*
*   Copyright (C) 2023 Bryan Biedenkapp N2PLL
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "Defines.h"
#include "AESCrypto.h"
#include "Log.h"
#include "Utils.h"

using namespace crypto;

#include <catch2/catch_test_macros.hpp>
#include <stdlib.h>
#include <time.h>

TEST_CASE("AES", "[LLA AM2 Test]") {
    SECTION("LLA_AM2_Test") {
        bool failed = false;

        INFO("AES P25 LLA AM2 Test");

        /*
        ** TIA-102.AACE-A 6.6 AM2 Sample
        */

        srand((unsigned int)time(NULL));

        // key (KS)
        uint8_t KS[16] = 
        {
            0x05, 0x24, 0x30, 0xBD, 0xAF, 0x39, 0xE8, 0x2F,
            0xD0, 0xDD, 0xD6, 0x98, 0xC0, 0x2F, 0xB0, 0x36
        };

        // RES1
        uint8_t resultRES1[4] = 
        {
            0x3E, 0x00, 0xFA, 0xA8
        };

        // RAND1
        uint8_t RAND1[5] = 
        {
            0x4D, 0x92, 0x5A, 0xF6, 0x08
        };

        // expand RAND1 to 16 bytes
        uint8_t expandedRAND1[16];
        for (uint32_t i = 0; i < 16U; i++)
            expandedRAND1[i] = 0x00U;
        for (uint32_t i = 0; i < 5U; i++)
            expandedRAND1[i] = RAND1[i];

        // perform crypto
        AES* aes = new AES(AESKeyLength::AES_128);

        uint8_t* aesOut = aes->encryptECB(expandedRAND1, 16 * sizeof(uint8_t), KS);

        // reduce AES output
        uint8_t RES1[4];
        for (uint32_t i = 0; i < 4U; i++)
            RES1[i] = aesOut[i];

        Utils::dump(2U, "LLA_AM2_Test, Const Result", resultRES1, 4);
        Utils::dump(2U, "LLA_AM2_Test, AES Out", aesOut, 16);
        Utils::dump(2U, "LLA_AM2_Test, Result", RES1, 4);

        for (uint32_t i = 0; i < 4U; i++) {
            if (RES1[i] != resultRES1[i]) {
                ::LogDebug("T", "LLA_AM2_Test, INVALID AT IDX %d\n", i);
                failed = true;
            }
        }

        delete aes;
        REQUIRE(failed==false);
    }
}
