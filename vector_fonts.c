/*
 * vector_fonts.c
 *
 *  Created on: Feb 26, 2025
 *      Author: GryaznevAA
 */
#include "stdint.h"

#include "epaper.h"
#include "main.h"


void vector_A(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth){
	uint16_t width = size1 / 2;  // Ширина буквы A (половина высоты)
	uint16_t crossbarY = y + (2 * size1 / 3);  // Положение перекладины (2/3 от высоты)

	    // Рисуем левую наклонную линию
	epd_paint_drawLine(x, y + size1, x + width / 2, y, color,LineWidth);

	    // Рисуем правую наклонную линию
	epd_paint_drawLine(x + width / 2, y, x + width, y + size1, color,LineWidth);

	    // Рисуем перекладину (горизонтальную линию)
	epd_paint_drawLine(x + width / 4, crossbarY, x + (3 * width / 4), crossbarY, color,LineWidth);

}
void vector_B(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth){
	 // Определяем параметры буквы B
	    uint16_t width = size1 / 2;  // Ширина буквы B (половина высоты)
	    uint16_t halfHeight = size1 / 2;  // Половина высоты для разделения буквы B


	    // Рисуем вертикальную линию (основа буквы B)
	    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

	    // Рисуем верхний полукруг
	    epd_paint_drawSemiCircle(x, y + halfHeight / 2, halfHeight / 2, color, 0, 3,LineWidth);  // Нижний полукруг

	    // Рисуем нижний полукруг
	    //epd_paint_drawSemiCircle(x, y + halfHeight + halfHeight / 2,halfHeight/2, color, 0, 3,LineWidth);  // Верхний полукруг
	    epd_paint_drawSemiEllipse(x,y + halfHeight + halfHeight / 2, halfHeight*3/4 ,halfHeight/2 , color, 0, 3,LineWidth);
	}
void vector_C(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth){
	// Определяем параметры буквы C
	    uint16_t radius = size1 / 2;  // Радиус буквы C (половина высоты)

	    // Рисуем левый полукруг (буква C)
	    epd_paint_drawSemiEllipse(x + radius/2,y+radius, radius*3/4 ,size1/2 , color, 0, 2,LineWidth);
	    //epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 0, 2);  Левый полукруг (пустой)
	}
void vector_D(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth){
    // Определяем параметры буквы D
    uint16_t radius = size1 / 2;  // Радиус полукруга (половина высоты)

    // Рисуем вертикальную линию (основа буквы D)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем правый полукруг
    epd_paint_drawSemiCircle(x, y + radius, radius, color, 0, 3, LineWidth);  // Правый полукруг (пустой)
}

