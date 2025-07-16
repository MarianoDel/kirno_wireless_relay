//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FLASH_PROGRAM.C #######################
//---------------------------------------------

#include "flash_program.h"
#include "stm32f0xx.h"



/* Externals variables ---------------------------------------------------------*/
extern parameters_typedef mem_conf;


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


// extern parameters_typedef param_struct;
// extern mem_bkp_typedef memory_backup;
// extern filesystem_typedef files;
// extern unsigned int v_bkp [];

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//const int vmem [1024] ;
// unsigned int * pmem = (unsigned int *)0x08007000;	//Sector 7 Page 28


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void ErasePage(uint32_t , unsigned char );
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);

/* Module functions ---------------------------------------------------------*/
unsigned char WriteConfigurations (void)
{
    parameters_typedef * p_param;

    ErasePage(FLASH_PAGE_FOR_BKP,0);

    //update en memoria
    p_param = &mem_conf;

    if (WriteFlash((unsigned int *) p_param, FLASH_PAGE_FOR_BKP, 1, sizeof(parameters_typedef)/4) == FAILED)
        return FAILED;

    return PASSED;
}

unsigned char WriteFlash(unsigned int * p, uint32_t p_addr, unsigned char with_lock, unsigned char len_in_4)
{
    unsigned short i;
    unsigned int * p_verif;

    p_verif = (unsigned int *) p_addr;

    for (i = 0; i < len_in_4; i++)
    {
        FLASH_ProgramWord(p_addr, *(p + i));
        p_addr += 4;
    }

    if (with_lock)
        FLASH_Lock();

    //verifico memoria
    for (i = 0; i < len_in_4; i++)
    {
        if (*(p_verif + i) != *(p + i))
            return FAILED;
    }
    return PASSED;
}

void ErasePage(uint32_t p_addr, unsigned char with_lock)
{
	FLASH_Unlock();
	FLASH_ErasePage(p_addr);
	if (with_lock)
		FLASH_Lock();
}

/**
  * @brief  Unlocks the FLASH control register and program memory access.
  * @param  None
  * @retval None
  */
void FLASH_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    /* Unlocking the program memory access */
    FLASH->KEYR = FLASH_FKEY1;
    FLASH->KEYR = FLASH_FKEY2;
  }
}

/**
  * @brief  Locks the Program memory access.
  * @param  None
  * @retval None
  */
void FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH control register and program memory access */
  FLASH->CR |= FLASH_CR_LOCK;
}

/**
  * @brief  Erases a specified page in program memory.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @param  Page_Address: The page address in program memory to be erased.
  * @note   A Page is erased in the Program memory only if the address to load
  *         is the start address of a page (multiple of 1024 bytes).
  * @retval FLASH Status: The returned value can be:
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
//  assert_param(IS_FLASH_PROGRAM_ADDRESS(Page_Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to erase the page */
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR  = Page_Address;
    FLASH->CR |= FLASH_CR_STRT;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    /* Disable the PER Bit */
    FLASH->CR &= ~FLASH_CR_PER;
  }

  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Waits for a FLASH operation to complete or a TIMEOUT to occur.
  * @param  Timeout: FLASH programming Timeout
  * @retval FLASH Status: The returned value can be: FLASH_BUSY,
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check for the FLASH Status */
  status = FLASH_GetStatus();

  /* Wait for a FLASH operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = FLASH_GetStatus();
    Timeout--;
  }

  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}

/**
  * @brief  Returns the FLASH Status.
  * @param  None
  * @retval FLASH Status: The returned value can be:
  *         FLASH_BUSY, FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP or FLASH_COMPLETE.
  */
FLASH_Status FLASH_GetStatus(void)
{
  FLASH_Status FLASHstatus = FLASH_COMPLETE;

  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY)
  {
    FLASHstatus = FLASH_BUSY;
  }
  else
  {
    if((FLASH->SR & (uint32_t)FLASH_FLAG_WRPERR)!= (uint32_t)0x00)
    {
      FLASHstatus = FLASH_ERROR_WRP;
    }
    else
    {
      if((FLASH->SR & (uint32_t)(FLASH_SR_PGERR)) != (uint32_t)0x00)
      {
        FLASHstatus = FLASH_ERROR_PROGRAM;
      }
      else
      {
        FLASHstatus = FLASH_COMPLETE;
      }
    }
  }
  /* Return the FLASH Status */
  return FLASHstatus;
}

/**
  * @brief  Programs a word at a specified address.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  __IO uint32_t tmp = 0;

  /* Check the parameters */
//  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to program the new first
    half word */
    FLASH->CR |= FLASH_CR_PG;

    *(__IO uint16_t*)Address = (uint16_t)Data;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    if(status == FLASH_COMPLETE)
    {
      /* If the previous operation is completed, proceed to program the new second
      half word */
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

      /* Disable the PG Bit */
      FLASH->CR &= ~FLASH_CR_PG;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR &= ~FLASH_CR_PG;
    }
  }

  /* Return the Program Status */
  return status;
}
