/**
  ******************************************************************************
  * @file    main.c 
  * @author  Postman
  * @brief   Display module, supplying the interaction between
  *          Ethernet based on LWIP and LED
  *          
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define BUF_SIZE 1024
#define STM32_UNIQUE_ID_SIZE 12 // UID长度：96bit = 12 * 8bit

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif gnetif; /* network interface structure */
/* Semaphore to signal Ethernet Link state update */
osSemaphoreId Netif_LinkSemaphore = NULL;
/* Ethernet link thread Argument */
struct link_str link_arg;

/* Private function prototypes -----------------------------------------------*/
static void StartThread(void const * argument);
static void led_opt_thread(void const * argument);
static void led_refresh_thread(void const * argument);
static void udp_send_thread(void const * argument);
static void udpClient_thread(void const * argument);
static void udpServer_thread(void const * argument);
static void readStm32UniqueID(u8 *stm32_uniqueId);
static void SystemClock_Config(void);
static void Netif_Config(void);

/* Global Variable----------------------------------------------------------- */
/* LED显示数据 */
float ConcHr = 0;
float ConcRt = 0;
float AT = 0;
float RH = 0;
float BP = 0;
float Longitude = 0;
float Latitude = 0;
float WindSpeed = 0;
float WindDir = 0;
float Noise = 0;
char flag_display = 0;
char flag_ConcHr = 0;
char flag_ConcRt = 0;
char flag_AT = 0;
char flag_RH = 0;
char flag_BP = 0;
char flag_Longitude = 0;
char flag_Latitude = 0;
char flag_WindSpeed = 0;
char flag_WindDir = 0;
char flag_Noise = 0;


u8_t macAddr[6];        // 48位
char recv_buf[BUF_SIZE] = {"hello server"};// 接收缓存数组 
char send_buf[BUF_SIZE];// 发送缓存数据
struct udp_pcb *upcb;   // 以太网变量
uint32_t message_count = 0; // UDP接收计数

/* main function ------------------------------------------------------------ */

int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  
  HAL_Init();
  
  SystemClock_Config(); //150 MHz
  
  osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(Start), NULL);
  
  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */
  for( ;; )
  {
    /* Delete the Init Thread */ 
    osThreadTerminate(NULL);
  }
  
}


/* 用户函数定义 ------------------------------------------------------------- */

static void StartThread(void const * argument)
{    
  led_init();
  
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);
  
  /* Initialize the LwIP stack */
  Netif_Config();
  
//  /* Start udp client thread */
//  osThreadDef(udpClient, udpClient_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
//  osThreadCreate(osThread(udpClient), &gnetif);

  /* Start udp server thread */
  osThreadDef(udpServer, udpServer_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(udpServer), &gnetif);
  
  /* start led content thread */
  osThreadDef(led_opt, led_opt_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(led_opt), NULL);
  
  /* start led refresh thread */
  osThreadDef(led_scan, led_refresh_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(led_scan), NULL);
  
  for( ;; )
  {
    /* Delete the Init Thread */
    osThreadTerminate(NULL);
  }
}

static void udpServer_thread(void const * argument)
{
  
  if(udp_server_init() != 0 )
    return;
  
  while (1)
  {
    osDelay(1);
  }
  
}

static void udpClient_thread(void const * argument)
{

//  u8 stm32_uniqueId[STM32_UNIQUE_ID_SIZE] = {0}; 
//  
//  /* 利用 UID 修改 MAC Address */
//  readStm32UniqueID(stm32_uniqueId);
//  /* UID有96bit，mac有48bit，确保mac的唯一性，将高低位求和 */
//  for (u8_t i = 0; i < 6; i++)
//  {
//    macAddr[i] = stm32_uniqueId[i]+stm32_uniqueId[i+6];
//  }
  
  if(udp_connect_to_server() != 0)
    return;
    
  /* Start the udp_send thread */
  osThreadDef(udp_send_to_server, udp_send_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(udp_send_to_server), &gnetif);
  
  while(1)
  {
    osDelay(1);
  }
}

static void udp_send_thread(void const * argument)
{
  /* 数据发送正常, commented by postman in 20190703 */
//  static int n = 0;
  while(1)
  {
//    sprintf(send_buf, "the %dth msg sending to server, the %dth msg received from server", n++, (int)message_count);    
//    udp_client_send(send_buf);
    
    udp_client_send(recv_buf);
    
    vTaskDelay(1000/portTICK_RATE_MS);

  }

}

static void led_opt_thread(void const * argument)
{  
  while(1)
  {
    led_display();
    osDelay(1);
  }
}

static void led_refresh_thread(void const * argument)
{ 
  while(1)
  {
    led_refresh(); 
    osDelay(1);
  }
}

/* 获取UID 用作设置 mac地址 */
static void readStm32UniqueID(u8 *stm32_uniqueId)
{
  u8 *addr = (u8*)(0x1fff7a10);   //UID首地址
  
  for(u8 i = 0; i < STM32_UNIQUE_ID_SIZE; ++i)
  {
    u8 id= *addr;
    stm32_uniqueId[i] = id;
    ++addr;
  }
}



/* 底层函数定义 ------------------------------------------------------------- */
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
	
#ifdef USE_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3); /* 静态IP地址设置 */
  IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3); /* 子网掩码设置 */
  IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2,GW_ADDR3); /* 网关设置 */
  
#endif /* USE_DHCP */
  
  /* - netif_add(struct netif *netif, ip_addr_t *ipaddr,
  ip_addr_t *netmask, ip_addr_t *gw,
  void *state, err_t (* init)(struct netif *netif),
  err_t (* input)(struct pbuf *p, struct netif *netif))
  
  Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.
  
  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  
  /* ethernetif_init 包括了硬件初始化过程 */
  if(NULL == netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input))
  {
    //printf("netif_add failed\r\n");
    return; //网口添加失败
  }
  else
  {
    //printf("netif_add OK\r\n");

    /*  Registers the default network interface. */
    netif_set_default(&gnetif); //设置netif为默认网口

    if (netif_is_link_up(&gnetif))
    {
    /* When the netif is fully configured this function must be called.*/
      netif_set_up(&gnetif);    //打开netif网口
    }
    else
    {
      /* When the netif link is down this function must be called */
      netif_set_down(&gnetif);
    }
    
    /* Set the link callback function, this function is called on change of link status*/
    netif_set_link_callback(&gnetif, ethernetif_update_config);

    /* create a binary semaphore used for informing ethernetif of frame reception */
    osSemaphoreDef(Netif_SEM);
    Netif_LinkSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM) , 1 );


    link_arg.netif = &gnetif;
    link_arg.semaphore = Netif_LinkSemaphore;
    /* Create the Ethernet link handler thread */
    #if defined(__GNUC__)
    osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    #else
    osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
    #endif
    osThreadCreate (osThread(LinkThr), &link_arg);
  }
  
  
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /*
  if (GPIO_Pin == GPIO_PIN_8)
  {
    // Get the IT status register value
    if(BSP_IO_ITGetStatus(MII_INT_PIN))
    {
      osSemaphoreRelease(Netif_LinkSemaphore);
    }
  }*/
}


/**
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  HAL_StatusTypeDef ret = HAL_OK;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  /* 详见 stm32f407 Reference---7.3.2 */
  /* PLL_CLK 频率 = 8M*PLLN/PLLM/PLLP = 150MHz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;              
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;                        
  RCC_OscInitStruct.PLL.PLLN = 300;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK) while(1);
 
  /* PLL_CLK 进行3分频，50MHz 输出到 PA8(MCO1) */
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_3);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  if(ret!=HAL_OK) while(1);

  
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
