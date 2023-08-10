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
uint32_t Addr_total_pulse=0x0800FFA0;
uint32_t Addr_num=0x0800FFB0;
uint32_t Addr_Ton=0x0800FFC0;
uint32_t Addr_Toff=0x0800FFD0;
uint8_t value;
uint32_t num,data,fdata1,fdata2,fdata3,freq,duty_cycle,total_pulses=0;
float Ton,Toff,Period;
static char line_buffer[LINE_MAX_LENGTH + 1];
char* tokens[MAX_TOKENS];
static uint32_t line_length;
static int state=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void PVD_init(void);
void Stop_mode(void);
void MENU_USB(uint8_t data);
void MENU_PVD(uint8_t data);
void PG_init(void);
void GPIO_LEDS(void);
void save_data(uint32_t Address,uint32_t data);
uint32_t read_data(uint32_t Address);
void erase_data(uint32_t Address);
uint32_t ASCII_TO_uint8_t(const char *table);
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
	  HAL_UART_Transmit(&huart2, &value, 1, 0);
 }
}

void HAL_PWR_PVDCallback(void){
    if (PWR->CSR & PWR_CSR_PVDO) {
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		save_data(Addr_total_pulse, total_pulses);
		save_data(Addr_num, num);
		save_data(Addr_Ton, (uint32_t)Ton);
		save_data(Addr_Toff, (uint32_t)Toff);
		//Stop_mode();
    }
}

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
  PVD_init();
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
  printf("dupa");
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
	    if(state!=0 && state!=4){
	    state=1;
	    }
	    else{
	    	state=4;
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
	    if(num>0 && state!=0 && state!=4){
	    	state=2;
	    	}
	    else{
	    	state=4;
	    	}
	    break;
	  case 3:

		state=0;
		break;
	  case 4:
		erase_data(Addr_total_pulse);
		state=0;
		break;
	  default:
		//  printf("dupa");
	  	break;
	  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
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

void PG_init(void){
	RCC->IOPENR  |= RCC_IOPENR_GPIOCEN;
	GPIOC -> MODER = (GPIO_MODER_MODE4_0)|(GPIOC->MODER & ~GPIO_MODER_MODE4);
	GPIOC->BSRR = (1U<<4);
}

void MENU_USB(uint8_t value){
char menu[]="---------------HOW TO USE---------------"
"\r\nrun: Start a program\r\nstop: Stop a program\r\nread:"
"Read the last value before changing power supply\r\n"
"\r\ncont: To continue\r\n";
char error[]="\r\nWrong key\r\n";
char error_with_run[]="\r\nrun num freq duty cycle(max 99)\r\n";
char error_with_duty_cycle[]="\r\nToo high value of duty cycle\r\n";
char error_with_lenght[]="\r\nToo long command\n\r";
int result;
char * ptr;
 		if (value == '\r' || value == '\n') {
			if (line_length > 0) {
				line_buffer[line_length] = '\0';
					if (strncmp(line_buffer, "run",3) == 0) {
						splitString(line_buffer, tokens);
						ptr=strpbrk(tokens[1], "oo");
						if(!(ptr==NULL)){
							for(int i=2;i<MAX_TOKENS;i++){
								result=1;
								for(int j=0;j<strlen(tokens[i]);j++){
									if(!isdigit(tokens[i][j])){
										result=0;
									}
								}
								if(result==0){
									break;
								}
							}
							if(result!=0){
									freq=ASCII_TO_uint8_t(tokens[MAX_TOKENS-2]);
									duty_cycle=ASCII_TO_uint8_t(tokens[MAX_TOKENS-1]);
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
//										save_data(Addr_Ton, (uint32_t)Ton);
//										save_data(Addr_Toff, (uint32_t)Toff);
										state=1;
									}
							}
							else{
								printf(error_with_run);
							}
						}
						else{
							for(int i=1;i<MAX_TOKENS;i++){
								result=1;
								for(int j=0;j<strlen(tokens[i]);j++){
									if(!isdigit(tokens[i][j])){
										result=0;
									}
								}
								if(result==0){
									break;
								}
							}
								if(result!=0){
									num=ASCII_TO_uint8_t(tokens[MAX_TOKENS-3]);
									freq=ASCII_TO_uint8_t(tokens[MAX_TOKENS-2]);
									duty_cycle=ASCII_TO_uint8_t(tokens[MAX_TOKENS-1]);
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
//										save_data(Addr_num, num);
//										save_data(Addr_Ton, (uint32_t)Ton);
//										save_data(Addr_Toff, (uint32_t)Toff);
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
				else if (strcmp(line_buffer, "stop") == 0){
					state=4;
				}
				else if (strcmp(line_buffer, "read") == 0){
					if(read_data(Addr_num)>0){
						fdata1=read_data(Addr_num)-read_data(Addr_total_pulse);
						fdata2=read_data(Addr_Ton);
						fdata3=read_data(Addr_Toff);
					}
					else{
						fdata1=read_data(Addr_total_pulse);
						fdata2=read_data(Addr_Ton);
						fdata3=read_data(Addr_Toff);
					}
				    char ch1[30];
				    char ch2[30];
				    char ch3[30];

				    printf("\n\r Pulses: ");
				    sprintf(ch1, "%lu", fdata1);
				    HAL_UART_Transmit(&huart2, (uint8_t*)ch1, strlen(ch1), 200);

				    printf("\n\r Ton");
				    sprintf(ch2, "%lu", fdata2);
				    HAL_UART_Transmit(&huart2, (uint8_t*)ch2, strlen(ch2), 200);

				    printf("\n\r Toff");
				    sprintf(ch3, "%lu", fdata3);
				    HAL_UART_Transmit(&huart2, (uint8_t*)ch3, strlen(ch3), 200);

				}
				else if (strcmp(line_buffer, "cont") == 0){
					if(read_data(Addr_num)>0){
						num=read_data(Addr_num)-read_data(Addr_total_pulse);
						Ton=read_data(Addr_Ton);
						Toff=read_data(Addr_Toff);
						state=2;
					}
					else{
						total_pulses=read_data(Addr_total_pulse);
						Ton=read_data(Addr_Ton);
						Toff=read_data(Addr_Toff);
						state=1;
					}
				}
				else {
					printf(error);
					printf(menu);
				}
				printf("\r\n");
				for(int i;i<line_length;i++){
					line_buffer[i]='\0';
					}
				line_length = 0;
			}
			else{
				printf(error);
				printf(menu);
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

uint32_t ASCII_TO_uint8_t(const char *table){
	uint32_t result = 0;
	uint32_t numvalue=0;
    int size = sizeof(table) / sizeof(char);
    for (int i = 0; i < size; i++) {
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
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
