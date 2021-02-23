/**
 * @brief       serialCLI.cpp
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

#include "serialCLI.h"


serialCLI::serialCLI(BufferedSerial* buf_serial, FILE* serial_fd, Callback<void(char *)> lineBufferCallback)
{
    bufserial = buf_serial;
    pc = serial_fd;
    this->lineBufferCallback = lineBufferCallback;


    //really not sure what's going on here, lower case callback not in docs at all
    //According to callback.h, callback() returns a Callback class with inferred type given static function and argument
    //Possibly this would all be simpler if we made the thread loop method static, since then only one would exist
    serialCLI_RX_Thread.start(callback(this, &serialCLI::serialCLI_RX_Loop));
}
   

void serialCLI::serialCLI_RX_Loop()
{
    char serial_in[256]{0}; //default BufferedSerial circular buffer of 256 bytes each for Rx / Tx
    char lineBuffer[257]{0}; //buffer for whole line, with extra char for \0

    int chars_scanned = 0;
    int lineBufferIndex = 0;
    while(true)
    {
        chars_scanned = bufserial->read(serial_in, sizeof(serial_in));

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
                    lineBufferCallback(lineBuffer);
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
    }
}


serialCLI::~serialCLI()
{
    serialCLI_RX_Thread.terminate();
}