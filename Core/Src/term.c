/*
 * term.c
 *
 *  Created on: Jul 21, 2024
 *      Author: VBanko
 */

/* Includes ------------------------------------------------------------------*/
#include "term.h"
#include "stdio.h"
#include "usbd_cdc_if.h"
#include "string.h"

/* Private functions prototypes ---------------------------------------------*/
static inline TERM_HandleTypeDef* TERM_GetInstance(void);
static inline void TERM_PutChar(uint8_t ch);
static inline void TERM_Transmit(uint8_t* pabBuff, uint32_t dwLen);
//static void TERM_Execute(uint8_t* pszCmd);
//static void TERM_SendInputRequest(void);
static void TERM_HandleEscapeSequence(uint8_t* pabBuf, uint32_t dwLen);
static void TERM_HandleUserInput(void);
static void TERM_UpdateScreen(void);

/* Exported variables --------------------------------------------------------*/
TERM_HandleTypeDef hTerminal;

/* Exported functions implementations ----------------------------------------*/
void TERM_Init(void)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  pHandle->dwRate = 9600;
  pHandle->bCharFormat = 0;
  pHandle->bParityType = 0;
  pHandle->bDataBits = 8;

  pHandle->abBuffer[0] = 0;
  pHandle->dwWriteIndex = 0;
  pHandle->dwReceived = 0;
  pHandle->bmFlags = 0;
  pHandle->eKeyPressed = TERM_KEY_NONE;

  pHandle->bScreenColumns = 80;
  pHandle->bScreenRows = 24;
  pHandle->bRow = 0;
  pHandle->bColumn = 0;
}

void TERM_Receive(uint8_t* pabBuf, uint32_t dwLen)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  if (dwLen == 0)
    return;

  uint8_t ch = pabBuf[0];
  if (ch == '\r')
  {
    pHandle->bmFlags |= TERM_FLAG_RUNCMD;
  }
  else if (ch == '\e')
  {
    TERM_HandleEscapeSequence(pabBuf, dwLen);
  }
  else
  {
    for (uint8_t i = 0; i < dwLen; i++)
    {
      uint8_t ch = pabBuf[i];
      TERM_PutChar(ch);
    }
  }
}

void TERM_Handle(void)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  // Echo
//  if (pHandle->dwReceived > 0)
//  {
//    uint8_t* pBuf = pHandle->abBuffer + (pHandle->dwWriteIndex - pHandle->dwReceived);
//    TERM_Transmit(pBuf, pHandle->dwReceived);
//    pHandle->dwReceived = 0;
//  }

  // Escape Sequence
  if (pHandle->eKeyPressed != TERM_KEY_NONE)
  {
    TERM_HandleUserInput();

    TERM_UpdateScreen();

    pHandle->eKeyPressed = TERM_KEY_NONE;
  }

  // Run command line
//  if (pHandle->bmFlags & TERM_FLAG_RUNCMD)
//  {
//    pHandle->bmFlags &= ~TERM_FLAG_RUNCMD;
//
//    uint8_t abBuff[TERM_LINE_BUFFER_LEN] = {0};
//    uint8_t i = 0;
//    for (; i < pHandle->dwWriteIndex; i++)
//    {
//      uint8_t srcChar = pHandle->abBuffer[i];
//      if (srcChar == '\r')
//      {
//        break;
//      }
//      abBuff[i] = srcChar;
//    }
//    abBuff[i] = '\0';
//    pHandle->dwWriteIndex = 0;
//
//    //TERM_Execute(abBuff);
//    //TERM_SendInputRequest();
//  }
}

/* Private functions implementations -----------------------------------------*/

//static void TERM_Execute(uint8_t* pszCmd)
//{
//  if (strlen((char*)pszCmd) == 0)
//    return;
//
//  if (strcmp((char*)pszCmd, "hi") == 0)
//  {
//    const char* pszText = "\n\rHello, I'm STM terminal.";
//    TERM_Transmit((uint8_t*)pszText, (uint32_t)strlen(pszText));
//  }
//  else if (strcmp((char*)pszCmd, "version") == 0)
//  {
//    const char* pszText = "\n\rVersion: 0.1 (beta) \\ 22.09.2024";
//    TERM_Transmit((uint8_t*)pszText, (uint32_t)strlen(pszText));
//  }
//  else
//  {
//    const char* pszText = "\n\rError: unknow command";
//    TERM_Transmit((uint8_t*)pszText, (uint32_t)strlen(pszText));
//  }
//
//  //TERM_HandleTypeDef* pHandle = TERM_GetInstance();
//  // hi
//  // version
//  // help
//  // clr
//  // speed 150|200|500|1000 (CAN, Mb/s)
//  // filter no|id|mask (CAN)
//  // run
//  // stop
//  // send (CAN) S|E 0x065D 01 02 03 04 05 06 07 08
//}

static inline void TERM_Transmit(uint8_t* pabBuff, uint32_t dwLen)
{
  CDC_Transmit_FS(pabBuff, dwLen);
}

