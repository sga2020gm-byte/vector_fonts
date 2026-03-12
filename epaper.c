/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/


#include "main.h"

extern SPI_HandleTypeDef hspi1;

typedef enum
{
    EPD_SPI_IDLE = 0,
    EPD_SPI_CMD,
    EPD_SPI_DATA
} epd_spi_mode_t;

//--------------очередь---------------------------
static volatile epd_spi_mode_t epd_spi_mode = EPD_SPI_IDLE;

EPD_PAINT EPD_Paint;

#define SPI_QUEUE_SIZE 32

//--------------очередь---------------------------





typedef enum
{
    SPI_TX_CMD = 0,
    SPI_TX_DATA
} spi_tx_type_t;

typedef struct
{
    spi_tx_type_t type;
    uint8_t byte;
} spi_tx_item_t;

static spi_tx_item_t spi_queue[SPI_QUEUE_SIZE];

static volatile uint8_t q_head = 0;
static volatile uint8_t q_tail = 0;
static volatile uint8_t q_count = 0;

static volatile uint8_t spi_dma_busy = 0;








static uint8_t _hibernating = 1;

static const unsigned char lut_partial[] =
{
  0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x40, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2,
  0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0,
};

void epd_delay(uint16_t ms)
{
  HAL_Delay(ms);
}

void epd_res_set()
{
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
}

void epd_res_reset()
{
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
}

void epd_dc_set()
{
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
}

void epd_dc_reset()
{
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
}

void epd_cs_set()
{
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}

void epd_cs_reset()
{
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
}

uint8_t epd_is_busy()
{
	HAL_GPIO_ReadPin(BUSY_GPIO_Port, BUSY_Pin) == RESET ? 0 : 1;
}

//------попытка отправки данных/команды из очереди-----
static void spi_try_start_next(void)
{
    if (spi_dma_busy || q_count == 0)
        return;

    spi_tx_item_t *item = &spi_queue[q_tail];

    spi_dma_busy = 1;

    if (item->type == SPI_TX_CMD)
    	epd_dc_reset();
    else
    	epd_dc_set();

    epd_cs_reset();

    HAL_SPI_Transmit_DMA(&hspi1, &item->byte, 1);
}



//------колбэк от SPI, убирает элемент из очереди, вызывает попытку отправки-----
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance != SPI1)
        return;

    //epd_cs_set();

    spi_dma_busy = 0;

    q_tail = (q_tail + 1) % SPI_QUEUE_SIZE;
    q_count--;

    spi_try_start_next();
}

//---------отправка команды из очереди--------
void epd_write_reg(uint8_t reg)
{

    if (q_count >= SPI_QUEUE_SIZE)
        return 0;
    spi_queue[q_head].type = SPI_TX_CMD;
    spi_queue[q_head].byte = reg;

    q_head = (q_head + 1) % SPI_QUEUE_SIZE;
    q_count++;

    spi_try_start_next();

    return 1;
}


//-----------отправка данных из очереди-----------------

void epd_write_data(uint8_t data)
{
    if (q_count >= SPI_QUEUE_SIZE)
        return 0;
    spi_queue[q_head].type = SPI_TX_DATA;
    spi_queue[q_head].byte = data;

    q_head = (q_head + 1) % SPI_QUEUE_SIZE;
    q_count++;

    spi_try_start_next();

    return 1;
}

void _epd_write_data(uint8_t data)
{
    if (q_count >= SPI_QUEUE_SIZE)
        return 0;
    spi_queue[q_head].type = SPI_TX_DATA;
    spi_queue[q_head].byte = data;

    q_head = (q_head + 1) % SPI_QUEUE_SIZE;
    q_count++;

    spi_try_start_next();

    return 1;
}

/*void _epd_write_data_over()
{
  //while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
}*/

uint8_t epd_wait_busy()
{
  uint32_t timeout = 0;
  while (epd_is_busy())
  {
    timeout++;
    if (timeout > 40000)
    {
      return 1;
    }
    epd_delay(1);
  }
  return 0;
}

