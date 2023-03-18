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
#include "edac/CRC.h"
#include "Log.h"
#include "Utils.h"

using namespace edac;

#include <catch2/catch_test_macros.hpp>
#include <stdlib.h>
#include <time.h>

TEST_CASE("CRC", "[6-bit Test]") {
    SECTION("6_Sanity_Test") {
        bool failed = false;

        INFO("CRC 6-bit CRC Test");

        srand((unsigned int)time(NULL));

        const uint32_t len = 32U;
        const uint32_t lenBits = len * 8U;
        uint8_t* random = (uint8_t*)malloc(len);

        for (size_t i = 0; i < len - 1U; i++) {
            random[i] = rand();
        }

        CRC::addCRC6(random, lenBits);

        uint32_t inCrc = (random[len - 1U] << 0);
        ::LogDebug("T", "CRC::checkCRC6(), crc = $%02X", inCrc);

        Utils::dump(2U, "6_Sanity_Test CRC", random, len);

        bool ret = CRC::checkCRC6(random, lenBits);
        if (!ret) {
            ::LogDebug("T", "6_Sanity_Test, failed CRC6 check");
            failed = true;
            goto cleanup;
        }

        random[10U] >>= 8;
        random[11U] >>= 8;

        ret = CRC::checkCRC6(random, lenBits);
        if (ret) {
            ::LogDebug("T", "6_Sanity_Test, failed CRC6 error check");
            failed = true;
            goto cleanup;
        }

cleanup:
        delete random;
        REQUIRE(failed==false);
    }
}