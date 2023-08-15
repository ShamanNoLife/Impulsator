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
#include "sleep.h"
#include "tim6.h"
#include "pulse_gen.h"
#include "flash.h"
//#include "PVD.H"
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
uint32_t Addr_num=0x0800FFA0;
uint32_t Addr_total_pulse=0x0800FFB0;
uint32_t Addr_Ton=0x0800FFC0;
uint32_t Addr_Toff=0x0800FFD0;
uint32_t Addr_freq=0x0800FFE0;
uint32_t Addr_duty_cycle=0x0800FFF0;
uint8_t value, bb;
uint32_t num,data,freq,duty_cycle,fdata1,fdata2,fdata3,fdata4,fdata5,fdata6,total_pulses=0;
float Ton,Toff,Period;
static char line_buffer[LINE_MAX_LENGTH + 1];
char* tokens[MAX_TOKENS];
static uint32_t line_length;
static int flag=3;
static int state;
static int pvd=1;
static char menu[]="\n\r---------------HOW TO USE---------------\n\rrun: Start a program\n\rstop: Stop a program\n\rcont: To continue\n\r";
static char error[]="\n\rWrong key\n\r";
static char error_with_run[]="\r\nrun num freq duty cycle(max 99) or run oo(infinitive) freq duty cycle(max 99)\n\r";
static char error_with_duty_cycle[]="\n\rToo high value of duty cycle\n\r";
static char error_with_lenght[]="Too long command";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE BEGIN PFP */
void Stop_mode(void);
void Start_STATUS_LED(void const * argument);
void START_IMPULSATOR(void const * argument);
void MENU_USB(uint8_t data);
void TIM6_init(void);
void GPIO_init(void);
void save_data(uint32_t Address,uint32_t data1);
void erase_data(uint32_t Address);
uint32_t read_data(uint32_t Address);
uint32_t ASCII_TO_uint8_t(const char *table);
void splitString(const char* input_string, char** tokens);
void TIM6_IRQHandler(void);
void TIM6_Callback(void);
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

void TIM6_Callback(void){
	if(state==1 || state==2){
		GPIOA->ODR^=(1U<<5);
	}
	else{
		GPIOA->ODR=(1U<<5);
	}
}