void vector_E(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы E
    uint16_t width = size1 / 2;  // Ширина буквы E (половина высоты)

    // Рисуем вертикальную линию (основа буквы E)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем верхнюю горизонтальную линию
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Рисуем среднюю горизонтальную линию
    epd_paint_drawLine(x, y + size1 / 2, x + width, y + size1 / 2, color, LineWidth);

    // Рисуем нижнюю горизонтальную линию
    epd_paint_drawLine(x, y + size1, x + width, y + size1, color, LineWidth);
}
void vector_F(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы E
    uint16_t width = size1 / 2;  // Ширина буквы E (половина высоты)

    // Рисуем вертикальную линию (основа буквы E)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем верхнюю горизонтальную линию
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Рисуем среднюю горизонтальную линию
    epd_paint_drawLine(x, y + size1 / 2, x + width*0.7, y + size1 / 2, color, LineWidth);
}
void vector_G(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
	// Определяем параметры буквы C
	    uint16_t radius = size1 / 2;  // Радиус буквы C (половина высоты)

	    // Рисуем левый полукруг (буква G)
	    epd_paint_drawSemiEllipse(x + (radius/3),y+radius, radius*2/3 ,size1/2 , color, 0, 2, LineWidth);
	    //epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 0, 2);
	    // Рисуем нижний полукруг (буква G)
	    epd_paint_drawSemiEllipse(x + (radius/3),y+radius, radius*2/3 ,size1/2 , color, 0, 1, LineWidth);
	    //epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 0, 1);
	    // Рисуем ЛИНИЮ (буква G)
	    epd_paint_drawLine(x + radius*2/6, y + radius, x + radius, y + size1 / 2, color, LineWidth);
}
void vector_H(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы H
    uint16_t width = size1 / 2;  // Ширина буквы H (половина высоты)

    // Рисуем левую вертикальную линию
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем правую вертикальную линию
    epd_paint_drawLine(x + width, y, x + width, y + size1, color, LineWidth);

    // Рисуем горизонтальную линию посередине
    epd_paint_drawLine(x, y + size1 / 2, x + width, y + size1 / 2, color, LineWidth);
}
void vector_I(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы I
    uint16_t width = size1 / 3;  // Ширина буквы I (четверть высоты)

    // Рисуем верхнюю горизонтальную линию
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Рисуем вертикальную линию (основа буквы I)
    epd_paint_drawLine(x + width / 2, y, x + width / 2, y + size1, color, LineWidth);

    // Рисуем нижнюю горизонтальную линию
    epd_paint_drawLine(x, y + size1, x + width, y + size1, color, LineWidth);
}
void vector_J(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы J
    uint16_t width = size1 / 2;  // Ширина буквы J (половина высоты)
    uint16_t radius = size1 / 4;  // Радиус закругления внизу

    // Рисуем вертикальную линию (основа буквы J)
    epd_paint_drawLine(x + width, y, x + width, y + size1 - radius, color, LineWidth);

    // Рисуем закругление внизу (полукруг)
    epd_paint_drawSemiCircle(x + width / 2, y + size1 - radius, radius, color, 0, 1, LineWidth);  // Нижний полукруг (пустой)
}
void vector_K(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы K
    uint16_t width = size1 / 2;  // Ширина буквы K (половина высоты)

    // Рисуем вертикальную линию (основа буквы K)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем верхнюю наклонную линию
    epd_paint_drawLine(x, y + size1 / 2, x + width, y, color, LineWidth);

    // Рисуем нижнюю наклонную линию
    epd_paint_drawLine(x, y + size1 / 2, x + width, y + size1, color, LineWidth);
}
void vector_L(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы L
    uint16_t width = size1 / 2;  // Ширина буквы L (половина высоты)

    // Рисуем вертикальную линию (основа буквы L)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем нижнюю горизонтальную линию
    epd_paint_drawLine(x, y + size1, x + width, y + size1, color, LineWidth);
}
void vector_M(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы M
    uint16_t width = size1 / 2;  // Ширина буквы M (половина высоты)

    // Рисуем левую вертикальную линию
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем правую вертикальную линию
    epd_paint_drawLine(x + width, y, x + width, y + size1, color, LineWidth);

    // Рисуем левую наклонную линию
    epd_paint_drawLine(x, y, x + width / 2, y + size1 / 2, color, LineWidth);

    // Рисуем правую наклонную линию
    epd_paint_drawLine(x + width / 2, y + size1 / 2, x + width, y, color, LineWidth);
}
void vector_N(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы N
    uint16_t width = size1 / 2;  // Ширина буквы N (половина высоты)

    // Рисуем левую вертикальную линию
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем правую вертикальную линию
    epd_paint_drawLine(x + width, y, x + width, y + size1, color, LineWidth);

    // Рисуем наклонную линию
    epd_paint_drawLine(x, y, x + width, y + size1, color, LineWidth);
}
void vector_O(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы O
	uint16_t size;
	size = size1/2;
    uint16_t radius = size / 2;  // Радиус буквы O (половина высоты)

    // Рисуем окружность (буква O)
    epd_paint_drawSemiEllipse(x + radius,y + size, radius ,size , color, 0, 4, LineWidth);// Пустой круг
}
void vector_P(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы P
    uint16_t width = size1 / 2;  // Ширина буквы P (половина высоты)
    uint16_t radius = size1 / 3;  // Радиус полукруга (четверть высоты)

    // Рисуем вертикальную линию (основа буквы P)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем верхний полукруг
    epd_paint_drawSemiCircle(x, y+size1/3, radius, color, 0, 3, LineWidth);  // Нижний полукруг (пустой)
}
void vector_Q(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы Q
    uint16_t radius = size1 / 2;  // Радиус буквы Q (половина высоты)

    // Рисуем окружность (основа буквы Q)
    epd_paint_drawSemiEllipse(x + radius/3,y + radius, radius/2 ,radius , color, 0, 4, LineWidth);
    //epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 4);  // Пустой круг

    // Рисуем хвостик (диагональная линия внизу справа)
    epd_paint_drawLine(x + radius*2/3, y + radius*5/3, x + radius, y+2*radius, color, LineWidth);
}
void vector_R(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы P
    uint16_t width = size1 / 2;  // Ширина буквы P (половина высоты)
    uint16_t radius = size1 / 3;  // Радиус полукруга (четверть высоты)

    // Рисуем вертикальную линию (основа буквы P)
    epd_paint_drawLine(x, y, x, y + size1, color, LineWidth);

    // Рисуем верхний полукруг
    epd_paint_drawSemiCircle(x, y+radius, radius, color, 0, 3, LineWidth);  // Нижний полукруг (пустой)

    // Рисуем наклонную линию (основа буквы P)
    epd_paint_drawLine(x, y+width, x+width, y + size1, color, LineWidth);
}
void vector_S(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы S
    uint16_t radius = size1 / 4;  // Радиус полукругов (четверть высоты)
    uint16_t midY = y + size1 / 2;  // Середина высоты

    // Рисуем НИЖНИЙ полукруг (вогнутый вниз)
    epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 0, 2, LineWidth);  // Нижний полукруг (пустой)

    // Рисуем ВЕРХНИЙ полукруг (вогнутый вверх)
    epd_paint_drawSemiCircle(x + radius/3, midY + radius, radius, color, 0, 3, LineWidth);  // Верхний полукруг (пустой)
}
void vector_T(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы T
    uint16_t width = size1 / 2;  // Ширина буквы T (половина высоты)

    // Рисуем горизонтальную линию (верхняя часть буквы T)
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Рисуем вертикальную линию (основа буквы T)
    epd_paint_drawLine(x + width / 2, y, x + width / 2, y + size1, color, LineWidth);
}
void vector_U(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы U
    uint16_t width = size1 / 2;  // Ширина буквы U (половина высоты)
    uint16_t radius = size1 / 4;  // Радиус полукруга (четверть высоты)

    // Рисуем левую вертикальную линию
    epd_paint_drawLine(x, y , x, y +size1-radius, color, LineWidth);

    // Рисуем правую вертикальную линию
    epd_paint_drawLine(x + width, y, x + width, y  +size1-radius, color, LineWidth);

    // Рисуем нижний полукруг
    epd_paint_drawSemiCircle(x + radius, y + size1-radius, radius, color, 0, 1, LineWidth);  // Нижний полукруг (пустой)
}
void vector_V(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы V
    uint16_t width = size1 / 2;  // Ширина буквы V (половина высоты)

    // Рисуем левую наклонную линию
    epd_paint_drawLine(x + width / 2, y+ size1, x, y , color, LineWidth);

    // Рисуем правую наклонную линию
    epd_paint_drawLine(x + width / 2, y+ size1, x + width, y , color, LineWidth);
}
void vector_W(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы W
    uint16_t width = size1 / 2;  // Ширина буквы W (половина высоты)

    // Рисуем левую наклонную линию (верхняя часть)
    epd_paint_drawLine(x, y, x + width / 4, y+size1, color, LineWidth);

    // Рисуем левую наклонную линию (нижняя часть)
    epd_paint_drawLine(x + width / 4, y+size1, x + width / 2, y + size1*3/4 / 2, color, LineWidth);

    // Рисуем правую наклонную линию (нижняя часть)
    epd_paint_drawLine(x + width / 2, y + size1 / 2, x + 3 * width / 4, y + size1, color, LineWidth);

    // Рисуем правую наклонную линию (верхняя часть)
    epd_paint_drawLine(x + 3 * width / 4, y + size1, x + width, y, color, LineWidth);
}
void vector_X(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы X
    uint16_t width = size1 / 2;  // Ширина буквы X (половина высоты)

    // Рисуем первую диагональную линию (сверху слева вниз направо)
    epd_paint_drawLine(x, y, x + width, y + size1, color, LineWidth);

    // Рисуем вторую диагональную линию (сверху справа вниз налево)
    epd_paint_drawLine(x + width, y, x, y + size1, color, LineWidth);
}
void vector_Y(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы Y
    uint16_t width = size1 / 2;  // Ширина буквы Y (половина высоты)
    uint16_t midY = y + size1 / 2;  // Середина высоты

    // Рисуем левую наклонную линию (верхняя часть)
    epd_paint_drawLine(x, y, x + width / 2, midY, color, LineWidth);

    // Рисуем правую наклонную линию (верхняя часть)
    epd_paint_drawLine(x + width, y, x + width / 2, midY, color, LineWidth);

    // Рисуем вертикальную линию (нижняя часть)
    epd_paint_drawLine(x + width / 2, midY, x + width / 2, y+size1, color, LineWidth);
}
void vector_Z(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы Z
    uint16_t width = size1 / 2;  // Ширина буквы Z (половина высоты)

    // Рисуем верхнюю горизонтальную линию
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Рисуем диагональную линию
    epd_paint_drawLine(x+ width, y, x, y + size1 , color, LineWidth);

    // Рисуем нижнюю горизонтальную линию
    epd_paint_drawLine(x, y + size1, x + width, y + size1, color, LineWidth);
}
void vector_0(uint16_t x, uint16_t y, uint16_t size1, uint16_t color,uint16_t LineWidth) {
    // Определяем параметры буквы O
	uint16_t size;
	size = size1/2;
    uint16_t radius = size / 2;  // Радиус буквы O (половина высоты)

    // Рисуем окружность (буква O)
    epd_paint_drawSemiEllipse(x + radius,y + size, radius ,size , color, 0, 4, LineWidth);// Пустой круг
}
void vector_1(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t width = size1 / 2;

    // Опционально: небольшая диагональная черта сверху влево (как засечка)
    epd_paint_drawLine(x + width / 2, y, x + width / 8, y + size1 / 4, color, LineWidth);

    // Основная вертикальная линия
    epd_paint_drawLine(x + width / 2, y, x + width / 2, y + size1, color, LineWidth);
}
void vector_2(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t width = size1 / 2;
    uint16_t radius = size1 / 4;
    uint16_t bottomY = y + size1;

    // Верхний полукруг вправо
    epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 0, 3, LineWidth);  // правый верхний полукруг

    // Диагональ от конца полукруга к нижнему левому углу
    epd_paint_drawLine(x + radius * 2, y + radius * 1.9, x, bottomY - LineWidth, color, LineWidth);

    // Нижняя горизонтальная линия
    epd_paint_drawLine(x, bottomY, x + width, bottomY, color, LineWidth);
    // Верхняя горизонтальная линия
    epd_paint_drawLine(x + width/5, y, x + width/2, y, color, LineWidth);
}
void vector_3(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t width = size1 / 2;
    uint16_t radius = size1 / 4;

    // Верхний полукруг (правая половина)
    epd_paint_drawSemiCircle(x + radius, y + radius, radius, color, 0, 3, LineWidth);

    // Нижний полукруг (правая половина)
    epd_paint_drawSemiCircle(x + radius, y + size1 - radius, radius, color, 0, 3, LineWidth);

    // верхняя горизонтальная линия (связка между полукругами)
    epd_paint_drawLine(x , y, x + width/2, y, color, LineWidth);
    // нижняя горизонтальная линия (связка между полукругами)
    epd_paint_drawLine(x ,y+size1, x + width/2, y+size1, color, LineWidth);
}
void vector_4(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t width = size1 / 2;

    // Рисуем диагональную линию (левая наклонная)
    epd_paint_drawLine(x + width, y, x, y + size1 / 2, color, LineWidth);

    // Рисуем горизонтальную линию посередине
    epd_paint_drawLine(x, y + size1 / 2, x + width, y + size1 / 2, color, LineWidth);

    // Рисуем вертикальную линию справа
    epd_paint_drawLine(x + width, y, x + width, y + size1, color, LineWidth);
}
void vector_5(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t width = size1 / 2;
    uint16_t midY = y + size1 / 2;
    uint16_t radius = size1 / 4;

    // Верхняя горизонтальная линия
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Левая вертикальная линия сверху до середины
    epd_paint_drawLine(x, y, x, midY, color, LineWidth);

    // Средняя горизонтальная линия
    epd_paint_drawLine(x, midY, x + width/2, midY, color, LineWidth);

    // Нижний полукруг (правый)
    epd_paint_drawSemiCircle(x + radius, y + size1 - radius, radius, color, 0, 3, LineWidth);

    // нижняя горизонтальная линия (связка между полукругами)
    epd_paint_drawLine(x ,y+size1, x + width/2, y+size1, color, LineWidth);
}
void vector_6(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t radius = size1 / 4;
	uint16_t centerX = x + radius;
	uint16_t centerY = y + size1 - radius;

	// Верхний круг (петля)
	epd_paint_drawSemiEllipse(centerX, centerY, radius, radius, color, 0, 4, LineWidth);

	// Нисходящая диагональ/линия
	epd_paint_drawLine(x, centerY,centerX , y, color, LineWidth);
}
void vector_7(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t width = size1 / 2;

    // Верхняя горизонтальная линия
    epd_paint_drawLine(x, y, x + width, y, color, LineWidth);

    // Диагональная линия вниз вправо
    epd_paint_drawLine(x + width, y, x, y + size1, color, LineWidth);
}
void vector_8(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t radius = size1 / 4;
    uint16_t centerX = x + radius;
    uint16_t centerTopY = y + radius;
    uint16_t centerBottomY = y + size1 - radius;

    // Верхний овал
    epd_paint_drawSemiEllipse(centerX, centerTopY, radius, radius, color, 0, 4, LineWidth);

    // Нижний овал
    epd_paint_drawSemiEllipse(centerX, centerBottomY, radius, radius, color, 0, 4, LineWidth);
}
void vector_9(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
    uint16_t radius = size1 / 4;
    uint16_t centerX = x + radius;
    uint16_t centerY = y + radius;

    // Верхний круг (петля)
    epd_paint_drawSemiEllipse(centerX, centerY, radius, radius, color, 0, 4, LineWidth);

    // Нисходящая диагональ/линия
    epd_paint_drawLine(centerX + radius, centerY, x, y + size1, color, LineWidth);
}
void vector_point(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth){
	epd_paint_drawSemiCircle(x+size1/2, y + size1, 3, color, 1, 3, LineWidth);
}
void vector_minus(x,y,size1,color,LineWidth){
	uint16_t width = size1 / 2;
	epd_paint_drawLine(x, y + size1/2, x+width, y + size1/2, color, LineWidth);
}
void vector_plus(x,y,size1,color,LineWidth){
	uint16_t width = size1 / 2;
	epd_paint_drawLine(x, y + size1/2, x+width, y + size1/2, color, LineWidth);
	epd_paint_drawLine(x+width/2, y+size1/4, x+width/2, y + size1*0.75, color, LineWidth);
}

