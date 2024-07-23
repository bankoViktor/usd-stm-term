/*
 * term.h
 *
 *  Created on: Jul 21, 2024
 *      Author: VBanko
 */

#ifndef INC_TERM_H_
#define INC_TERM_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"

/* Exported defines ----------------------------------------------------------*/

/**
 * Terminal Line Buffer length
 */
#define TERM_LINE_BUFFER_LEN    16

/**
 * Flags
 */
#define TERM_FLAG_RUNCMD        (1 << 0)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Enum of pressed key.
 */
typedef enum _TERM_KeyTypeDef
{
  TERM_KEY_NONE = 0,
  TERM_KEY_ARROW_UP,
  TERM_KEY_ARROW_DOWN,
  TERM_KEY_ARROW_RIGHT,
  TERM_KEY_ARROW_LEFT,
  TERM_KEY_END,
  TERM_KEY_HOME,
  TERM_KEY_PAGE_UP,
  TERM_KEY_PAGE_DOWN,
  TERM_KEY_DELETE,
  TERM_KEY_INSERT,
//  TERM_KEY_FUNC_1,
//  TERM_KEY_FUNC_2,
//  TERM_KEY_FUNC_3,
//  TERM_KEY_FUNC_4,
//  TERM_KEY_EX_CIRCLE,
//  TERM_KEY_EX_TRIANGLE,
//  TERM_KEY_EX_RECTANGLE,
//  TERM_KEY_EX_CROSS,
} TERM_KeyTypeDef;

/**
 * @brief Data of terminal.
 */
typedef struct _TERM_HandleTypeDef
{
  /* Line Coding *************************************************************/

  /**
   * @brief Data terminal rate, in bits per second.
   */
  uint32_t  dwRate;

  /**
   * @brief Stop bits:
   * 0 - 1 Stop bit;
   * 1 - 1.5 Stop bits
   * 2 - 2 Stop bits
   */
  uint8_t   bCharFormat;

  /**
   * @brief Parity
   * 0 - 1 Stop bit;
   * 1 - 1.5 Stop bits
   * 2 - 2 Stop bits
   */
  uint8_t   bParityType;

  /**
   * @brief Number Data bits (5, 6, 7, 8 or 16).
   */
  uint8_t   bDataBits;

  /**
   * @brief Reserver (Align).
   */
  uint8_t   bmFlags;

  /* Buffers  ****************************************************************/

  /**
   * Index of writing next received char.
   */
  uint32_t  dwWriteIndex;

  /**
   * Count of received chars.
   */
  uint32_t  dwReceived;

  /**
   * Buffer of received bytes.
   */
  uint8_t   abBuffer[TERM_LINE_BUFFER_LEN];

  /**
   * Pressed key.
   */
  TERM_KeyTypeDef eKeyPressed;

  uint8_t   bScreenColumns;
  uint8_t   bScreenRows;
  uint8_t   bRow;
  uint8_t   bColumn;

} TERM_HandleTypeDef;

/* Exported variables --------------------------------------------------------*/
extern TERM_HandleTypeDef hTerminal;

/* Exported functions --------------------------------------------------------*/
void TERM_Init(void);
void TERM_Receive(uint8_t* pabBuf, uint32_t dwLen);
void TERM_Handle(void);

#endif /* INC_TERM_H_ */