void epd_reset(void)
{
  epd_res_reset();
  epd_delay(50);
  epd_res_set();
  epd_delay(50);
  _hibernating = 0;
}

uint8_t epd_init(void)
{
  if (_hibernating)
    epd_reset();

  if (epd_wait_busy())
    return 1;

  epd_write_reg(0x12); // SWRESET

	epd_delay(10);

  if (epd_wait_busy())
    return 1;

	#if defined(EPD_29) || defined(EPD_213) || defined(EPD_154) 
  epd_write_reg(0x01); // Driver output control
#if defined(EPD_29) || defined(EPD_213)
  epd_write_data(0x27);
  epd_write_data(0x01);
  epd_write_data(0x01);
#else
  epd_write_data(0xC7);
  epd_write_data(0x00);
  epd_write_data(0x01);
	#endif

  epd_write_reg(0x11); // data entry mode
  epd_write_data(0x01);

#ifdef EPD_154
	epd_write_reg(0x44); // set Ram-X address start/end position
  epd_write_data(0x00);
  epd_write_data(0x18);

  epd_write_reg(0x45);  // set Ram-Y address start/end position
  epd_write_data(0xC7);
  epd_write_data(0x00);
  epd_write_data(0x00);
  epd_write_data(0x00);
	#else
  epd_write_reg(0x44); // set Ram-X address start/end position
  epd_write_data(0x00);
  epd_write_data(0x0F); // 0x0F-->(15+1)*8=128

  epd_write_reg(0x45);  // set Ram-Y address start/end position
  epd_write_data(0x27); // 0x127-->(295+1)=296
  epd_write_data(0x01);
  epd_write_data(0x00);
  epd_write_data(0x00);
#endif

  epd_write_reg(0x3C); // BorderWavefrom
  epd_write_data(0x05);

#if defined(EPD_29) || defined(EPD_213)
  epd_write_reg(0x21); //  Display update control
  epd_write_data(0x00);
  epd_write_data(0x80);
		#endif

	#elif defined(EPD_42)
		epd_write_reg(0x21); // Display Update Controll
		epd_write_data(0x40);
		epd_write_data(0x00);
		epd_write_reg(0x01);  // Set MUX as 300
		epd_write_data(0x2B);
		epd_write_data(0x01);
		epd_write_data(0x00);
		epd_write_reg(0x3C); //BorderWavefrom
		epd_write_data(0x01);	
		epd_write_reg(0x11);	// data  entry  mode
		epd_write_data(0x03);		// X-mode
		epd_address_set(0,0,EPD_W-1,EPD_H-1);
#endif

  epd_write_reg(0x18); // Read built-in temperature sensor
  epd_write_data(0x80);

  epd_setpos(0,0);

  if (epd_power_on())
    return 1;

  return 0;
}

uint8_t epd_init_partial(void)
{
  if (epd_init())
    return 1;

#if defined(EPD_29) || defined(EPD_213)
  epd_write_reg(0x32);
  //epd_cs_reset();
  for (int j = 0; j < sizeof(lut_partial); j++)
  {
    _epd_write_data(lut_partial[j]);
  }
  //_epd_write_data_over();
  //epd_cs_set();
	#elif defined(EPD_42)
	epd_write_reg(0x3C); //BorderWavefrom
	epd_write_data(0x80);	
	epd_write_reg(0x21); // Display Update Controll
  epd_write_data(0x00);    // RED normal
  epd_write_data(0x00);    // single chip application
#endif

  return 0;
}

void epd_enter_deepsleepmode(uint8_t mode)
{
epd_power_off();
  epd_write_reg(0x10);
  epd_write_data(mode);
  _hibernating = 1;
}

uint8_t epd_power_on(void)
{
	#if defined EPD_42
	epd_write_reg(0x22); // Display Update Control
	epd_write_data(0xe0);
	#else
	epd_write_reg(0x22); // Display Update Control
	epd_write_data(0xf8);
	#endif
  epd_write_reg(0x20); // Activate Display Update Sequence

  return epd_wait_busy();
}

