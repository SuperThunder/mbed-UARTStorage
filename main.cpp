#include "mbed.h"
#include <BufferedSerial.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "serialCLI.h"
#include "UARTStorage.h"

// 40MHz speed too much for Super Blue Pill with W25Q16 (should be well within datasheet params for W25Q16 though)
// Max SPI speed on F103 is 18 MHz (datasheet), but 'unofficially' 36MHz https://stm32duinoforum.com/forum/viewtopic_f_17_t_2041.html
// F411CEU6: official max speed is 25-50MHz depending on voltage and which SPI peripheral


UARTStorage storage;
BufferedSerial bufserial(PA_2, PA_3, 115200);


int main()
{
    
    FILE *pc ;
    pc = fdopen(&bufserial, "w");
    int status;

    fprintf(pc, "UARTStorage demo\n");
    ThisThread::sleep_for(100ms);

    //mosi, miso, sclk, CS, freq
    storage.init_SPIFlash(PA_7, PB_4, PA_5, PA_4, 1000000);

    storage.spif_test_program(pc);
    
    //https://os.mbed.com/questions/77791/Correct-construction-for-callback-on-a-m/
    //The handler that serialCLI calls when \n encountered is a member of the UARTStorage class, so the callback for it has be in "class method invocation" form
    Callback<void(char*, FILE*)> lineBufferCallback = callback(&storage, &UARTStorage::lineBufferHandler);
    serialCLI serial_cli(&bufserial, pc, lineBufferCallback);
        

    while(true)
    {
        //fprintf(pc, "UARTStorage demo\n");
        ThisThread::sleep_for(1s);
    }
}

