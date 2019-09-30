#include "led.h"

/* Function Delaration */
static void rowEnable(u8 row);
static void TC5020Shift16bits(u16 data);
static void delay(u32 t);

void draw_point(u8 x, u8 y);
void draw_line(u8 from_x, u8 from_y, u8 to_x, u8 to_y);
void draw_rect(u8 ori_x, u8 ori_y, u8 length, u8 width);
void fill_rect(u8 ori_x, u8 ori_y, u8 length, u8 width);

void clr_point(u8 x, u8 y);
void clr_line(u8 from_x, u8 from_y, u8 to_x, u8 to_y);
void clr_rect(u8 ori_x, u8 ori_y, u8 length, u8 width);

void led_put_char(u8 x, u8 y, u8 c);
void led_put_string(u8 x, u8 y, const u8* str, u8 align);
void led_put_chinese(u8 x, u8 y, int uni_val);
void led_put_int(u8 x, u8 y, int n, u8 align);
void led_put_float(u8 x, u8 y, float n, u8 align);
void led_put_symbol(u8 x, u8 y, const u8 symbol[]);

static void frame0_display(void);
static void frame1_display(void);
static void frame2_display(void);
static void frame3_display(void);


static char flag_new_led = 1; // �¾�LED����ʾ�����෴

/* -----------------------------------------
   General Function Definition
   -----------------------------------------*/
void led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  __HAL_RCC_GPIOD_CLK_ENABLE();

  
  HAL_GPIO_WritePin(LED_GPIO_PORT, AIN_GPIO_PIN | OEIN_GPIO_PIN | BIN_GPIO_PIN | SCLK_GPIO_PIN \
                    | LATCH_GPIO_PIN | SERIN_GPIO_PIN, GPIO_PIN_RESET);
  
  GPIO_InitStructure.Pin = (AIN_GPIO_PIN | BIN_GPIO_PIN | OEIN_GPIO_PIN | SCLK_GPIO_PIN \
                            | LATCH_GPIO_PIN | SERIN_GPIO_PIN);
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);  
  
  HAL_GPIO_WritePin(LED_GPIO_PORT, AIN_GPIO_PIN | OEIN_GPIO_PIN | BIN_GPIO_PIN | SCLK_GPIO_PIN \
                    | LATCH_GPIO_PIN | SERIN_GPIO_PIN, GPIO_PIN_RESET);
  
  OE_High();
  
  TC5020_Lock();
  
}

void led_test(void)
{
  /*------------------------------*/
  /* ������ʾ */
  /* AMAE Logo */
  led_put_img(0, 4, 96, 40, amae_img);
  osDelay(300);
  led_clear(); 
  /* "AMAE" */
  led_put_char(32+8*0, 16, 'A');
  led_put_char(32+8*1, 16, 'M');
  led_put_char(32+8*2, 16, 'A');
  led_put_char(32+8*3, 16, 'E');
  /* "���ڹ�������" */
  led_put_chinese(0+16*0, 0, 0x6df1);
  led_put_chinese(0+16*1, 0, 0x5733);
  led_put_chinese(0+16*2, 0, 0x56fd);
  led_put_chinese(0+16*3, 0, 0x6280);
  led_put_chinese(0+16*4, 0, 0x4eea);
  led_put_chinese(0+16*5, 0, 0x5668);
  osDelay(300);
  led_clear();
  /* "AMAE" */
  led_put_char(32+8*0, 0, 'A');
  led_put_char(32+8*1, 0, 'M');
  led_put_char(32+8*2, 0, 'A');
  led_put_char(32+8*3, 0, 'E');
  /* "���ڹ�������" */
  led_put_chinese(0+16*0, 16, 0x6df1);
  led_put_chinese(0+16*1, 16, 0x5733);
  led_put_chinese(0+16*2, 16, 0x56fd);
  led_put_chinese(0+16*3, 16, 0x6280);
  led_put_chinese(0+16*4, 16, 0x4eea);
  led_put_chinese(0+16*5, 16, 0x5668);
  osDelay(300);
  led_clear();  
  /*------------------------------*/
  /* �ַ�����ʾ */  
  led_put_string(12, 0, "Hello world, I'm LED", 0);
  /*------------------------------*/
  /* ������ʾ */
  led_put_int(0, 0, 100, 0);
  led_put_float(0, 30, 123.456, 0);
  
  
}

