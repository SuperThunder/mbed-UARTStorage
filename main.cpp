#include "mbed.h"
#include <BufferedSerial.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "serialCLI.h"
#include "UARTStorage.h"



BufferedSerial bufserial(PA_9, PA_10, 115200);
FILE * pc;

// Constructor: MOSI, MISO, SCLK, CSEL, freq=40000000
// 40MHz speed too much for Super Blue Pill with W25Q16 (should be well within datasheet params for W25Q16 though)
// Max SPI speed on F103 is 18 MHz (datasheet), but 'unofficially' 36MHz https://stm32duinoforum.com/forum/viewtopic_f_17_t_2041.html
//SPIFBlockDevice spif(PA_7, PA_6, PA_5, PC_13, 30000000);
// F411CEU6: official max speed is 25-50MHz depending on voltage and which SPI peripheral


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
    
    //Callback<void(char*)> lineBufferCallback = callback(&lineBufferHandler);
    serialCLI serial_cli(&bufserial, pc, &lineBufferHandler);

    ThisThread::sleep_for(1ms);
        

    while(true)
    {
        ThisThread::sleep_for(1s);
    }
}