uint8_t epd_power_off(void)
{
	epd_write_reg(0x22); // Display Update Control
	epd_write_data(0x83);
  epd_write_reg(0x20); // Activate Display Update Sequence

  return epd_wait_busy();
}

void epd_init_internalTempSensor(void)
{
  epd_write_reg(0x18);
  epd_write_data(0x80);

  epd_write_reg(0x1A);
  epd_write_data(0x7F);
//  epd_write_data(0xF0);
}

void epd_update(void)
{

	#ifdef EPD_154
  epd_write_reg(0x22); // Display Update Control
	epd_write_data(0xF4);
	#elif defined EPD_42
	epd_write_reg(0x22); // Display Update Control
  epd_write_data(0xF7);
	#else
	epd_write_reg(0x22); // Display Update Control
  epd_write_data(0xF7);
#endif
  epd_write_reg(0x20); // Activate Display Update Sequence

  //epd_wait_busy();
}

void epd_update_partial(void)
{
  #ifdef EPD_154
  epd_write_reg(0x22); // Display Update Control
	epd_write_data(0xFC);
	#elif defined EPD_42
	epd_write_reg(0x22); // Display Update Control
	epd_write_data(0xFF);
	#else
	epd_write_reg(0x22); // Display Update Control
  epd_write_data(0xCC);
#endif
  epd_write_reg(0x20); // Activate Display Update Sequence
  //epd_wait_busy();
}

