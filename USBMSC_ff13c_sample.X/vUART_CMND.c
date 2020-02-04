/***************************************************************************//**
 * @file vUART_CMND.c
 * @brief	serial COMMAND input low level driver.
 *			use UART2.
 * @author hiroshi murakami
 * @date	20200204
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/
#define _vUART_CMND_C

#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include "integer.h"
#include "xprintf.h"
#include "vUART_CMND.h"

#define NULL_0 0
unsigned char cCmdBuf[CMND_BUFFER_SIZE + 1];	//command buffer, last 1 byte is for stored NULL
int iCIdx;		//command buffer insert position.

enum _eEOL {
	eCR = 0,
	eLF,
	eCRLF
};

enum _eEOL eEOL;


//******************************************************************************
/**	
 * @brief	Put a byte into TxFifo.
 * @param[in,out]	TxFifo	
 * @details	
 * Put a byte into TxFifo.
 * when TxBuff is full, this module wait until TxBuff has a space.
 */
void vPutc_to_TxFifo(unsigned char d)
{
	while (UART2_TransmitBufferIsFull() && (UART2_TransferStatusGet() & UART2_TRANSFER_STATUS_TX_FULL) )
	{
	}	// Wait while Tx FIFO is full

    if (d == '\n')
    {
        if (eEOL == eCRLF)
        {
        	UART2_Write('\r');
        	while (UART2_TransmitBufferIsFull())
            {
            }
        }
        else if (eEOL == eCR)
        {
            d = '\r';
        }
    }

	UART2_Write(d);
}


//******************************************************************************
/**	
 * @brief	UART initialization.
 * @param[out]	TxFifo	clear
 * @param[out]	TxBuff	clear
 * @details	
 * clear the TxFifo object and TxBuff
 */
void vUART_CMND_init(void)
{
	iCIdx = 0;							// command buffer clear 
	xdev_out(vPutc_to_TxFifo);			// join xPutc to xPutc_to_TxFifo
    
}


//******************************************************************************
//* get command strings, and shift command status
//* 1byte get from RX and insert command buffer
//* echo back to UART
//******************************************************************************
unsigned char ucExData;
unsigned char ucRxData;
enum eUART_CMND eUART_CMND_Getc(void)
{
	if(!UART2_ReceiveBufferIsEmpty()) 
	{

        ucExData = ucRxData;
		ucRxData = UART2_Read();
	    
	    if(ucExData == '\r' && ucRxData == '\r' )
	    {
	        eEOL = eCR;
	    }
	    else if (ucExData == '\n' && ucRxData == '\n' )
	    {
	        eEOL = eLF;
	    }
	    else if (ucExData == '\r' && ucRxData == '\n' )
	    {
	        eEOL = eCRLF;
	    }


		switch(ucRxData)	//check the data.
		{
		case NULL_0:
			break;

		case '\b':  // BS (Back Space)
			if(iCIdx > 0) 
			{
				iCIdx--;   //delete 1 byte from command buffer.
				//following logic mean clear 1 character on screen.
				xputs("\b \b");     //back to 1 byte and clear space.
			}
			break;

		case '\r':  // CR (unused)
			if (eEOL != eCR)
		    {
    			break;
		    }

		case '\n':  // LF (= 'Enter' mean finish command input)
			if(iCIdx < CMND_BUFFER_SIZE) 
			{
				//if there are some space in the command buffer...
				cCmdBuf[iCIdx] = NULL_0; //add NULL

			} else {
				//there isn't space in the command buffer 
				cCmdBuf[CMND_BUFFER_SIZE] = NULL_0;	//add NULL
			}
			iCIdx = 0;     //clear the command buffer
			xputs("\n");
			return eUART_CMND_Exec;
			break;

		case '\x7F':    // DEL (input cancel)
			iCIdx = 0 ;    //clear the command buffer
			xputs("\nDelete the Command\n");
			return eUART_CMND_Del;
			break;

		case '\x1B':    // ESC (Escape menu hierarchy)
			iCIdx = 0 ;    //clear the command buffer
			xputs("\nEscape the Command\n");
			return eUART_CMND_Esc;
			break;

		default:    // get command strings 1byte 1.
			if(iCIdx < CMND_BUFFER_SIZE)		//if there are some space in the command buffer..
			{
				cCmdBuf[iCIdx] = ucRxData;		//input 1byte in the command buffer
				xputc (ucRxData);	//echo back 1 byte to screen.
				iCIdx++;           //next position
			} else {
				iCIdx = 0 ;    //clear the command buffer
				xputs("\nCommand Over Flow\n");
				return eUART_CMND_Err;
			}
		}
	}
	return eUART_CMND_Wait;
}

