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

        int write_SPIF_Stats(FILE* fd);

        void spif_test_program(FILE* pc);


        ~UARTStorage();

    private:
        SPIFBlockDevice* spif;

        //todo record SPIF stats for a session
        uint32_t erase_count = 0;
        uint32_t write_count = 0;
        uint32_t read_count = 0;
};


 #endif
