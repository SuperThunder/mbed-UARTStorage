/**
 * @brief       UARTStorage.cpp
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


 //Class for SPI and I2C storage over UART

 
#include "UARTStorage.h"

/* TODO:
SPI:
Commands:
- Init (reports back OK or error)
    - Provide desired MOSI/MISO/CLK pins? Or fixed SPI pins? (probably better to have fixed SPI pins but allow re-init of new SPI chips)
    - Also, fixed CS pin or specified?
- Status (report OK or error, return the stats on the flash storage)
- Read N bytes starting at address A
- Run checksum on inclusive range [S, E]
- Write sequence of bytes B starting at address A
- Exposes functions of SPI library such as force sync, trim a block
- Keep stats on number of erases, writes, etc done during a session
- Allow live connect (detect chip, init) / disconnect (flush all writes, deinit)


I2C: find a library
- Add command to switch between I2C and SPI mode, or have different commands for I2C SPI?
*/


// don't init anything here, have to call separate SPI / I2C init
// todo: Should probably have separate classes for SPI and I2C storage, with constructors that take the pin/freq parameters
 UARTStorage::UARTStorage()
 {
     
 }

 int UARTStorage::init_SPIFlash(PinName mosi,PinName miso,PinName sclk,PinName csel,int freq)
 {
     //if re-init, clean up old object
     if(spif != 0)
     {
         delete spif;
     }

    spif = new SPIFBlockDevice(mosi, miso, sclk, csel, freq);
    return spif->init();
 }

// Writes SPIF stats to specified FILE (eg, buffered serial)
int UARTStorage::write_SPIF_Stats(FILE* fd)
{
    int status = -1;

    if(fd != 0)
    {
        //fprintf(fd, "spif init status: %d\n", status);
        fprintf(fd, "spif size: %llu\n",         spif->size());
        fprintf(fd, "spif read size: %llu\n",    spif->get_read_size());
        fprintf(fd, "spif program size: %llu\n", spif->get_program_size());
        fprintf(fd, "spif erase size: %llu\n",   spif->get_erase_size());
        fprintf(fd, "spif total device size: %llu\n", spif->size());
    }
    else{
        fprintf(fd, "E: spif not initialized!");
    }

    return 0;
}

//int UARTStorage::erase_SPIF_Block(uint32_t blkaddr, SPIFBlockDevice& spifd){}

// Run test program and write output to specific FILE (eg, buffered serial)
void UARTStorage::spif_test_program(FILE* pc)
{
    int status;

    fprintf(pc, "\n\nspif test\n");

        // Initialize the SPI flash device, and print the memory layout
        //status = spif->init();
        write_SPIF_Stats(pc);

        //uint16_t erase_size = spif->get_erase_size();
        
        fprintf(pc, "\nErasing flash chip...\n");
        // Write "Hello World!" to the first block
        char *out_buffer = (char *)malloc(spif->get_erase_size());
        sprintf(out_buffer, "Hello World! test123\n");
        spif->erase(0, spif->get_erase_size());
        spif->program(out_buffer, 0, spif->get_erase_size());

        fprintf(pc, "\nReading back flash chip: \n");

        // Read back what was stored
        char *in_buffer = (char *)malloc(spif->get_erase_size());
        status = spif->read(in_buffer, 0, spif->get_erase_size());
        fprintf(pc, "spif read status: %d\n", status);
        fprintf(pc, "%s", in_buffer);

        // Deinitialize the devicev
        //spif->deinit();
} 


 UARTStorage::~UARTStorage()
 {
     if(spif != 0)
     {
         spif->deinit();
     }
 }