void epd_address_set(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
	epd_write_reg(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
	epd_write_data((x_start>>3) & 0xFF);
	epd_write_data((x_end>>3) & 0xFF);

	epd_write_reg(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
	epd_write_data(y_start & 0xFF);
	epd_write_data((y_start >> 8) & 0xFF);
	epd_write_data(y_end & 0xFF);
	epd_write_data((y_end >> 8) & 0xFF);
}

void epd_setpos(uint16_t x, uint16_t y)
{
  uint8_t _x;
  uint16_t _y;

  _x = x / 8;

#ifdef EPD_154
	_y = 199 - y;
	#elif defined(EPD_29) || defined(EPD_213)
    _y = 295 - y;
	#elif defined(EPD_42)
	_y = y;
#endif

  epd_write_reg(0x4E); // set RAM x address count to 0;
  epd_write_data(_x);
  epd_write_reg(0x4F); // set RAM y address count to 0x127;
  epd_write_data(_y & 0xff);
  epd_write_data(_y >> 8 & 0x01);
}

void epd_writedata(uint8_t *Image1, uint32_t length)
{
	  for (uint32_t j = 0; j < length; j++)
	  {
	    _epd_write_data(Image1[j]);
	  }
}
void epd_writedata2(uint8_t data, uint32_t length)
{
  //epd_cs_reset();
  for (uint32_t j = 0; j < length; j++)
  {
    _epd_write_data(data);
  }
  //_epd_write_data_over();
  //epd_cs_set();
}

void epd_display(uint8_t *Image1, uint8_t *Image2)
{
  uint32_t Width, Height, i, j;
  uint32_t k = 0;
  Width = EPD_H;
  Height = EPD_W_BUFF_SIZE;

  epd_setpos(0, 0);

  epd_write_reg(0x24);
  epd_writedata(Image1, Width * Height);

  epd_setpos(0, 0);

  epd_write_reg(0x26);
  k = 0;
  //epd_cs_reset();
  for (j = 0; j < Height; j++)
  {
    for (i = 0; i < Width; i++)
    {
      _epd_write_data(~Image2[k]);
      k++;
    }
  }
  //_epd_write_data_over();
  //epd_cs_set();

	#if defined EPD_42
	epd_write_reg(0x21); // Display Update Controll
  epd_write_data(0x00);    // RED normal
  epd_write_data(0x00);    // single chip application
	#endif

  epd_update();
}

void epd_displayBW(uint8_t *Image)
{
  uint32_t Width, Height;
  
  Width = EPD_H;
  Height = EPD_W_BUFF_SIZE;

  /*epd_setpos(0, 0);
  epd_write_reg(0x26);
  epd_writedata(Image, Width * Height);*/

  epd_setpos(0, 0);
  epd_write_reg(0x24);
  epd_writedata(Image, Width * Height);

  epd_update();
}

void epd_displayBW_partial(uint8_t *Image)
{
	if (!epd_is_busy())
	{
  uint32_t Width, Height;

  Width = EPD_H;
  Height = EPD_W_BUFF_SIZE;

  epd_setpos(0, 0);
  epd_write_reg(0x24);
  epd_writedata(Image, Width * Height);

  epd_update_partial();

  //epd_setpos(0, 0);
  //epd_write_reg(0x26);
  //epd_writedata(Image, Width * Height);
	}
    }

void epd_displayRED(uint8_t *Image)
{
  uint32_t Width, Height;

  Width = EPD_H;
  Height = EPD_W_BUFF_SIZE;

  epd_setpos(0, 0);

  epd_write_reg(0x26);
  epd_writedata(Image, Width * Height);

	#if defined EPD_42
	epd_write_reg(0x21); // Display Update Controll
  epd_write_data(0x00);    // RED normal
  epd_write_data(0x00);    // single chip application
	#endif
  epd_update();
}

void epd_paint_newimage(uint8_t *image, uint16_t Width, uint16_t Height, uint16_t Rotate, uint16_t Color)
{
  EPD_Paint.Image = 0x00;
  EPD_Paint.Image = image;

  EPD_Paint.WidthMemory = Width;
  EPD_Paint.HeightMemory = Height;
  EPD_Paint.Color = Color;
  EPD_Paint.WidthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
  EPD_Paint.HeightByte = Height;
  EPD_Paint.Rotate = Rotate;
  if (Rotate == EPD_ROTATE_0 || Rotate == EPD_ROTATE_180)
  {

    EPD_Paint.Width = Height;
    EPD_Paint.Height = Width;
  }
  else
  {
    EPD_Paint.Width = Width;
    EPD_Paint.Height = Height;
  }
}

void epd_paint_setpixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color)
{
  uint16_t X, Y;
  uint32_t Addr;
  uint8_t Rdata;
  switch (EPD_Paint.Rotate)
  {
  case 0:

    X = EPD_Paint.WidthMemory - Ypoint - 1;
    Y = Xpoint;
    break;
  case 90:
    X = EPD_Paint.WidthMemory - Xpoint - 1;
    Y = EPD_Paint.HeightMemory - Ypoint - 1;
    break;
  case 180:
    X = Ypoint;
    Y = EPD_Paint.HeightMemory - Xpoint - 1;
    break;
  case 270:
    X = Xpoint;
    Y = Ypoint;
    break;
  default:
    return;
  }
  Addr = X / 8 + Y * EPD_Paint.WidthByte;
  Rdata = EPD_Paint.Image[Addr];
  if (Color == EPD_COLOR_BLACK)
  {
    EPD_Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
  }
  else
    EPD_Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
}

void epd_paint_clear(uint16_t color)
{
  uint16_t X, Y;
  uint32_t Addr;

  for (Y = 0; Y < EPD_Paint.HeightByte; Y++)
  {
    for (X = 0; X < EPD_Paint.WidthByte; X++)
    { // 8 pixel =  1 byte
      Addr = X + Y * EPD_Paint.WidthByte;
      EPD_Paint.Image[Addr] = color;
    }
  }
}

void epd_paint_selectimage(uint8_t *image)
{
  EPD_Paint.Image = image;
}

void epd_paint_drawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color)
{
  epd_paint_setpixel(Xpoint - 1, Ypoint - 1, Color);
}