void HAL_PWR_PVDCallback(void){
    if (pvd==0) {
		pvd=1;
		state=3;
   		GPIOB->ODR=~(1U<<1);
   		GPIOA->ODR=~(1U<<5);
    }
    else{
    	pvd=0;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_13) {
	pvd=1;
	state=3;
	GPIOB->ODR=~(1U<<1);
	GPIOA->ODR=~(1U<<5);
  } else {
    __NOP();
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
  GPIO_init();
  TIM6_init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  printf(menu);
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
	  		  flag=2;
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
	  		  if(num>0 && state==2){
	  			  state=2;
	  			  flag=1;
	  		  }
	  		  else{
	  			  state=3;
	  		  }
	  		  break;
	  	  case 3:
	  		  save_data(Addr_total_pulse, total_pulses);
	  		  save_data(Addr_num, num);
	  		  save_data(Addr_Ton, (uint32_t)Ton);
	  		  save_data(Addr_Toff, (uint32_t)Toff);
	  		  save_data(Addr_freq, freq);
	  		  save_data(Addr_duty_cycle, duty_cycle);
	  		  state=4;
	  		  break;
	  	  case 4:
	  		  if(flag==1){
	  			  fdata1=read_data(Addr_num);
	  			  fdata2=read_data(Addr_total_pulse);
	  			  fdata3=read_data(Addr_Ton);
	  			  fdata4=read_data(Addr_Toff);
	  			  fdata5=read_data(Addr_freq);
	  			  fdata6=read_data(Addr_duty_cycle);
	  		  }
	  		  else{
	  			  fdata1=read_data(Addr_num);
	  			  fdata2=read_data(Addr_total_pulse);
	  			  fdata3=read_data(Addr_Ton);
	  			  fdata4=read_data(Addr_Toff);
	  			  fdata5=read_data(Addr_freq);
	  			  fdata6=read_data(Addr_duty_cycle);
	  		  }
	  		  num=0;
	  		  freq=0;
	  		  duty_cycle=0;
	  		  Ton=0;
	  		  Toff=0;
	  		  total_pulses=0;
	  		  if(pvd==0){
	  			  state=5;
	  		  }
	  		  else{
	  			  state=0;
	  		  }
	  		  break;
	  	  case 5:
	  		  sprintf(line_buffer,"Num:%lu,Total pulse:%lu,Ton:%lu,Toff:%lu,Freq:%lu,Duty_cycle:%lu\r\n",fdata1,fdata2,fdata3,fdata4,fdata5,fdata6);
	  		  HAL_UART_Transmit_IT (&huart2, (uint8_t*)line_buffer, strlen(line_buffer));
	  		  for(int i;i<strlen(line_buffer);i++){
	  			  line_buffer[i]='\0';
	  		  }
	  		  state=0;
	  		  break;
	  	  default:
	  		  break;
	  }
	if(pvd==1){
		Stop_mode();
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
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void GPIO_init(void){
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN|RCC_IOPENR_GPIOAEN|RCC_IOPENR_GPIOCEN;;

	GPIOA->MODER &= ((GPIOB->MODER & ~(GPIO_MODER_MODE5)) | (GPIO_MODER_MODE5_0));
	GPIOA->MODER &= ((GPIOB->MODER & ~(GPIO_MODER_MODE10)) | (GPIO_MODER_MODE10_0));

	GPIOB->MODER &= ((GPIOB->MODER & ~(GPIO_MODER_MODE1)) | (GPIO_MODER_MODE1_0));
	GPIOB->MODER &= ((GPIOB->MODER & ~(GPIO_MODER_MODE15)) | (GPIO_MODER_MODE15_0));
	GPIOB->MODER &= ((GPIOB->MODER & ~(GPIO_MODER_MODE3)) | (GPIO_MODER_MODE3_0));

	GPIOC -> MODER &= (GPIO_MODER_MODE4_0)|(GPIOC->MODER & ~GPIO_MODER_MODE4);

	GPIOC->BSRR = (1U<<4);
	GPIOB->ODR = (1U<<1);
}



void MENU_USB(uint8_t value){
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
										state=1;
								  		erase_data(Addr_total_pulse);
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
								  		erase_data(Addr_total_pulse);
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
					state=3;
				}
				else if (strcmp(line_buffer, "read") == 0){
					pvd=0;
					state=4;
				}
				else if (strcmp(line_buffer, "cont") == 0){
					if(flag==1){
                    	num=read_data(Addr_num);
                    	if(num!=0){
							total_pulses=read_data(Addr_total_pulse);
							Ton=read_data(Addr_Ton);
							Toff=read_data(Addr_Toff);
							duty_cycle=read_data(Addr_duty_cycle);
							freq=read_data(Addr_freq);
							erase_data(Addr_total_pulse);
							state=2;
                    	}
                    	else{
                    		state=0;
                    	}
                        flag=0;
					}
					else if(flag==2){
						if(read_data(Addr_Ton)>0){
							total_pulses=read_data(Addr_total_pulse);
							Ton=read_data(Addr_Ton);
							Toff=read_data(Addr_Toff);
							duty_cycle=read_data(Addr_duty_cycle);
							freq=read_data(Addr_freq);
							erase_data(Addr_total_pulse);
							state=1;
						}
						else{
							state=0;
						}
                      flag=0;
					}
				}
				else {
					printf(error);
					printf( menu);
				}
				printf("\n\r");
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
 		else if (value == '\177') {
 		            if (line_length > 0) {
 		            	line_length--;
 		            	printf("\177 \177");
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
	uint32_t numeric_value=0;
    int size = sizeof(table) / sizeof(char);
    for (int i = 0; i < size; i++) {
    	if(table[i]==' ' || table[i]=='\0'){
    		break;
    	}
    	else{
        	numeric_value = table[i] - '0';
            result = result * 10 + numeric_value;
    	}
    }
    return result;
}

void splitString(const char* input_string, char** tokens) {
	const char delimiters[] = " \r\n";
    char* copy_of_input = strdup(input_string);
    char* token = strtok(copy_of_input, delimiters);

    int token_count = 0;
    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count] = strdup(token);
        token = strtok(NULL, delimiters);
        token_count++;
    }
    free(copy_of_input);
}

void TIM6_IRQHandler(void){
	TIM6->SR &=~TIM_SR_UIF;
	if(pvd==0){
		TIM6_Callback();
	}
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