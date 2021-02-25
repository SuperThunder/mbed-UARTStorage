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
#include <cstdint>

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
//          also, should UART_SPI_Storage then be a child of SPIFBlockDevice? to expose all the functions of the SPIFBlockDevice object without defining them again
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
    int status = spif->init();
    
    //if we were able to init the flash device, malloc buffers of the correct size
    if(status == SPIF_BD_ERROR_OK)
    {
        buffer_size = spif->get_erase_size();
        out_buffer = (char *)malloc(buffer_size);
        in_buffer = (char *)malloc(buffer_size);
    }

    return status;
    
 }

 mbed::bd_size_t UARTStorage::get_EraseSize()
 {
    return spif->get_erase_size();
 }

char* UARTStorage::get_WriteBuffer()
{
    return out_buffer;
}

char* UARTStorage::get_ReadBuffer()
{
    return in_buffer;
}

// Program the current contents of the write buffer
int UARTStorage::program_WriteBuffer(uint32_t flash_addr, uint32_t count)
{
    //todo: check if erase actually needs to be done? or always do it?

    //todo check if flash_addr is on erase boundary

    int status;

    //todo figure out which block to erase - size parameter to erase has to be a multiple of erase block size
    //todo check erase status
    spif->erase(flash_addr, spif->get_erase_size());

    status = spif->program(out_buffer, flash_addr, count);

    return status;
}


// Read address to the read buffer
int UARTStorage::readto_ReadBuffer(uint32_t flash_block_addr, uint32_t count)
{
    int status;

    status = spif->read(in_buffer, flash_block_addr, count);

    return status;
}



int UARTStorage::read_SPIF_Byte(uint8_t* dest_byte, uint32_t flash_byte_addr)
{
    int status = -1;

    status = spif->read(dest_byte, flash_byte_addr, 1);

    return status;
}

// Very inefficient - much better to do whole blocks
// TODO Figure out which location in buffer byte should be written to 
//      And maybe read that byte first to see if we need to do an erase or not
//  if we have to do an erase first, we will need to read whole block -> modify given byte -> erase whole block -> write back modified block
/*
//Function to write a specific byte to a specific byte address in the flash (not block address)
int UARTStorage::write_SPIF_Byte(uint8_t databyte, uint32_t flash_byte_addr)
{
    uint8_t current_byte = spif->read(current_byte, , spif->get_erase_size());

    int status = spif->program(&databyte, flash_addr, spif->get_erase_size());

    return status;
} */



int UARTStorage::read_SPIF_Block(uint32_t flash_block_addr, uint8_t chunksize)
{
    // send bytes to serial, 'chunksize' at a time
    //readto_ReadBuffer(flash_block_addr);

    return 0;
}

//int UARTStorage::erase_SPIF_Block(uint32_t blkaddr, SPIFBlockDevice& spifd){}