void epd_paint_drawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint16_t LineWidth)
{
    int32_t dx = abs((int32_t)Xend - (int32_t)Xstart);
    int32_t dy = abs((int32_t)Yend - (int32_t)Ystart);
    int32_t sx = (Xstart < Xend) ? 1 : -1;
    int32_t sy = (Ystart < Yend) ? 1 : -1;
    int32_t err = dx - dy;
    int32_t e2;

    while (1)
    {
        drawThickPoint(Xstart, Ystart, Color, LineWidth);

        if (Xstart == Xend && Ystart == Yend)
            break;

        e2 = err * 2;
        if (e2 > -dy)
        {
            err -= dy;
            Xstart += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            Ystart += sy;
        }
    }
}

// Функция рисования точки с учетом ширины линии
void drawThickPoint(uint16_t X, uint16_t Y, uint16_t Color, uint8_t LineWidth)
{
    int8_t offset = LineWidth / 2;
    for (int8_t i = -offset; i <= offset; i++)
    {
        for (int8_t j = -offset; j <= offset; j++)
        {
            epd_paint_drawPoint(X + i, Y + j, Color);
        }
    }
}


void epd_paint_drawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t mode)
{
  uint16_t i;
  if (mode)
  {
    for (i = Ystart; i < Yend; i++)
    {
      epd_paint_drawLine(Xstart, i, Xend, i, Color,1);
    }
  }
  else
  {
    epd_paint_drawLine(Xstart, Ystart, Xend, Ystart, Color,1);
    epd_paint_drawLine(Xstart, Ystart, Xstart, Yend, Color,1);
    epd_paint_drawLine(Xend, Yend, Xend, Ystart, Color,1);
    epd_paint_drawLine(Xend, Yend, Xstart, Yend, Color,1);
  }
}

void epd_paint_drawSemiCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius, uint16_t Color, uint8_t mode, uint8_t semicircleMode, uint16_t LineWidth) {
    int16_t XCurrent = 0, YCurrent = Radius;
    int16_t Esp = 3 - (Radius << 1); // Алгоритм Брезенхэма

    while (XCurrent <= YCurrent) {
        if (mode) {
            // Закрашенный полукруг: рисуем горизонтальными линиями
            for (int16_t i = -XCurrent; i <= XCurrent; i++) {
                if (semicircleMode == 0 || semicircleMode == 4) { // Верхний полукруг
                    drawThickPoint(X_Center + i, Y_Center - YCurrent, Color, LineWidth);
                    drawThickPoint(X_Center + i, Y_Center - XCurrent, Color, LineWidth);
                }
                if (semicircleMode == 1 || semicircleMode == 4) { // Нижний полукруг
                    drawThickPoint(X_Center + i, Y_Center + YCurrent, Color, LineWidth);
                    drawThickPoint(X_Center + i, Y_Center + XCurrent, Color, LineWidth);
                }
            }
            for (int16_t i = -YCurrent; i <= YCurrent; i++) {
                if (semicircleMode == 2 || semicircleMode == 4) { // Левый полукруг
                    drawThickPoint(X_Center - YCurrent, Y_Center + i, Color, LineWidth);
                    drawThickPoint(X_Center - XCurrent, Y_Center + i, Color, LineWidth);
                }
                if (semicircleMode == 3 || semicircleMode == 4) { // Правый полукруг
                    drawThickPoint(X_Center + YCurrent, Y_Center + i, Color, LineWidth);
                    drawThickPoint(X_Center + XCurrent, Y_Center + i, Color, LineWidth);
                }
            }
        } else {
            // Пустой полукруг: рисуем только контурные точки
            if (semicircleMode == 0 || semicircleMode == 4) { // Верхний полукруг
                drawThickPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, LineWidth);
                drawThickPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, LineWidth);
                drawThickPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, LineWidth);
                drawThickPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, LineWidth);
            }
            if (semicircleMode == 1 || semicircleMode == 4) { // Нижний полукруг
                drawThickPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, LineWidth);
                drawThickPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, LineWidth);
                drawThickPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, LineWidth);
                drawThickPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, LineWidth);
            }
            if (semicircleMode == 2 || semicircleMode == 4) { // Левый полукруг
                drawThickPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, LineWidth);
                drawThickPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, LineWidth);
                drawThickPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, LineWidth);
                drawThickPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, LineWidth);
            }
            if (semicircleMode == 3 || semicircleMode == 4) { // Правый полукруг
                drawThickPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, LineWidth);
                drawThickPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, LineWidth);
                drawThickPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, LineWidth);
                drawThickPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, LineWidth);
            }
        }

        // Обновление ошибки по алгоритму Брезенхэма
        if (Esp < 0) {
            Esp += 4 * XCurrent + 6;
        } else {
            Esp += 4 * (XCurrent - YCurrent) + 10;
            YCurrent--;
        }
        XCurrent++;
    }
}

