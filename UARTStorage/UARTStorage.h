/**
 * @brief       UARTStorage.h
 * @details     SPI and I2C storage over UART
 *
 *
 * @return      N/A
 *
 * @author      
 * @date        
 * @version     
 * @pre         N/A.
 * @warning     N/A
 * @pre        
 */

#ifndef UARTSTORAGE_H
#define UARTSTORAGE_H

#include "mbed.h"
#include "SPIFBlockDevice.h"
//TODO find MD5 library

#define SERIAL_DATA_CHUNKSIZE 64

class UARTStorage
{
    public:
        UARTStorage();

        //Use default SPI pins but at 1MHz by default to be tolerant of wires to SPI chip
        int init_SPIFlash(PinName mosi = MBED_CONF_SPIF_DRIVER_SPI_MOSI,
                    PinName miso = MBED_CONF_SPIF_DRIVER_SPI_MISO,
                    PinName sclk = MBED_CONF_SPIF_DRIVER_SPI_CLK,
                    PinName csel = MBED_CONF_SPIF_DRIVER_SPI_CS,
                    int freq = 1000000);

        mbed::bd_size_t get_ReadSize();
        mbed::bd_size_t get_EraseSize();
        mbed::bd_size_t get_ProgramSize();
        

        //Returns the pointer to the buffers
        char* get_WriteBuffer();
        char* get_ReadBuffer();

        // Program the current contents of the write buffer, up to a whole erase block
        int program_WriteBuffer(uint32_t flash_addr, uint32_t count);
        // Read address to the read buffer, up to a whole block
        int readto_ReadBuffer(uint32_t flash_addr, uint32_t count);

        //Set a chunk of the write buffer. For example, chunks arrive 64 bytes at a time from serial (hex encoded so 128 serial bytes for 64 real bytes)
        int set_WriteBufferChunk(uint32_t buffer_index, char* bytes, uint32_t count);
        //can be done trivially by getting read buffer pointer and reading the required number of bytes from the index
        //Get a given chunk of the read buffer. For example, after reading a whole block and writing it to serial 128 hex bytes at a time
        //int get_ReadBufferChunk(uint32_t buffer_index, uint32_t count);

        int write_SPIF_Byte(uint8_t databyte, uint32_t flash_byte_addr);
        int read_SPIF_Byte(uint8_t* dest_byte, uint32_t flash_byte_addr);
        
        //read the specified block on the device to serial, in chunks of the given size
        //The address is in bytes but most fall on a block boundary
        int read_SPIF_Block(uint32_t flash_boundary_addr, uint8_t chunksize);
        int write_SPIF_Block(uint32_t flash_boundary_addr);
        int erase_SPIF_Block(uint32_t flash_boundary_addr);
        char* checksum_SPIF_Block(uint32_t flash_boundary_addr);

        // todo read byte range
        // todo write byte range
        char* checksum_ByteRange(char* bytes, int start, int end);

        int sync();

        int trim(bd_addr_t addr, bd_size_t size);
        

        //handle CLI input
        void lineBufferHandler(char* lineBuffer, FILE* output_pc);


        //writes stats on the SPIF device to serial
        int write_SPIF_Stats(FILE* fd);

        //simple test program, writes results to serial
        void spif_test_program(FILE* pc);

        ~UARTStorage();

    private:
        SPIFBlockDevice* spif;
        //TODO: EEPROM https://os.mbed.com/docs/mbed-os/v6.7/feature-i2c-doxy/class_i2_c_e_e_block_device.html

        //todo record SPIF stats for a session
        uint32_t erase_count = 0;
        uint32_t write_count = 0;
        uint32_t read_count = 0;

        //TODO: Do we need two buffers?
        //For Winbond, these will be 4 KBytes malloc'd each!
        char* in_buffer = NULL;
        char* out_buffer = NULL;
        uint32_t buffer_size = 0; //buffer size discovered at runtime by reading flash erase size, although will usually be 4096 bytes

        
};


 #endif
