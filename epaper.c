/*
 * epaper.c
 *
 * Документирование интерфейса драйвера e-paper и графического слоя.
 * Ниже приведены описания всех функций, используемых в модуле.
 */

#include "main.h"

/** Задержка в миллисекундах. */
void epd_delay(uint16_t ms);
/** Поднять линию RESET дисплея. */
void epd_res_set(void);
/** Опустить линию RESET дисплея. */
void epd_res_reset(void);
/** Переключить DC в режим данных. */
void epd_dc_set(void);
/** Переключить DC в режим команд. */
void epd_dc_reset(void);
/** Деактивировать CS (высокий уровень). */
void epd_cs_set(void);
/** Активировать CS (низкий уровень). */
void epd_cs_reset(void);
/** Прочитать флаг BUSY контроллера дисплея. */
uint8_t epd_is_busy(void);

/** Попытаться запустить следующую передачу из SPI-очереди. */
static void spi_try_start_next(void);
/** Колбэк завершения SPI DMA-передачи, продвигает очередь. */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
/** Поставить байт команды в очередь передачи. */
void epd_write_reg(uint8_t reg);
/** Поставить байт данных в очередь передачи. */
void epd_write_data(uint8_t data);
/** Внутренняя версия отправки байта данных через очередь. */
void _epd_write_data(uint8_t data);

/** Дождаться готовности контроллера (BUSY=0) с таймаутом. */
uint8_t epd_wait_busy(void);
/** Аппаратный сброс дисплея. */
void epd_reset(void);
/** Полная инициализация дисплея. */
uint8_t epd_init(void);
/** Инициализация режима частичного обновления. */
uint8_t epd_init_partial(void);
/** Перевести дисплей в deep sleep режим. */
void epd_enter_deepsleepmode(uint8_t mode);
/** Включить питание/обновление панели. */
uint8_t epd_power_on(void);
/** Выключить питание панели. */
uint8_t epd_power_off(void);
/** Инициализировать внутренний датчик температуры контроллера. */
void epd_init_internalTempSensor(void);
/** Запустить полное обновление экрана. */
void epd_update(void);
/** Запустить частичное обновление экрана. */
void epd_update_partial(void);

/** Задать адресное окно RAM дисплея. */
void epd_address_set(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
/** Установить текущую позицию указателя RAM дисплея. */
void epd_setpos(uint16_t x, uint16_t y);
/** Отправить массив данных в RAM дисплея. */
void epd_writedata(uint8_t *Image1, uint32_t length);
/** Отправить повторяющийся байт в RAM дисплея. */
void epd_writedata2(uint8_t data, uint32_t length);
/** Вывести два буфера (ч/б и второй канал) и обновить экран. */
void epd_display(uint8_t *Image1, uint8_t *Image2);
/** Вывести один ч/б буфер и обновить экран. */
void epd_displayBW(uint8_t *Image);
/** Частично обновить экран по ч/б буферу. */
void epd_displayBW_partial(uint8_t *Image);
/** Вывести буфер красного канала и обновить экран. */
void epd_displayRED(uint8_t *Image);

/** Инициализировать структуру холста EPD_Paint. */
void epd_paint_newimage(uint8_t *image, uint16_t Width, uint16_t Height, uint16_t Rotate, uint16_t Color);
/** Установить один пиксель в буфере холста с учетом поворота. */
void epd_paint_setpixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color);
/** Очистить буфер холста заданным цветом. */
void epd_paint_clear(uint16_t color);
/** Выбрать активный буфер холста. */
void epd_paint_selectimage(uint8_t *image);
/** Нарисовать точку (обертка над setpixel). */
void epd_paint_drawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color);
/** Нарисовать линию с заданной толщиной. */
void epd_paint_drawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint16_t LineWidth);
/** Вспомогательная функция: толстая точка (квадрат кисти). */
void drawThickPoint(uint16_t X, uint16_t Y, uint16_t Color, uint8_t LineWidth);
/** Нарисовать прямоугольник (контур или заливка). */
void epd_paint_drawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t mode);
/** Нарисовать полуокружность/окружность (контур или заливка). */
void epd_paint_drawSemiCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius, uint16_t Color, uint8_t mode, uint8_t semicircleMode, uint16_t LineWidth);
/** Нарисовать полуэллипс/эллипс. */
void epd_paint_drawSemiEllipse(uint16_t X_Center, uint16_t Y_Center, uint16_t RadiusX, uint16_t RadiusY, uint16_t Color, uint8_t mode, uint8_t semicircleMode, uint16_t LineWidth);
/** Вспомогательная функция: вывод точек полуэллипса по режиму. */
void drawSemiEllipsePoints(uint16_t X_Center, uint16_t Y_Center, uint16_t x, int y, uint16_t Color, uint8_t semicircleMode, uint16_t LineWidth);

/** Вывести C-строку векторным шрифтом. */
void epd_paint_showString(uint16_t x, uint16_t y, uint8_t *chr, uint16_t size1, uint16_t color);
/** Быстрое целочисленное возведение m^n. */
static uint32_t _Pow(uint16_t m, uint16_t n);
/** Вывести десятичное число фиксированной длины. */
void epd_paint_showNum(uint16_t x, uint16_t y, uint32_t num, uint16_t len, uint16_t size1, uint16_t color);
/** Показать битовую картинку на холсте. */
void epd_paint_showPicture(uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey, const uint8_t BMP[], uint16_t Color);