void epd_paint_drawSemiEllipse(uint16_t X_Center, uint16_t Y_Center, uint16_t RadiusX, uint16_t RadiusY, uint16_t Color, uint8_t mode, uint8_t semicircleMode, uint16_t LineWidth) {
    int x = 0, y = RadiusY;
    long a2 = (long)RadiusX * RadiusX;
    long b2 = (long)RadiusY * RadiusY;
    long twoA2 = 2 * a2, twoB2 = 2 * b2;
    long p;
    long px = 0, py = twoA2 * y;

    // Часть 1: Отрисовка первой половины эллипса
    p = b2 - a2 * RadiusY + (a2 / 4);
    while (px < py) {
        drawSemiEllipsePoints(X_Center, Y_Center, x, y, Color, semicircleMode, LineWidth);
        x++;
        px += twoB2;
        if (p < 0) {
            p += b2 + px;
        } else {
            y--;
            py -= twoA2;
            p += b2 + px - py;
        }
    }

    // Часть 2: Вторая половина эллипса
    p = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        drawSemiEllipsePoints(X_Center, Y_Center, x, y, Color, semicircleMode, LineWidth);
        y--;
        py -= twoA2;
        if (p > 0) {
            p += a2 - py;
        } else {
            x++;
            px += twoB2;
            p += a2 - py + px;
        }
    }
}

// Функция для рисования точек с учетом режима полуэллипса
void drawSemiEllipsePoints(uint16_t X_Center, uint16_t Y_Center, uint16_t x, int y, uint16_t Color, uint8_t semicircleMode, uint16_t LineWidth) {
    switch (semicircleMode) {
        case 0:  // Верхняя половина
        	drawThickPoint(X_Center + x, Y_Center - y, Color, LineWidth);
        	drawThickPoint(X_Center - x, Y_Center - y, Color, LineWidth);
            break;
        case 1:  // Нижняя половина
        	drawThickPoint(X_Center + x, Y_Center + y, Color, LineWidth);
        	drawThickPoint(X_Center - x, Y_Center + y, Color, LineWidth);
            break;
        case 2:  // Левая половина
        	drawThickPoint(X_Center - x, Y_Center + y, Color, LineWidth);
        	drawThickPoint(X_Center - x, Y_Center - y, Color, LineWidth);
            break;
        case 3:  // Правая половина
        	drawThickPoint(X_Center + x, Y_Center + y, Color, LineWidth);
        	drawThickPoint(X_Center + x, Y_Center - y, Color, LineWidth);
            break;
        case 4:  // Полный эллипс
        	drawThickPoint(X_Center + x, Y_Center - y, Color, LineWidth);
        	drawThickPoint(X_Center - x, Y_Center - y, Color, LineWidth);
        	drawThickPoint(X_Center + x, Y_Center + y, Color, LineWidth);
        	drawThickPoint(X_Center - x, Y_Center + y, Color, LineWidth);
            break;
    }
}