void led_display(void)
{
  /* �л�3��frame */
  led_clear();
  
  frame0_display(); 
  osDelay(1000);
  led_clear();
  
  frame1_display();
  osDelay(1000);
  led_clear();
  
  frame2_display();
  osDelay(1000);
  led_clear();
  
  frame3_display();
  osDelay(1000);
  led_clear();

}

static void frame0_display(void) // СʱŨ�ȡ�ʵʱŨ��
{
  led_put_string(28, 32, "TSP", 1);
  led_put_chinese(36, 32, 0x6d53);//Ũ
  led_put_chinese(52, 32, 0x5ea6);//��
  led_put_chinese(68, 32, 0x503c);//ֵ
  
  led_put_chinese(0,  16, 0x5c0f);//С
  led_put_chinese(16, 16, 0x65f6);//ʱ
//  led_put_chinese(32, 16, 0x503c);//ֵ
  
  led_put_chinese(0,   0, 0x5206);//��
  led_put_chinese(16,  0, 0x949f);//��
//  led_put_chinese(32,  0, 0x503c);//ֵ
  
  led_put_char(32, 16, ':');  
  led_put_char(32,  0, ':');
  
  if (flag_display == 1)
  {
    if (flag_ConcHr == 1)
      led_put_float(72, 16, ConcHr, 1);
    else
      led_put_string(64, 16, "---", 1);
    
    if (flag_ConcRt == 1)
      led_put_float(72,  0, ConcRt, 1);
    else
      led_put_string(64,  0, "---", 1);
  }
  
  else
  {
    led_put_string(64, 16, "---", 1);
    led_put_string(64,  0, "---", 1);
  }
  
  led_put_string(88, 16, "ug", 1);
  led_put_string(88,  0, "ug", 1);

}

static void frame1_display(void) // �����¶ȡ�����ʪ�ȡ�����ѹ
{
  led_put_chinese( 0, 32, 0x6e29);//��
  led_put_chinese(16, 32, 0x5ea6);//��
  led_put_chinese( 0, 16, 0x6e7f);//ʪ
  led_put_chinese(16, 16, 0x5ea6);//��
  led_put_chinese( 0,  0, 0x6c14);//��
  led_put_chinese(16,  0, 0x538b);//ѹ

  led_put_char(32,  0, ':');
  led_put_char(32, 16, ':');
  led_put_char(32, 32, ':');
  
  if (flag_display == 1)
  {
    if (flag_AT == 1)
      led_put_float(72, 32, AT, 1);
    else
      led_put_string(64, 32, "---", 1);
    
    if (flag_RH == 1)
      led_put_float(72, 16, RH, 1);
    else
      led_put_string(64, 16, "---", 1);

    if (flag_BP == 1)
      led_put_float(72,  0, BP, 1);
    else
      led_put_string(64,  0, "---", 1);
  }
  else
  {
    led_put_string(64, 32, "---", 1);
    led_put_string(64, 16, "---", 1);
    led_put_string(64,  0, "---", 1);
  }

  led_put_symbol(80, 32, symbol_celsius);
  led_put_symbol(80, 16, symbol_percent);
  led_put_string(80,  0, "kP", 0);
  
}

static void frame2_display(void) // ���ȡ�γ��
{
  led_put_chinese( 0, 32, 0x7ecf);//��
  led_put_chinese(16, 32, 0x5ea6);//��
  led_put_chinese( 0, 16, 0x7eac);//γ
  led_put_chinese(16, 16, 0x5ea6);//��
  
  led_put_char(32, 32, ':');
  led_put_char(32, 16, ':');
  
  if (flag_display == 1)
  {
    if (flag_Longitude == 1)
      led_put_float(72, 32, Longitude, 1);
    else
      led_put_string(64, 32, "---", 1);
    
    if (flag_Latitude == 1)
      led_put_float(72, 16, Latitude, 1);
    else
      led_put_string(64, 16, "---", 1);
  }
  else
  {
    led_put_string(64, 32, "---", 1);
    led_put_string(64, 16, "---", 1);
  }
  
  
  led_put_symbol(80, 32, symbol_degree);
  led_put_symbol(80, 16, symbol_degree);
  
}