// Writes SPIF stats to specified FILE (eg, buffered serial)
int UARTStorage::write_SPIF_Stats(FILE* fd)
{
    int status = -1;

    if(fd != 0)
    {
        //fprintf(fd, "spif init status: %d\n", status);
        //NOTE: Some of these get_*_size functions (program / read) are hardcoded to assume 1 byte in SPIFBlockDevice.cpp
        fprintf(fd, "spif size: %llu bytes \n",   spif->size());
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

// Run test program and write output to specific FILE (eg, buffered serial)
void UARTStorage::spif_test_program(FILE* pc)
{
    int status;

    fprintf(pc, "\n\nspif test\n");

    // Initialize the SPI flash device, and print the memory layout
    //status = spif->init();
    write_SPIF_Stats(pc);

    //uint16_t erase_size = spif->get_erase_size();
    
    //fprintf(pc, "\nErasing flash chip...\n");
    // Write "Hello World!" to the first block

    //sprintf(out_buffer, "Some new buffer text!\nand another line\n");
    //spif->erase(0, spif->get_erase_size());
    //spif->program(out_buffer, 0, spif->get_erase_size());

    fprintf(pc, "\nReading flash chip block 0: \n");

    // Read back what was stored
    
    status = spif->read(in_buffer, 0, spif->get_erase_size());
    fprintf(pc, "spif read status: %d\n", status);
    fprintf(pc, "%s", in_buffer);

    // Deinitialize the devicev
    //spif->deinit();
}

//Called by serialCLI whenever a new line is encountered, and sent all the characters from the beginning of the line up to the newline
//get commands from PC
// init / deinit SPI flash chip
//  write XXXX to block A (sequence of bytes up to some limit eg 64 bytes)
//  read block A
//  do md5 on range [S, E]

//send output back to PC
//  content of block A is XXXXXX
//  erased block A
//  md5 of block range [S, E] is HHHHHH
//  acknowledgement of success of a write sequence
//  return success or failure of a init / deinit
//  status of SPIF device
//  stats on SPIF device (number of writes this session, erase size, chip model, total size, etc)

// TODO: implement parsing of above commands from PC
void UARTStorage::lineBufferHandler(char* lineBuffer, FILE* output_pc)
{
    int status, addr, start_byte, end_byte = -1; //temporary vars used when parsing the command
    char command[16] = {0};
    //Value 1
    //Reads: used for address (byte or block), or start of range
    //Writes: used for address
    //Checksum: used for address if for single block, or start of byte range
    char value1 [32] = {0};
    //Value 2
    //Reads: end of byte range for range readingg
    //Writes: used for data
    //Checksum: used for end of byte range
    char value2 [240] = {0};

    //fprintf(pc, "--lineBufferHandler got line: %s--\n", lineBuffer);
    //TODO: sscanf or strtok or istream the lineBuffer into command and value(s)
    //      then process command and call appropriate UARTStorage functions

    int scan_count = sscanf(lineBuffer, "%s %s %s", command, value1, value2);

    //TODO: buffer overflow possible, use strtok or regex instead?
    fprintf(output_pc, "--lineBuffer command: %s--\n", command);
    fprintf(output_pc, "--lineBuffer value1: %s--\n", value1);\
    fprintf(output_pc, "--lineBuffer value2: %s--\n", value2);
    fprintf(output_pc, "--lineBuffer scan count: %d--\n", scan_count);

    //init, deinit, status
    if(scan_count == 1)
    {
        //Initialize the SPI flash
        if(strcmp("initspi", command) == 0)
        {
            //todo: allow for pins and freq to be specified
            //for now will use default in UARTStorage.h
            status = this->init_SPIFlash();
            fprintf(output_pc, "I: SPI Initialized %d\n", status);
        }
        else if(strcmp("deinitspi", command) == 0)
        {
            status = spif->deinit();
            fprintf(output_pc, "I: SPI Deinitialized %d\n", status);
        }
    }
    // readblk A, chksum A
    else if(scan_count == 2)
    {
        // read block A
        if(strcmp("readblk", command) == 0)
        {
            sscanf(value1, "%d", &addr);
            read_SPIF_Block(addr, SERIAL_DATA_CHUNKSIZE);
        }
        // Read a single byte
        else if(strcmp("readbyte", command) == 0)
        {
            uint8_t result = 0;
            uint8_t scanned = 0;
            scanned = sscanf(value1, "%d", &start_byte);
            if(scanned == 1)
            {
                fprintf(output_pc, "Reading single byte %d\n", start_byte);
                read_SPIF_Byte(&result, start_byte);
                //Write out as hex
                fprintf(output_pc, "D: %x\n", result);
            }
            //todo case of not getting value
        }
        //get checksum of block A
        else if(strcmp("chkblk", command) == 0)
        {
            
        }
    }
    //
    else if(scan_count == 3)
    {
        //read byte range from byte Start to End (inclusive)
        if(strcmp("readbytes", command) == 0)
        {
            sscanf(value1, "%d", &start_byte);
            sscanf(value2, "%d", &end_byte);
            //todo do read into buffer, then print out in chunks
        }
        //write byte range
        else if(strcmp("writebytes", command) == 0)
        {
            //get count of bytes

            //put in buffer, and then write output buffer to the device at the appropriate place
        }
        //write block
        else if(strcmp("writebytes", command) == 0)
        {
            //todo since we are writing block, have routine to fill up output buffer before doing any writes?

            //get count of bytes

            //put in buffer, and then write output buffer to the device at the appropriate place

        }
        //checksum a byte range from Start to End (inclusive)
        else if(strcmp("chkbytes", command) == 0)
        {
            
        }
    }
    else
    {
        fprintf(output_pc, "E: Invalid input\n");
    }

}


 UARTStorage::~UARTStorage()
 {
     if(spif != 0)
     {
         spif->deinit();
     }
 }