/*void epd_paint_showChar(uint16_t x, uint16_t y, uint16_t chr, uint16_t size1, uint16_t color)
{
  uint16_t i, m, temp, size2, chr1;
  uint16_t x0, y0;
  x += 1, y += 1, x0 = x, y0 = y;
  if (x - size1 > EPD_H)
    return;
  if (size1 == 8)
    size2 = 6;
  else
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
  chr1 = chr - ' ';
  for (i = 0; i < size2; i++)
  {
    if (size1 == 8)
    {
      temp = asc2_0806[chr1][i];
    } // 0806
    else if (size1 == 12)
    {
      temp = asc2_1206[chr1][i];
    } // 1206
    else if (size1 == 16)
    {
      temp = asc2_1608[chr1][i];
    } // 1608
    else if (size1 == 24)
    {
      temp = asc2_2412[chr1][i];
    } // 2412
    else
      return;
    for (m = 0; m < 8; m++)
    {
      if (temp & 0x01)
        epd_paint_drawPoint(x, y, color);
      else
        epd_paint_drawPoint(x, y, !color);
      temp >>= 1;
      y++;
    }
    x++;
    if ((size1 != 8) && ((x - x0) == size1 / 2))
    {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}
*/
void epd_paint_showString(uint16_t x, uint16_t y, uint8_t *chr, uint16_t size1, uint16_t color)
{
  while (*chr != '\0')
  {
	epd_paint_showChar_vector(x, y, *chr, size1, color);
    chr++;
    if (*chr =='.'){
    	x = x + size1 / 10;
    }
    else
    {
    x = x + size1 * 0.75;
    }
    }
}

// m^n
static uint32_t _Pow(uint16_t m, uint16_t n)
{
  uint32_t result = 1;
  while (n--)
  {
    result *= m;
  }
  return result;
}

void epd_paint_showNum(uint16_t x, uint16_t y, uint32_t num, uint16_t len, uint16_t size1, uint16_t color)
{
  uint8_t t, temp, m = 0;
  if (size1 == 8)
    m = 2;
  for (t = 0; t < len; t++)
  {
    temp = (num / _Pow(10, len - t - 1)) % 10;
    if (temp == 0)
    {
      epd_paint_showChar(x + (size1 / 2 + m) * t, y, '0', size1, color);
    }
    else
    {
      epd_paint_showChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, color);
    }
  }
}

/*void epd_paint_showChinese(uint16_t x, uint16_t y, uint16_t num, uint16_t size1, uint16_t color)
{
  uint16_t m, temp;
  uint16_t x0, y0;
  uint16_t i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1;
  x += 1, y += 1, x0 = x, y0 = y;
  for (i = 0; i < size3; i++)
  {
    if (size1 == 16)
    {
      temp = Hzk1[num][i];
    } // 16*16
    else if (size1 == 24)
    {
      temp = Hzk2[num][i];
    } // 24*24
    else if (size1 == 32)
    {
      temp = Hzk3[num][i];
    } // 32*32
    else if (size1 == 64)
    {
      temp = Hzk4[num][i];
    } // 64*64
    else
      return;
    for (m = 0; m < 8; m++)
    {
      if (temp & 0x01)
        epd_paint_drawPoint(x, y, color);
      else
        epd_paint_drawPoint(x, y, !color);
      temp >>= 1;
      y++;
    }
    x++;
    if ((x - x0) == size1)
    {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}
*/
void epd_paint_showPicture(uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey, const uint8_t BMP[], uint16_t Color)
{
  uint16_t j = 0;
  uint16_t i, n = 0, temp = 0, m = 0;
  uint16_t x0 = 0, y0 = 0;
  x += 1, y += 1, x0 = x, y0 = y;
  sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
  for (n = 0; n < sizey; n++)
  {
    for (i = 0; i < sizex; i++)
    {
      temp = BMP[j];
      j++;
      for (m = 0; m < 8; m++)
      {
        if (temp & 0x01)
          epd_paint_drawPoint(x, y, !Color);
        else
          epd_paint_drawPoint(x, y, Color);
        temp >>= 1;
        y++;
      }
      x++;
      if ((x - x0) == sizex)
      {
        x = x0;
        y0 = y0 + 8;
      }
      y = y0;
    }
  }
}