static void frame3_display(void) // ���١���������
{
  led_put_chinese( 0, 32, 0x98ce);//��
  led_put_chinese(16, 32, 0x901f);//��
  led_put_chinese( 0, 16, 0x98ce);//��
  led_put_chinese(16, 16, 0x5411);//��
  led_put_chinese( 0,  0, 0x566a);//��
  led_put_chinese(16,  0, 0x58f0);//��

  led_put_char(32,  0, ':');
  led_put_char(32, 16, ':');
  led_put_char(32, 32, ':');
  
  if (flag_display == 1)
  {
    if (flag_WindSpeed == 1)
      led_put_float(64, 32, WindSpeed, 1);
    else
      led_put_string(64, 32, "---", 1);
    
    if (flag_WindDir == 1)
      led_put_float(72, 16, WindDir, 1);
    else
      led_put_string(64, 16, "---", 1);
    
    if (flag_Noise == 1)
      led_put_float(72,  0, Noise, 1);    
    else
      led_put_string(64,  0, "---", 1);
  }
  else
  {
    led_put_string(64, 32, "---", 1);
    led_put_string(64, 16, "---", 1);
    led_put_string(64,  0, "---", 1);
  }
  
  led_put_string(72, 32, "m/s", 0);
  led_put_symbol(80, 16, symbol_degree);
  led_put_string(80,  0, "dB", 0);

  
}


void led_clear(void)
{
  u8 i, j;
  for (i = 0; i < ROW; i++)
    for (j = 0; j < COL; j++)
      clr_point(j, i);

}

void led_full(void)
{
  u8 i, j;
  for (i = 0; i < ROW; i++)
    for (j = 0; j < COL; j++)
      draw_point(j, i);

}

void led_refresh(void)
{
  int i;
  int intra_index;
  int panel_index;
  int intra_col_index;
  int panel_col_index;
  
  uint16_t data;
  
  while(1)
  {
    for(i = 0; i < 4 ; i++)//��ʹ��ˢ��
    {
      /* Big_Led contains 3*3 panels */
      for(panel_col_index = 2; panel_col_index >= 0; panel_col_index --)//colum = 3
      {
        for(panel_index = 2; panel_index >= 0; panel_index --)// row = 3
        {
          for(intra_col_index = 3; intra_col_index >= 0; intra_col_index --)//each panel contains 4 units (��ʾ4���ַ�/2������)
          {
            for(intra_index = 1; intra_index >= 0; intra_index --)//each unit contains 2 parts (16λ��λ����������4��)
            {
              /* ##################################### ��λ�������������(frame_buf)�Ĺ�ϵ ########################################### */
              data = 0;
              data += frame_buf[panel_index * 2 * 8 + (1 - intra_index) * 8 + (3 - i) + 4][panel_col_index * 4 + intra_col_index] << 0;
              data += frame_buf[panel_index * 2 * 8 + (1 - intra_index) * 8 + (3 - i) + 0][panel_col_index * 4 + intra_col_index] << 8;

              TC5020Shift16bits(data);
            }
          }
        }
      }
      
      
      /* ͨ���ӳ�����̬����A������ʱ�����л����ź�ʹ���ź� */
      rowEnable(i + 1);
   
      TC5020_Unlock();
      delay(1);
      TC5020_Lock();
      
      vTaskDelay(1);
    }    
  }
}

          
void draw_point(u8 x, u8 y)
{
  int byte_col_index;
  int bit_col_index;

  byte_col_index = x / 8;
  
  if (flag_new_led) 
  {
    bit_col_index = 7 - x % 8;
    frame_buf[47-y][11-byte_col_index] |= (0x01 << bit_col_index);
  }
  else 
  {
    /* λ���ֽڵĹ�ϵ */
    bit_col_index = x % 8;
    frame_buf[y][byte_col_index] |= (0x01 << bit_col_index);
  }
  
}

void clr_point(u8 x, u8 y)
{

  int byte_col_index;
  int bit_col_index;

  byte_col_index = x / 8;  
  
  if (flag_new_led)
  {
    bit_col_index = 7 - x % 8;
    frame_buf[47-y][11 - byte_col_index] &= ~(0x01 << bit_col_index);
  }
  else
  {
    bit_col_index = x % 8;
    frame_buf[y][byte_col_index] &= ~(0x01 << bit_col_index);
  }
  
}

void draw_line(u8 from_x, u8 from_y, u8 to_x, u8 to_y)
{
  u8 i;
  if (from_x == to_x || from_y == to_y)
  {
    //xy�غ�
    if (from_x == to_x && from_y == to_y) 
      draw_point(from_x, from_y);
    //x�غ�
    else if (from_x == to_x) 
    {
      if (to_y > from_y)
        for(i = from_y; i <= to_y; i++)
        {
          draw_point(from_x, i);
        }
      else
        for(i = from_y; i >= to_y; i--)
        {
          draw_point(from_x, i);
        }
    }
    //y�غ�
    else 
    {
      if (to_x > from_x)
        for(i = from_x; i <= to_x; i++)
        {
          draw_point(i, from_y);
        }
      else
        for(i = from_y; i >= to_y; i--)
        {
          draw_point(i, from_y);
        }
    }      
  }
  else //xy����ͬ
  {
    ;
  }
}
 
