//
// ESP32Tests.cc
//
// Copyright © 2018 Couchbase. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "FleeceTests.hh"
#include "Fleece.hh"
#include "sliceIO.hh"
#include <iostream>

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"

using namespace std;


/* Notes:
ESP32 Thing has 4MB external Flash.
Flash can be mapped to address range 0x3F400000 ... 0x3F7FFFFF

http://esp-idf.readthedocs.io/en/latest/api-reference/storage/spi_flash.html#memory-mapping-apis
Memory mapping APIs are declared in esp_spi_flash.h and esp_partition.h:

spi_flash_mmap() maps a region of physical flash addresses into instruction space or data
space of the CPU
spi_flash_munmap() unmaps previously mapped region
esp_partition_mmap() maps part of a partition into the instruction space or data space
of the CPU.
*/


static void dump(const void *start, size_t size) {
    auto byte = (const uint8_t*)start;
    cerr << hex;
    for (int i = 0; i < size; i += 16) {
        for (int j = 0; j < 16; j++)
            cerr << setw(2) << (unsigned)byte[i+j] << " ";
        cerr << "\n";
    }
    cerr << dec;

}

TEST_CASE("ESP32 mmap") {
    // http://esp-idf.readthedocs.io/en/latest/api-reference/storage/spi_flash.html#_CPPv218esp_partition_mmapPK15esp_partition_t8uint32_t8uint32_t23spi_flash_mmap_memory_tPPKvP23spi_flash_mmap_handle_t
    auto freePages = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
    auto freeBytes = freePages * SPI_FLASH_MMU_PAGE_SIZE;
    cerr << "Mmap pages available: " << freePages << " = " << (freeBytes / 1024) << "KB\n";

    auto ip = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "mmap");
    REQUIRE(ip);
    auto partition = esp_partition_get(ip);
    cerr << "Partition offset = 0x" << hex << partition->address << dec
         << ", size = " << partition->size << "\n";

    const void *mapped = nullptr;
    spi_flash_mmap_handle_t mapHandle = 0;
    CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA,
                                 &mapped, &mapHandle) == 0);

    auto byte = (const uint8_t*)mapped;
    cerr << "Mapped at 0x" << hex << mapped
         << " -- " << (void*)(byte + partition->size - 1) << dec << "\n";
    dump(mapped, 128);

    cerr << "Erasing 4KB...\n";
    CHECK(esp_partition_erase_range(partition, 0, 4096) == 0);
    dump(mapped, 128);

    int wrongByteCount = 0;
    for (int i = 0; i < 128; i++)
        wrongByteCount += (byte[i] != 0xFF);
    CHECK(wrongByteCount == 0);

    char buf[128];
    auto len = sprintf(buf, "Memory mapping is cool! %d", esp_random()) + 1;
    cerr << "Writing some data: \"" << buf << "\"\n";
    CHECK(esp_partition_write(partition, 0, buf, len) == 0);
    dump(mapped, 128);

    CHECK(slice(mapped, len) == slice(buf, len));
}