void epd_paint_showChar_vector(uint16_t x, uint16_t y, uint16_t chr, uint16_t size1, uint16_t color){
	uint16_t LineWidth;
	LineWidth = size1/10;
	if (chr !="" && chr !='.'){
		epd_paint_drawRectangle(x-1, y-1, x+size1*0.75+1, y+size1+2, EPD_COLOR_WHITE, 1);
	}
	if (LineWidth == 0) LineWidth = 1;
	switch (chr){
	case 65:
		vector_A(x,y,size1,color,LineWidth);
		break;
	case 66:
		vector_B(x,y,size1,color,LineWidth);
		break;
	case 67:
		vector_C(x,y,size1,color,LineWidth);
		break;
	case 68:
		vector_D(x,y,size1,color,LineWidth);
		break;
	case 'E':
		vector_E(x,y,size1,color,LineWidth);
		break;
	case 'F':
		vector_F(x,y,size1,color,LineWidth);
		break;
	case 'G':
		vector_G(x,y,size1,color,LineWidth);
		break;
	case 'H':
		vector_H(x,y,size1,color,LineWidth);
		break;
	case 'I':
		vector_I(x,y,size1,color,LineWidth);
		break;
	case 'J':
		vector_J(x,y,size1,color,LineWidth);
		break;
	case 'K':
		vector_K(x,y,size1,color,LineWidth);
		break;
	case 'L':
		vector_L(x,y,size1,color,LineWidth);
		break;
	case 'M':
		vector_M(x,y,size1,color,LineWidth);
		break;
	case 'N':
		vector_N(x,y,size1,color,LineWidth);
		break;
	case 'O':
		vector_O(x,y,size1,color,LineWidth);
		break;
	case 'P':
		vector_P(x,y,size1,color,LineWidth);
		break;
	case 'Q':
		vector_Q(x,y,size1,color,LineWidth);
		break;
	case 'R':
		vector_R(x,y,size1,color,LineWidth);
		break;
	case 'S':
		vector_S(x,y,size1,color,LineWidth);
		break;
	case 'T':
		vector_T(x,y,size1,color,LineWidth);
		break;
	case 'U':
		vector_U(x,y,size1,color,LineWidth);
		break;
	case 'V':
		vector_V(x,y,size1,color,LineWidth);
		break;
	case 'W':
		vector_W(x,y,size1,color,LineWidth);
		break;
	case 'X':
		vector_X(x,y,size1,color,LineWidth);
		break;
	case 'Y':
		vector_Y(x,y,size1,color,LineWidth);
		break;
	case 'Z':
		vector_Z(x,y,size1,color,LineWidth);
		break;
	case '0':
		vector_0(x,y,size1,color,LineWidth);
		break;
	case '1':
		vector_1(x,y,size1,color,LineWidth);
		break;
	case '2':
		vector_2(x,y,size1,color,LineWidth);
		break;
	case '3':
		vector_3(x,y,size1,color,LineWidth);
		break;
	case '4':
		vector_4(x,y,size1,color,LineWidth);
		break;
	case '5':
		vector_5(x,y,size1,color,LineWidth);
		break;
	case '6':
		vector_6(x,y,size1,color,LineWidth);
		break;
	case '7':
		vector_7(x,y,size1,color,LineWidth);
		break;
	case '8':
		vector_8(x,y,size1,color,LineWidth);
		break;
	case '9':
		vector_9(x,y,size1,color,LineWidth);
		break;
	case ' ':

		break;
	case '.':
		vector_point(x,y,size1,color,LineWidth);
		break;
	case '-':
		vector_minus(x,y,size1,color,LineWidth);
		break;
	case '+':
		vector_plus(x,y,size1,color,LineWidth);
		break;
	 default:
		 vector_Z(10,10,30,color,LineWidth);
	}
}
