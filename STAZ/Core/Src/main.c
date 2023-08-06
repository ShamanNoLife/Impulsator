/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "uart4.h"
#include "pulse_gen.h"
#include "flash.h"
#include "PVD.H"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LINE_MAX_LENGTH 80
#define MAX_TOKENS 4

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static char menu[]="---------------HOW TO USE---------------\r\nrun: Start a program\r\nstop: Stop a program\r\nread: Read the last value before changing power supply\r\n";
static char menu_after_reset[]="\n\rProblem with USB occured\n\r---------------HOW TO USE---------------\n\r start new: give new parametrs\n\r continue: Continue from last pulse\n\r";
static char error[]="\r\nWrong key\r\n";
static char error_with_run[]="\r\nrun num freq duty cycle(max 99)\r\n";
static char error_with_duty_cycle[]="\r\nToo high value of duty cycle\r\n";
static char error_with_lenght[]="\r\nToo long command\n\r";
char new_line[]="\r\n";
uint32_t Addr1=0x0800FFA0;
//uint32_t Addr2=0x0800FFB0;
//uint32_t Addr3=0x0800FFC0;
uint8_t value,freq,duty_cycle,fdata;
uint32_t total_pulses=0;
uint32_t num,data;
float Ton,Toff,Period;
static char line_buffer[LINE_MAX_LENGTH + 1];
char* tokens[MAX_TOKENS];
char * ptr;
int result=1;
static uint32_t line_length;
static int state=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void PVD_init(void);
void HAL_PWR_PVDCallback(void);
uint8_t send_pulse_v1(uint8_t freg, uint8_t duty_cycle);
uint32_t send_pulse_v2(uint32_t num, uint8_t freg, uint8_t duty_cycle);

void MENU_USB(uint8_t data);
void MENU_AFTER_RESET(uint8_t data);
void PG_init(void);
void GPIO_LEDS(void);
void save_data(uint32_t Address,uint32_t data1,uint32_t data2,uint32_t data3,uint32_t data4);
uint8_t read_data(uint32_t Address);
uint32_t ACII_TO_uint8_t(const char *table);
void splitString(const char* input_string, char** tokens);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0);
    return 1;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if (huart == &huart2) {
	  MENU_USB(value);
	  HAL_UART_Receive_IT(&huart2, &value, 1);
 }
}

