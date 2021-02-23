#include "mbed.h"


//#include "SPIFBlockDevice.h"
//mbed studio sometimes thinks the library doesn't exist if only name given, in which case give full path to avoid complaints in IDE
//#include "mbed-os/storage/blockdevice/COMPONENT_SPIF/include/SPIF/SPIFBlockDevice.h"

#include <BufferedSerial.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

BufferedSerial bufserial(PA_9, PA_10, 115200);
FILE * pc;

// Constructor: MOSI, MISO, SCLK, CSEL, freq=40000000
// 40MHz speed too much for Super Blue Pill with W25Q16 (should be well within datasheet params for W25Q16 though)
// Max SPI speed on F103 is 18 MHz (datasheet), but 'unofficially' 36MHz https://stm32duinoforum.com/forum/viewtopic_f_17_t_2041.html
//SPIFBlockDevice spif(PA_7, PA_6, PA_5, PC_13, 30000000);



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

//todo record SPIF stats for a session
/*
class SPIF_Stats
{
    public:

    private:
        uint32_t erase_count = 0;
        uint32_t write_count = 0;
        uint32_t read_count = 0;
}; */


/*
// Writes SPIF stats to specified FILE (eg, buffered serial)
int write_SPIF_Stats(FILE* fd, SPIFBlockDevice& spifd)
{
    int status = -1;

    if(fd != 0)
    {
        status = spif.init();
        fprintf(fd, "spif init status: %d\n", status);
        fprintf(fd, "spif size: %llu\n",         spif.size());
        fprintf(fd, "spif read size: %llu\n",    spif.get_read_size());
        fprintf(fd, "spif program size: %llu\n", spif.get_program_size());
        fprintf(fd, "spif erase size: %llu\n",   spif.get_erase_size());
        fprintf(fd, "spif total device size: %llu\n", spif.size());
    }
    else{
        fprintf(fd, "E: spif not initialized!");
    }

    return status;
} */

//int erase_SPIF_Block(uint32_t blkaddr, SPIFBlockDevice& spifd){}

/*
void spif_test_program()
{
    int status;

    fprintf(pc, "\n\nspif test\n");

        // Initialize the SPI flash device, and print the memory layout
        status = spif.init();
        write_SPIF_Stats(pc, spif);

        //uint16_t erase_size = spif.get_erase_size();
        
        fprintf(pc, "\nErasing flash chip...\n");
        // Write "Hello World!" to the first block
        char *out_buffer = (char *)malloc(spif.get_erase_size());
        sprintf(out_buffer, "Hello World! test123\n");
        spif.erase(0, spif.get_erase_size());
        spif.program(out_buffer, 0, spif.get_erase_size());

        fprintf(pc, "\nReading back flash chip: \n");

        // Read back what was stored
        char *in_buffer = (char *)malloc(spif.get_erase_size());
        status = spif.read(in_buffer, 0, spif.get_erase_size());
        fprintf(pc, "spif read status: %d\n", status);
        fprintf(pc, "%s", in_buffer);

        // Deinitialize the devicev
        spif.deinit();
} */

void lineBufferHandler(char* lineBuffer)
{
    fprintf(pc, "--lineBufferHandler got line: %s--\n", lineBuffer);
    //TODO: sscanf or istream the chars 
}

// main() runs in its own thread in the OS
int main()
{
    pc = fdopen(&bufserial, "w");
    int status;
    
    char serial_in[256]{0}; //default BufferedSerial circular buffer of 256 bytes each for Rx / Tx
    char lineBuffer[257]{0}; //buffer for whole line, with extra char for \0


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



    int chars_scanned = 0;
    int lineBufferIndex = 0;
    while(true)
    {
        chars_scanned = bufserial.read(serial_in, sizeof(serial_in));

        // TODO: potential for buffer overflow of line_buffer if newline not sent within 256 characters
        // should be detectable by if( lineBufferLength < chars_scanned + lineBufferIndex )
        // to recover: throw an error and reset the line? stop accepting any more chars except backspace?
        
        //go through all the chars we just received
        // echo them back as appropriate
        // and also fill up a line buffer that we will send to a handler function when we hit a newline
        for(int i = 0; i < chars_scanned; i++)
        {
            //handle newline behaviour -> print back newline, call lineBuffer handler function
            if(serial_in[i] == '\r' || serial_in[i] == '\n')
            {
                    //handle CR LF or LF CR cases by skipping the second char of the sequence
                    if( serial_in[i+1] == '\n' || serial_in[i+1] == '\r')
                    {
                        i++;
                    }
                    fputc('\n', pc); //print back the newline
                    //fprintf(pc, "\n");

                    lineBuffer[lineBufferIndex++] = '\0'; //don't include the newline in the line buffer
                    lineBufferHandler(lineBuffer);
                    lineBufferIndex = 0;
            }
            //handle backspace behaviour - print back backspace, then a space to clear the character, then another backspace
            //for the lineBuffer, we just move the index back one character
            else if(serial_in[i] == '\b')
            {
                //don't back up if we are at the start of the line
                if(lineBufferIndex != 0)
                {
                    fprintf(pc, "\b \b"); //print backspace space backspace to move back a character and clear the character on the terminal
                    lineBuffer[lineBufferIndex--] = '\0'; //set the most recent char back to \0 and move the line buffer back a character
                }
            }
            // All text chars, print back and record in buffer
            // Space (0x20) is start of printable range, ~ (0x7E) is end
            else if (serial_in[i] >= ' ' && serial_in[i] <= '~')
            {
                fputc(serial_in[i], pc);
                lineBuffer[lineBufferIndex++] = serial_in[i];
            }
            //all others chars, ignore for now
            //Arrow keys, home, end, tab, DEL, etc
            else {
                //do nothing
            }
        }

        ThisThread::sleep_for(1ms);
        
    }


    uint16_t sleep_count = 0;
    while(true)
    {
        fprintf(pc, "Sleeping %u...\n", sleep_count);
        sleep_count++;
        ThisThread::sleep_for(30s);
    }
}