void clr_line(u8 from_x, u8 from_y, u8 to_x, u8 to_y)
{
    u8 i;
  if (from_x == to_x || from_y == to_y)
  {
    //xy�غ�
    if (from_x == to_x && from_y == to_y) 
      clr_point(from_x, from_y);
    //x�غ�
    else if (from_x == to_x) 
    {
      if (to_y > from_y)
        for(i = from_y; i <= to_y; i++)
        {
          clr_point(from_x, i);
        }
      else
        for(i = from_y; i >= to_y; i--)
        {
          clr_point(from_x, i);
        }
    }
    //y�غ�
    else 
    {
      if (to_x > from_x)
        for(i = from_x; i <= to_x; i++)
        {
          clr_point(i, from_y);
        }
      else
        for(i = from_y; i >= to_y; i--)
        {
          clr_point(i, from_y);
        }
    }      
  }
  else //xy����ͬ
  {
    ;
  }
}

void draw_rect(u8 ori_x, u8 ori_y, u8 length, u8 width)
{
  u8 i;
  if (ori_x + length > 95)
    length = 95 - ori_x;
  
  if (ori_y + width > 47)
    width = 47 - ori_y;
  
  for (i = 0; i < length; i++)
  {
    draw_line(ori_x, ori_y, ori_x+length-1, ori_y);
    draw_line(ori_x, ori_y+width-1, ori_x+length-1, ori_y+width-1);
  }
  for (i = 0; i < width; i++)
  {
    draw_line(ori_x, ori_y, ori_x, ori_y+width-1);
    draw_line(ori_x+length-1, ori_y, ori_x+length-1, ori_y+width-1);
  }

}

void clr_rect(u8 ori_x, u8 ori_y, u8 length, u8 width)
{
  u8 i;
  if (ori_x + length > 95)
    length = 95 - ori_x;
  
  if (ori_y + width > 47)
    width = 47 - ori_y;
  
  for (i = 0; i < length; i++)
  {
    clr_line(ori_x, ori_y, ori_x+length-1, ori_y);
    clr_line(ori_x, ori_y+width-1, ori_x+length-1, ori_y+width-1);
  }
  for (i = 0; i < width; i++)
  {
    clr_line(ori_x, ori_y, ori_x, ori_y+width-1);
    clr_line(ori_x+length-1, ori_y, ori_x+length-1, ori_y+width-1);
  }
}

void fill_rect(u8 ori_x, u8 ori_y, u8 length, u8 width)
{
  int i, j;
  
  if (ori_x + length > 95)
    length = 95 - ori_x;
  
  if (ori_y + width > 47)
    width = 47 - ori_y;  
  
  for (i = 0; i < length; i++)
  {
    for (j = 0; j < width; j++)
      draw_line(ori_x, ori_y+j, ori_x+length-1, ori_y+j);
  }
}


/* �趨ASCII�ַ����� 8*16 (16 bytes) */
void led_put_char(u8 x, u8 y, u8 c)
{
  u8 i, j;
  u8 temp;//�ݴ��ַ�ֲ��������λ
  // ��������ַ���λ�ú���ЧASCII��Χ
  if (x < 89 && y < 33 && (c >= 32 && c <= 126))
  {
    for (i = 0; i < 16; i++)//�趨1���ַ�16�ֽ�
    {
      temp = led_ascii[c-32][i];
      for (j = 0; j < 8; j++)
      {
        if (temp & 0x01)
          draw_point(x+j, y+i);
        else
          clr_point(x+j, y+i);
        temp >>= 1;
      }
    }
  }
}


/* ��ʾ˫�ֽ��ַ� 16*16 */
void led_put_symbol(u8 x, u8 y, const u8 symbol[])
{
  u8 i, j;
  u8 flag;
  u8 temp;
  
  for (i = 0; i < 32; i++)// 32 bytes
  {
    temp = symbol[i];
    for (j = 0; j < 8; j++)
    {
      if (i%2)//������ż�ַ����ַ��ֿ� 2 bytes
        flag = 1;
      else
        flag = 0;
      
      if (temp & 0x01)
        draw_point(x+j+flag*8, y+i/2);
      else
        clr_point(x+j+flag*8, y+i/2);
      
      temp >>= 1;             
    }
  }
}