typedef enum {
    READY,
    RUNNING,
	READ,
    DONE
} CoroutineState;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  HAL_Init();

  /* USER CODE BEGIN Init */
  //PVD_init();
  PG_init();
  GPIO_LEDS();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, &value, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1){
	switch(state) {
	  case 1:
	    GPIOC->BSRR = ~(1U<<4);
	    while((GPIOC->ODR & GPIO_ODR_OD4)){}
	    HAL_Delay((uint32_t)Ton);
	    GPIOC->BSRR = (1U<<4);
	    while(!(GPIOC->ODR & GPIO_ODR_OD4)){}
	    HAL_Delay((uint32_t)Toff);
	    total_pulses++;
	    if(state!=0){
	    state=1;
	    }
	    else{
	    	state=0;
	    }
	    break;
	  case 2:
	    GPIOC->BSRR = ~(1U<<4);
	    while((GPIOC->ODR & GPIO_ODR_OD4)){}
	    HAL_Delay((uint32_t)Ton);
	    GPIOC->BSRR = (1U<<4);
	    while(!(GPIOC->ODR & GPIO_ODR_OD4)){}
	    HAL_Delay((uint32_t)Toff);
	    total_pulses++;
	    num--;
	    if(num>0){
	    	state=2;
	    	}
	    else{
	    	state=0;
	    	}
	    break;
	  case 3:
	    //fdata=100;
	    //HAL_UART_Transmit(&huart2, &value, 1, 0);
	    state=0;
	    break;
	  default:
		  total_pulses=0;
		  state=0;
		  }
	  	  break;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void GPIO_LEDS(void){
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	//2
	GPIOB->MODER &= ((GPIOA->MODER & ~(GPIO_MODER_MODE2)) | (GPIO_MODER_MODE2_0));
	//1
	GPIOB->MODER &= ((GPIOA->MODER & ~(GPIO_MODER_MODE1)) | (GPIO_MODER_MODE1_0));
	//15
	GPIOB->MODER &= ((GPIOA->MODER & ~(GPIO_MODER_MODE15)) | (GPIO_MODER_MODE15_0));
}

void MENU_USB(uint8_t value){
 		if (value == '\r' || value == '\n') {
			if (line_length > 0) {
				line_buffer[line_length] = '\0';
				splitString(line_buffer, tokens);
				if (strncmp(line_buffer, "run",3) == 0) {
					ptr=strpbrk(tokens[1], "oo");
					if(!(ptr==NULL)){
//						for(int i=2;i<4;i++){
//							for(int j=0;j<strlen(tokens[i]);j++){
//								result*= isdigit(tokens[i][j])/2048;
//							}
//						}
							freq=ACII_TO_uint8_t(tokens[MAX_TOKENS-2]);
							duty_cycle=ACII_TO_uint8_t(tokens[MAX_TOKENS-1]);
							if(freq==0 || duty_cycle==0){
								printf(error_with_run);
							}
							else if(duty_cycle>=100){
								printf(error_with_duty_cycle);
							}
							else{
							  	Period=(float)(1000/freq);
							  	Ton=(float)((Period*duty_cycle)/100);
							  	Toff=Period-Ton;
								state=1;
							}
					}
					else{
//							for(int i=1;i<4;i++){
//									result*= isdigit(tokens[i])/2048;
//								}
//							}
							if(result!=0){
								num=ACII_TO_uint8_t(tokens[MAX_TOKENS-3]);
								freq=ACII_TO_uint8_t(tokens[MAX_TOKENS-2]);
								duty_cycle=ACII_TO_uint8_t(tokens[MAX_TOKENS-1]);
								if(num==0 || freq==0 || duty_cycle==0 ){
									printf(error_with_run);
								}
								else if(duty_cycle>=100){
									printf(error_with_duty_cycle);
								}
								else{
								  	Period=(float)(1000/freq);
								  	Ton=(float)((Period*duty_cycle)/100);
								  	Toff=Period-Ton;
								  	state=2;
								}
							}
							else{
								printf(error_with_run);
							}
					}
					for (int i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++) {
						 free(tokens[i]);
					}

				}
				else if (strncmp(line_buffer, "stop",4) == 0) {
					state=0;
				}
				else if (strncmp(line_buffer, "read",4) == 0) {
					state=3;
				}
			}
				else {
					printf(error);
					printf(menu);
				}
			for(int i;i<line_length;i++){
				line_buffer[i]='\0';
				}
				line_length = 0;
		}
		else {
			if (line_length >= LINE_MAX_LENGTH) {
				line_length = 0;
				printf(error);
				printf(error_with_lenght);
			}
			line_buffer[line_length++] = value;
		}
}

void MENU_AFTER_RESET (uint8_t value){

	if (value == '\r' || value == '\n') {
		if (strcmp(line_buffer, "start new") == 0) {

			}
		else if (strcmp(line_buffer, "continue") == 0) {
				if(read_data(Addr1+1)){
					data=send_pulse_v2(read_data(Addr1)-read_data(Addr1+4), read_data(Addr1+8), read_data(Addr1+12));
				}
				else{
					data=send_pulse_v1(read_data(Addr1+8), read_data(Addr1+12));
				}
			}
		else{

		}
	}
	else {
		if (line_length >= LINE_MAX_LENGTH) {
			line_length = 0;
			printf(error);
			printf(error_with_lenght);
		}
		line_buffer[line_length++] = value;
	}


}

uint32_t ACII_TO_uint8_t(const char *table){
	uint32_t result = 0;
	uint32_t numvalue=0;
    int size = sizeof(table) / sizeof(char);
    for (int i = 0; i < size-1; i++) {
    	if(table[i]==' ' || table[i]=='\0'){
    		break;
    	}
    	else{
    		numvalue = table[i] - '0';
            result = result * 10 + numvalue;
    	}
    }
    return result;
}

void splitString(const char* input_string, char** tokens) {
	const char spacebar[] = " \r\n";
    char* copy_of_input = strdup(input_string);
    char* token = strtok(copy_of_input, spacebar);

    int token_count = 0;
    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count] = strdup(token);
        token = strtok(NULL, spacebar);
        token_count++;
    }
    free(copy_of_input);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
