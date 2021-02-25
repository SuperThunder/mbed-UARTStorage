/**
 * @brief       serialCLI.h
 * @details     Simple serial console class that handles serial input in a thread and calls a specified callback function whenever a newline is encountered
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

 #ifndef SERIALCLI_H
 #define SERIALCLI_H

 #include "mbed.h"
 #include <cstdio>
 #include <BufferedSerial.h>


//Simple class that handles readline-ish things
class serialCLI
{
    public:
        //takes a file descriptor for your serial stream to write
        //and the bufserial object to read from
        //because I couldn't get mbed-os 6 to fscanf from the serial FILE
        serialCLI(BufferedSerial* buf_serial, FILE* serial_fd, Callback<void(char *, FILE*)> lineBufferCallback);

        ~serialCLI();

    private:
        //TODO: try to find out why scanf doesn't work on the FILE* pc
        BufferedSerial* bufserial;
        FILE* pc;
        Callback<void(char*, FILE*)> lineBufferCallback;

        Thread serialCLI_RX_Thread;
        void serialCLI_RX_Loop();

};


 #endif