/* �����ַ����� 16*16 */
void led_put_chinese(u8 x, u8 y, int uni_val)
{
  u8 i, j;
  u8 index;
  u8 flag;
  u8 temp;
  
  if (x < 81 && y < 33)
  {
    
    for (index = 0; index < 150; index++)   //����ģ������Ѱ����Ӧunicode�����ģ����index
    {
      if (uni_val == led_chinese[index].unicode) //���Ҫ�ҵ�unicode����ģ�����ڵ�unicodeƥ�䣬���жϴ˴�ѭ��
        break;
    }
    if (index == 150)           //û���ҵ���Ӧ���֣��˳�����
      return; 
    
    for (i = 0; i < 32; i++)// 32 bytes
    {
      temp = led_chinese[index].data[i];
      for (j = 0; j < 8; j++)
      {
        if (i%2)//������ż�ַ��������ַ��ֿ� 2 bytes
          flag = 1;
        else
          flag = 0;
          
        if (temp & 0x01)
          draw_point(x+j+flag*8, y+i/2);
        else
          clr_point(x+j+flag*8, y+i/2);
        
        temp >>= 1;             
      }
    }
    
  }
}


void led_put_string(u8 x, u8 y, const u8* str, u8 align)
{
  u8 i;
  int str_len;
  u8 offset = 0;
  
  str_len = strlen(str);
  
  if (!align) //�����
  {
    for (i = 0; i < str_len; i++)
    {
      led_put_char(x+offset, y, *(str+i));
      offset += 8;
    }
  }
  
  else//�Ҷ���
  {
    for (i = 0; i < str_len; i++)
    {
      led_put_char(x-offset, y, *(str+str_len-1-i));
      offset += 8;
    }
  }

}


void led_put_img(u8 x, u8 y, u8 width, u8 length, const u8 img[])
{
  u8 i, j, k;
  
  /* ע��t�����ͣ�����ȡ unsigned char������Ԫ�ظ������� 255 �����*/
  int t;
  
  u8 temp;
  u8 byte_col;
  
  if (x < 96 && y < 48 //ͼ��ԭ�㷶Χ
      && ((x+width) < 97) && ((y+length) < 49))//ͼ���С��Χ
  {
    byte_col = width/8;
    if (width % 8)
      byte_col++;//һ�������ֽ���
    
    t = 0;//ͼ��������λ��ʶ
    
    for (i = 0; i < length; i++)//������
    {
      for (j = 0; j < byte_col; j++) //�����ص��ֽ�
      {
        temp = *(img+(t++));
 
        for (k = 0; k < 8; k++)
        {
          if (temp & 0x01)
            draw_point(x+j*8+k, y+i);
          else
            clr_point(x+j*8+k, y+i);
          temp >>= 1;
        }   
      } 
    }
  }

}


void led_put_int(u8 x, u8 y, int num, u8 align)
{
  u8* str;
  u8 temp[32] = {0};
  str = temp;
  
  sprintf(str, "%d", num);
  
  led_put_string(x, y, str, align);
  
}

void led_put_float(u8 x, u8 y, float num, u8 align)
{
  u8* str;
  u8 temp[32] = {0};
  
  str = temp;
  
  sprintf(str, "%.1f", num); //����1λС��

  led_put_string(x, y, str, align);

}







/* -----------------------------------------
//Priate Function Definition
   ----------------------------------------- */
static void TC5020Shift16bits(u16 data)
{
  int i;
  for (i = 0; i < 16; i++)
  {
    if (data & 0x8000)
      TC5020_Data_High();
    else
      TC5020_Data_Low();    
    
    TC5020_SCLK_Low();
    delay(1);
    TC5020_SCLK_High();
    
    data <<= 1;
  }

}

static void rowEnable(u8 row)
{
  /* BIN��λ��AIN��λ */
  switch(row)
  {

  case 1:
    {
      BIN_Low();
      AIN_Low();
    }
    break;
  case 2:
    {
      BIN_Low();
      AIN_High();
    }
    break;
  case 3:
    {
      BIN_High();
      AIN_Low();
    }
    break;
  case 4:
    {
      BIN_High();
      AIN_High();
    }
    break;
    
  default:
    break;
  }
}

static void delay(u32 t)
{
  for (; t != 0; t--);
}

