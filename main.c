/*
 * main.c
 *
 *  Created on: Jun 22, 2020
 *      Author: Heba Ramadan Taha
 */
#include "../Lib/Std_types.h"
#include "../Lib/Bit_math.h"
#include "Lcd_int.h"
#include "Uart_int.h"

#define MAX_CHAR_IN_LINE 16
#define BACKSPACE  0x7F      /* ASSCII Of BACKSPACE IN HEX */
#define ESC        0x1B      /* ASSCII Of ESC_KEY   IN HEX */
#define ENTER      0x0D      /* ASSCII Of ENTER_KEY IN HEX */
#define TAP        0x20      /* ASSCII Of TAP_KEY   IN HEX */

/* Arrow_U { 'ESC' , '[' , 'A'  }           */
/* Arrow_D { 'ESC' , '[' , 'B'' }           */
/* Arrow_R { 'ESC' , '[' , 'C'  }           */
/* Arrow_L { 'ESC' , '[' , 'D'  }           */
/* DEL     { 'ESC' , '[' ,  0x33  , 0x7E}   */


int main(void)
{
	/* Declare Variable to save Received data via UART */
	u8 App_u8ReceivedData = 0;

	/* Flag Indicate Esc key	          */
	u8 App_u8ESCFlag = 0;

	/* Flag Indicate '[' key	          */
	u8 App_u8flag = 0;

	/* Flag Indicate DEL Key	          */
	u8 App_u8Delflag = 0;

	/* Initialize UART Communication Protocol           */
	Uart_vidInit();

	/* Initialize LCD_2*16_8Bit */
	Lcd_vidInit();

	while(1)
	{
		/* Receive Data from UART  */
		App_u8ReceivedData = Uart_u8ReceiveByte();


		/* Check if Received Data any Character unless 'ESC' 'ENTER' 'TAP'  'BACKSPACE'  '['  */
		if((App_u8ReceivedData != ESC) && (App_u8ReceivedData != ENTER) && (App_u8ReceivedData != TAP) && (App_u8ReceivedData != BACKSPACE) && (App_u8ReceivedData != UART_u8NO_RECEIVE) && (App_u8ESCFlag != 1))
		{
			/* Display Character */
			Lcd_vidWriteChar(App_u8ReceivedData);

		}


		/**************** Check if Received Data is 'ENTER' ****************/
		else if(App_u8ReceivedData == ENTER)
		{
			/* MOVR to the next Line */
			Lcd_vidSendCommand(0xC0);
		}


		/**************** Check if Received Data is 'TAP' ****************/
		else if(App_u8ReceivedData == TAP)
		{
			/* Print Space On LCD */
			Lcd_vidWriteChar(' ');
		}


		/************** Check if Received Data is 'BACKSPACE' *************/
		else if(App_u8ReceivedData == BACKSPACE)
		{
			/* Shift Cursor Left*/
			Lcd_vidSendCommand(0x10);
			/* Print Space On LCD instead character */
			Lcd_vidWriteChar(' ');
			/* Shift Cursor Left*/
			Lcd_vidSendCommand(0x10);
		}


		/**************** Check if Received Data is 'ESC' ****************/
		else if(App_u8ReceivedData == ESC)
		{
			/* Raise Esc flag*/
			App_u8ESCFlag   = 1;
		}


		/* Check ESC flag  */
		if(App_u8ESCFlag == 1)
		{
			/* Receive the second Byte */
			if(App_u8ReceivedData == '[')
			{
				/* Expect an Extend Character*/
				App_u8flag = 1;
			}
			/* If second Byte Not '[' that mean character is ESC_KEY NOT Extended */
			else if(App_u8ReceivedData == UART_u8NO_RECEIVE)
			{
				/*Clear Display And Return Cursor   */
				Lcd_vidSendCommand(0x01);
				/* Make flag = 0   */
				App_u8ESCFlag   = 0;
			}

		}

		/* Check Third Byte */
		if((App_u8ESCFlag == 1) && (App_u8flag == 1))
		{
			switch(App_u8ReceivedData)
			{
			case'A':  //Arrow Up
				/* Set Cursor At The end of line_1 */
				Lcd_vidSendCommand(0x8F);
				/* Make flag = 0   */
				App_u8ESCFlag   = 0;
				App_u8flag      = 0;
			break;

			case'B':  //Arrow Down
				/* Set Cursor At The first of line_2 */
				Lcd_vidSendCommand(0xC0);
				/* Make flag = 0   */
				App_u8ESCFlag   = 0;
				App_u8flag      = 0;
			break;

			case'C':  //Arrow RIGHT
				/* Shift Right Cursor */
				Lcd_vidSendCommand(0x14);
				App_u8ESCFlag   = 0;
				App_u8flag      = 0;
				break;

			case'D':  //Arrow LEFT
				/* ShiftLEFT Cursor */
				Lcd_vidSendCommand(0x10);
				/* Make flag = 0   */
				App_u8ESCFlag   = 0;
				App_u8flag      = 0;
			break;

			case 0x33 : // DEL Arrow
				App_u8Delflag = 1;
			break;

			}
		}
		/**************** Check if Received Data is 'DEL' ****************/
		if((App_u8ESCFlag == 1) && (App_u8flag == 1) && (App_u8Delflag == 1))
		{
			/* Check forth Byte*/
			if(App_u8ReceivedData == 0X7E)
			{
				/* Delete character  */
				Lcd_vidWriteChar(' ');
				/* ShiftLEFT Cursor */
				Lcd_vidSendCommand(0x10);
				/* Make flag = 0   */
				App_u8ESCFlag   = 0;
				App_u8flag      = 0;

			}

		}

	}

	return 0;
}


