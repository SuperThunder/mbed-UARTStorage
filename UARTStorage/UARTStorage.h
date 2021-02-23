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

#include <string>
#include <sstream>
#include <map>


class UARTStorage
{
    public:
        UARTStorage();


        ~UARTStorage();

    private:
        Thread _thread_serialProcess;

        // parses the input from serial
        void inputProcessThread();
};


 #endif