static void TERM_HandleEscapeSequence(uint8_t* pabBuf, uint32_t dwLen)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  if (dwLen == 0 && pabBuf[0] != '\e')
    return;

  uint8_t ch1 = pabBuf[1];
  switch (ch1)
  {
    case '[': // Control Sequence Introducer
      if (dwLen > 1)
      {
        uint8_t ch2 = pabBuf[2];

        // xterm Sequence
        if (ch2 == 'A')
          pHandle->eKeyPressed = TERM_KEY_ARROW_UP;
        else if (ch2 == 'B')
          pHandle->eKeyPressed = TERM_KEY_ARROW_DOWN;
        else if (ch2 == 'C')
          pHandle->eKeyPressed = TERM_KEY_ARROW_RIGHT;
        else if (ch2 == 'D')
          pHandle->eKeyPressed = TERM_KEY_ARROW_LEFT;
        else if (ch2 == 'F')
          pHandle->eKeyPressed = TERM_KEY_END;
        else if (ch2 == 'H')
          pHandle->eKeyPressed = TERM_KEY_HOME;
//        else if (ch2 == '1' && dwLen > 2)
//        {
//          uint8_t ch3 = pabBuf[3];
//          if (ch3 == 'P')
//            pHandle->eKeyPressed = TERM_KEY_FUNC_1;
//          else if (ch3 == 'Q')
//            pHandle->eKeyPressed = TERM_KEY_FUNC_2;
//          else if (ch3 == 'R')
//            pHandle->eKeyPressed = TERM_KEY_FUNC_3;
//          else if (ch3 == 'S')
//            pHandle->eKeyPressed = TERM_KEY_FUNC_4;
//        }

        // VT Sequence
        else if (dwLen > 2 && pabBuf[3] == '~')
        {
          if (ch2 == '2')
            pHandle->eKeyPressed = TERM_KEY_INSERT;
          else if (ch2 == '3')
            pHandle->eKeyPressed = TERM_KEY_DELETE;
          else if (ch2 == '5')
            pHandle->eKeyPressed = TERM_KEY_PAGE_UP;
          else if (ch2 == '6')
            pHandle->eKeyPressed = TERM_KEY_PAGE_DOWN;
        }
      }
      break;
  }
}

static void TERM_HandleUserInput(void)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  switch (pHandle->eKeyPressed)
  {
    case TERM_KEY_ARROW_UP:
      if (pHandle->bRow > 0)
      {
        pHandle->bRow -= 1;
      }
      break;

    case TERM_KEY_ARROW_DOWN:
      if (pHandle->bRow < pHandle->bScreenRows - 1)
      {
        pHandle->bRow += 1;
      }
      break;

    case TERM_KEY_ARROW_RIGHT:
      if (pHandle->bColumn < pHandle->bScreenColumns - 1)
      {
        pHandle->bColumn += 1;
      }
      break;

    case TERM_KEY_ARROW_LEFT:
      if (pHandle->bColumn > 0)
      {
        pHandle->bColumn -= 1;
      }
      break;

    default:
      break;
  }
}

static void TERM_UpdateScreen(void)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  uint32_t dwTotalLen = 0;
  uint32_t dwLen = 0;

  // Clear & reset cursor position
  dwLen = snprintf((char*)&UserTxBufferFS, APP_TX_DATA_SIZE - dwTotalLen,
    "\e[?25l"   // Hide Cursor
    "\e[H"      // Set Cursor Position [1,1]
  );
  if (dwLen < 1) goto error;
  dwTotalLen += dwLen;

  // Content Rows
  for (uint8_t bRow = 0; bRow < pHandle->bScreenRows - 1; bRow++)
  {
    dwLen = snprintf((char*)&UserTxBufferFS + dwTotalLen, APP_TX_DATA_SIZE - dwTotalLen,
      "~"
      "\e[K"    // Erase in Line, after curson
      "\r\n"
    );
    if (dwLen < 1) goto error;
    dwTotalLen += dwLen;
  }

  // Last Row - Status Bar
  dwLen = snprintf((char*)&UserTxBufferFS + dwTotalLen, APP_TX_DATA_SIZE - dwTotalLen,
    "\e[7m"     // Set BG color
    "STM32F103, %liMHz - v0.0.1",
    (uint32_t)SystemCoreClock / 1000000
  );
  if (dwLen < 1) goto error;
  dwTotalLen += dwLen;

  // Fill left Columns of last Row
  uint32_t dwLeftLineLen = pHandle->bScreenColumns - dwLen;
  memset((void*)&UserTxBufferFS + dwTotalLen, ' ', dwLeftLineLen);
  dwTotalLen += dwLeftLineLen;

  // Set Cursor Position [x,y]
  dwLen = snprintf((char*)&UserTxBufferFS + dwTotalLen, APP_TX_DATA_SIZE - dwTotalLen,
    "\e[0m"           // Reset BG color
    "\e[?25h"
    "\e[%i;%iH",      // Set Cursor Position [x,y]
    pHandle->bRow + 1,
    pHandle->bColumn + 1
  );
  if (dwLen < 1) goto error;
  dwTotalLen += dwLen;

  TERM_Transmit((uint8_t*)&UserTxBufferFS, dwTotalLen);
  return;

  error:;
  printf("TX Buffer Overflow\r\n");
}

static inline TERM_HandleTypeDef* TERM_GetInstance(void)
{
  return &hTerminal;
}

static inline void TERM_PutChar(uint8_t ch)
{
  TERM_HandleTypeDef* pHandle = TERM_GetInstance();

  if (pHandle->dwWriteIndex < TERM_LINE_BUFFER_LEN)
  {
    pHandle->abBuffer[pHandle->dwWriteIndex] = ch;
    pHandle->dwWriteIndex++;
    pHandle->dwReceived++;
  }
}

//static void TERM_SendInputRequest(void)
//{
//  const char* pszText = "\n\rstm>";
//  TERM_Transmit((uint8_t*)pszText, (uint32_t)strlen(pszText));
//}
