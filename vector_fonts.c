/*
 * vector_fonts.c
 *
 *  Created on: Feb 26, 2025
 *      Author: GryaznevAA
 */
#include "stdint.h"

#include "epaper.h"
#include "main.h"

#define VF_BEZIER_STEPS 28U
#define VF_KAPPA_NUM 552U
#define VF_KAPPA_DEN 1000U
#define VF_T_SCALE 1024LL

typedef struct {
	int32_t x;
	int32_t y;
} vf_point_t;

typedef struct {
	vf_point_t lastP3;
	vf_point_t lastC2;
	uint8_t hasPrev;
} vf_smooth_state_t;

static vf_smooth_state_t vfSmoothState;

static void vf_resetSmoothState(void) {
	vfSmoothState.hasPrev = 0U;
}

static void vf_drawBezierCubic(vf_point_t p0, vf_point_t p1, vf_point_t p2, vf_point_t p3,
		uint16_t color, uint16_t lineWidth) {
	uint32_t i;
	vf_point_t prev = p0;

	for (i = 1; i <= VF_BEZIER_STEPS; ++i) {
		const int32_t t = (int32_t) ((i * VF_T_SCALE) / VF_BEZIER_STEPS);
		const int32_t u = (int32_t) (VF_T_SCALE - t);
		const int64_t b0 = (int64_t) u * u * u;
		const int64_t b1 = 3LL * u * u * t;
		const int64_t b2 = 3LL * u * t * t;
		const int64_t b3 = (int64_t) t * t * t;
		vf_point_t cur;

		cur.x = (int32_t) ((b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x)
				/ (VF_T_SCALE * VF_T_SCALE * VF_T_SCALE));
		cur.y = (int32_t) ((b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y)
				/ (VF_T_SCALE * VF_T_SCALE * VF_T_SCALE));

		epd_paint_drawLine((uint16_t) prev.x, (uint16_t) prev.y,
				(uint16_t) cur.x, (uint16_t) cur.y, color, lineWidth);
		prev = cur;
	}
}

static void vf_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t color, uint16_t lineWidth) {
	const int32_t dx = (int32_t) x1 - (int32_t) x0;
	const int32_t dy = (int32_t) y1 - (int32_t) y0;
	const int32_t dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
	const int32_t bow = dist / 14 + 1;
	const int32_t nx = (dy == 0 && dx == 0) ? 0 : ((dy >= 0) ? 1 : -1);
	const int32_t ny = (dy == 0 && dx == 0) ? 0 : ((dx >= 0) ? -1 : 1);
	vf_point_t p0 = { x0, y0 };
	vf_point_t p1;
	vf_point_t p2 = {
		((int32_t) x0 + 2 * (int32_t) x1) / 3 + nx * bow,
		((int32_t) y0 + 2 * (int32_t) y1) / 3 + ny * bow
	};
	vf_point_t p3 = { x1, y1 };

	if (vfSmoothState.hasPrev && vfSmoothState.lastP3.x == p0.x && vfSmoothState.lastP3.y == p0.y) {
		p1.x = 2 * p0.x - vfSmoothState.lastC2.x;
		p1.y = 2 * p0.y - vfSmoothState.lastC2.y;
	} else {
		p1.x = (2 * (int32_t) x0 + (int32_t) x1) / 3 + nx * bow;
		p1.y = (2 * (int32_t) y0 + (int32_t) y1) / 3 + ny * bow;
	}

	vf_drawBezierCubic(p0, p1, p2, p3, color, lineWidth);
	vfSmoothState.lastP3 = p3;
	vfSmoothState.lastC2 = p2;
	vfSmoothState.hasPrev = 1U;
}

static void vf_drawArcQuarter(uint16_t cx, uint16_t cy, uint16_t rx, uint16_t ry,
		uint8_t quarter, uint16_t color, uint16_t lineWidth) {
	const int32_t kx = ((int32_t) rx * VF_KAPPA_NUM) / VF_KAPPA_DEN;
	const int32_t ky = ((int32_t) ry * VF_KAPPA_NUM) / VF_KAPPA_DEN;
	vf_point_t p0, p1, p2, p3;

	switch (quarter & 3U) {
	case 0: /* top -> right */
		p0 = (vf_point_t ) { cx, cy - (int32_t) ry };
		p1 = (vf_point_t ) { cx + kx, cy - (int32_t) ry };
		p2 = (vf_point_t ) { cx + (int32_t) rx, cy - ky };
		p3 = (vf_point_t ) { cx + (int32_t) rx, cy };
		break;
	case 1: /* right -> bottom */
		p0 = (vf_point_t ) { cx + (int32_t) rx, cy };
		p1 = (vf_point_t ) { cx + (int32_t) rx, cy + ky };
		p2 = (vf_point_t ) { cx + kx, cy + (int32_t) ry };
		p3 = (vf_point_t ) { cx, cy + (int32_t) ry };
		break;
	case 2: /* bottom -> left */
		p0 = (vf_point_t ) { cx, cy + (int32_t) ry };
		p1 = (vf_point_t ) { cx - kx, cy + (int32_t) ry };
		p2 = (vf_point_t ) { cx - (int32_t) rx, cy + ky };
		p3 = (vf_point_t ) { cx - (int32_t) rx, cy };
		break;
	default: /* left -> top */
		p0 = (vf_point_t ) { cx - (int32_t) rx, cy };
		p1 = (vf_point_t ) { cx - (int32_t) rx, cy - ky };
		p2 = (vf_point_t ) { cx - kx, cy - (int32_t) ry };
		p3 = (vf_point_t ) { cx, cy - (int32_t) ry };
		break;
	}

	if (vfSmoothState.hasPrev && vfSmoothState.lastP3.x == p0.x && vfSmoothState.lastP3.y == p0.y) {
		p1.x = 2 * p0.x - vfSmoothState.lastC2.x;
		p1.y = 2 * p0.y - vfSmoothState.lastC2.y;
	}
	vf_drawBezierCubic(p0, p1, p2, p3, color, lineWidth);
	vfSmoothState.lastP3 = p3;
	vfSmoothState.lastC2 = p2;
	vfSmoothState.hasPrev = 1U;
}

static void vf_drawOval(uint16_t cx, uint16_t cy, uint16_t rx, uint16_t ry,
		uint16_t color, uint16_t lineWidth) {
	vf_drawArcQuarter(cx, cy, rx, ry, 0, color, lineWidth);
	vf_drawArcQuarter(cx, cy, rx, ry, 1, color, lineWidth);
	vf_drawArcQuarter(cx, cy, rx, ry, 2, color, lineWidth);
	vf_drawArcQuarter(cx, cy, rx, ry, 3, color, lineWidth);
}

static uint16_t vf_w(uint16_t size1) {
	return (uint16_t) ((size1 * 3U) / 5U);
}

void vector_A(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y + size1, x + w / 2, y, color, LineWidth);
	vf_drawLine(x + w / 2, y, x + w, y + size1, color, LineWidth);
	vf_drawLine(x + w / 4, y + size1 * 3 / 5, x + w * 3 / 4, y + size1 * 3 / 5, color, LineWidth);
}

void vector_B(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	uint16_t r = w / 2;
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawArcQuarter(x + r, y + size1 / 4, r, size1 / 4, 3, color, LineWidth);
	vf_drawArcQuarter(x + r, y + size1 / 4, r, size1 / 4, 0, color, LineWidth);
	vf_drawArcQuarter(x + r, y + (size1 * 3) / 4, r, size1 / 4, 1, color, LineWidth);
	vf_drawArcQuarter(x + r, y + (size1 * 3) / 4, r, size1 / 4, 2, color, LineWidth);
}

void vector_C(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawArcQuarter(x + w / 2, y + size1 / 2, w / 2, size1 / 2, 2, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 2, w / 2, size1 / 2, 3, color, LineWidth);
}

void vector_D(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 2, w / 2, size1 / 2, 0, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 2, w / 2, size1 / 2, 1, color, LineWidth);
}

void vector_E(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawLine(x, y, x + w, y, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w * 4 / 5, y + size1 / 2, color, LineWidth);
	vf_drawLine(x, y + size1, x + w, y + size1, color, LineWidth);
}

void vector_F(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawLine(x, y, x + w, y, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w * 4 / 5, y + size1 / 2, color, LineWidth);
}

void vector_G(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	uint16_t cx = x + w / 2;
	uint16_t cy = y + size1 / 2;
	vf_drawArcQuarter(cx, cy, w / 2, size1 / 2, 2, color, LineWidth);
	vf_drawArcQuarter(cx, cy, w / 2, size1 / 2, 3, color, LineWidth);
	vf_drawArcQuarter(cx, cy, w / 2, size1 / 2, 0, color, LineWidth);
	vf_drawLine(cx, cy, x + w, cy, color, LineWidth);
}

void vector_H(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawLine(x + w, y, x + w, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w, y + size1 / 2, color, LineWidth);
}

void vector_I(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w, y, color, LineWidth);
	vf_drawLine(x + w / 2, y, x + w / 2, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1, x + w, y + size1, color, LineWidth);
}

void vector_J(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x + w, y, x + w, y + size1 - size1 / 4, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 - size1 / 4, w / 2, size1 / 4, 1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 - size1 / 4, w / 2, size1 / 4, 2, color, LineWidth);
}

void vector_K(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w, y, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w, y + size1, color, LineWidth);
}

void vector_L(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1, x + w, y + size1, color, LineWidth);
}

void vector_M(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y + size1, x, y, color, LineWidth);
	vf_drawLine(x, y, x + w / 2, y + size1 / 2, color, LineWidth);
	vf_drawLine(x + w / 2, y + size1 / 2, x + w, y, color, LineWidth);
	vf_drawLine(x + w, y, x + w, y + size1, color, LineWidth);
}

void vector_N(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y + size1, x, y, color, LineWidth);
	vf_drawLine(x, y, x + w, y + size1, color, LineWidth);
	vf_drawLine(x + w, y + size1, x + w, y, color, LineWidth);
}

void vector_O(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawOval(x + w / 2, y + size1 / 2, w / 2, size1 / 2, color, LineWidth);
}

void vector_P(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 3, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 0, color, LineWidth);
}

void vector_Q(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	uint16_t cx = x + w / 2;
	uint16_t cy = y + size1 / 2;
	vf_drawOval(cx, cy, w / 2, size1 / 2, color, LineWidth);
	vf_drawLine(cx + w / 6, cy + size1 / 6, x + w, y + size1, color, LineWidth);
}

void vector_R(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 3, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 0, color, LineWidth);
	vf_drawLine(x + w / 3, y + size1 / 2, x + w, y + size1, color, LineWidth);
}

void vector_S(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	uint16_t cx = x + w / 2;
	vf_drawArcQuarter(cx, y + size1 / 4, w / 2, size1 / 4, 2, color, LineWidth);
	vf_drawArcQuarter(cx, y + size1 / 4, w / 2, size1 / 4, 0, color, LineWidth);
	vf_drawArcQuarter(cx, y + (size1 * 3) / 4, w / 2, size1 / 4, 1, color, LineWidth);
	vf_drawArcQuarter(cx, y + (size1 * 3) / 4, w / 2, size1 / 4, 3, color, LineWidth);
}

void vector_T(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w, y, color, LineWidth);
	vf_drawLine(x + w / 2, y, x + w / 2, y + size1, color, LineWidth);
}

void vector_U(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x, y + size1 - size1 / 4, color, LineWidth);
	vf_drawLine(x + w, y, x + w, y + size1 - size1 / 4, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 - size1 / 4, w / 2, size1 / 4, 1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 - size1 / 4, w / 2, size1 / 4, 2, color, LineWidth);
}

void vector_V(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w / 2, y + size1, color, LineWidth);
	vf_drawLine(x + w, y, x + w / 2, y + size1, color, LineWidth);
}

void vector_W(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w / 4, y + size1, color, LineWidth);
	vf_drawLine(x + w / 4, y + size1, x + w / 2, y + size1 / 2, color, LineWidth);
	vf_drawLine(x + w / 2, y + size1 / 2, x + w * 3 / 4, y + size1, color, LineWidth);
	vf_drawLine(x + w * 3 / 4, y + size1, x + w, y, color, LineWidth);
}

void vector_X(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w, y + size1, color, LineWidth);
	vf_drawLine(x + w, y, x, y + size1, color, LineWidth);
}

void vector_Y(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w / 2, y + size1 / 2, color, LineWidth);
	vf_drawLine(x + w, y, x + w / 2, y + size1 / 2, color, LineWidth);
	vf_drawLine(x + w / 2, y + size1 / 2, x + w / 2, y + size1, color, LineWidth);
}

void vector_Z(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w, y, color, LineWidth);
	vf_drawLine(x + w, y, x, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1, x + w, y + size1, color, LineWidth);
}

void vector_0(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	vector_O(x, y, size1, color, LineWidth);
}

void vector_1(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x + w / 2, y, x + w / 2, y + size1, color, LineWidth);
	vf_drawLine(x + w / 4, y + size1 / 5, x + w / 2, y, color, LineWidth);
}

void vector_2(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 3, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 0, color, LineWidth);
	vf_drawLine(x + w, y + size1 / 4, x, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1, x + w, y + size1, color, LineWidth);
}

void vector_3(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 3, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 / 4, w / 2, size1 / 4, 0, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 * 3 / 4, w / 2, size1 / 4, 1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 * 3 / 4, w / 2, size1 / 4, 0, color, LineWidth);
}

void vector_4(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x + w, y, x + w, y + size1, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w, y + size1 / 2, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w, y, color, LineWidth);
}

void vector_5(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x + w, y, x, y, color, LineWidth);
	vf_drawLine(x, y, x, y + size1 / 2, color, LineWidth);
	vf_drawLine(x, y + size1 / 2, x + w / 2, y + size1 / 2, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 * 3 / 4, w / 2, size1 / 4, 1, color, LineWidth);
	vf_drawArcQuarter(x + w / 2, y + size1 * 3 / 4, w / 2, size1 / 4, 0, color, LineWidth);
}

void vector_6(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawOval(x + w / 2, y + size1 * 3 / 4, w / 2, size1 / 4, color, LineWidth);
	vf_drawLine(x, y + size1 * 3 / 4, x + w / 2, y, color, LineWidth);
}

void vector_7(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y, x + w, y, color, LineWidth);
	vf_drawLine(x + w, y, x, y + size1, color, LineWidth);
}

void vector_8(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawOval(x + w / 2, y + size1 / 4, w / 2, size1 / 4, color, LineWidth);
	vf_drawOval(x + w / 2, y + size1 * 3 / 4, w / 2, size1 / 4, color, LineWidth);
}

void vector_9(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawOval(x + w / 2, y + size1 / 4, w / 2, size1 / 4, color, LineWidth);
	vf_drawLine(x + w, y + size1 / 4, x, y + size1, color, LineWidth);
}

void vector_point(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t r = size1 / 12 + 1;
	vf_drawOval(x + vf_w(size1) / 2, y + size1 - r, r, r, color, LineWidth);
}

void vector_minus(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y + size1 / 2, x + w, y + size1 / 2, color, LineWidth);
}

void vector_plus(uint16_t x, uint16_t y, uint16_t size1, uint16_t color, uint16_t LineWidth) {
	uint16_t w = vf_w(size1);
	vf_drawLine(x, y + size1 / 2, x + w, y + size1 / 2, color, LineWidth);
	vf_drawLine(x + w / 2, y + size1 / 4, x + w / 2, y + (size1 * 3) / 4, color, LineWidth);
}

void epd_paint_showChar_vector(uint16_t x, uint16_t y, uint16_t chr, uint16_t size1, uint16_t color) {
	uint16_t LineWidth = size1 / 10;

	if (LineWidth == 0U) {
		LineWidth = 1U;
	}

	if (chr != ' ' && chr != '.') {
		epd_paint_drawRectangle(x - 1, y - 1, x + size1 * 3 / 4 + 1, y + size1 + 2, EPD_COLOR_WHITE, 1);
	}

	vf_resetSmoothState();

	switch (chr) {
	case 'A': case 'a': vector_A(x, y, size1, color, LineWidth); break;
	case 'B': case 'b': vector_B(x, y, size1, color, LineWidth); break;
	case 'C': case 'c': vector_C(x, y, size1, color, LineWidth); break;
	case 'D': case 'd': vector_D(x, y, size1, color, LineWidth); break;
	case 'E': case 'e': vector_E(x, y, size1, color, LineWidth); break;
	case 'F': case 'f': vector_F(x, y, size1, color, LineWidth); break;
	case 'G': case 'g': vector_G(x, y, size1, color, LineWidth); break;
	case 'H': case 'h': vector_H(x, y, size1, color, LineWidth); break;
	case 'I': case 'i': vector_I(x, y, size1, color, LineWidth); break;
	case 'J': case 'j': vector_J(x, y, size1, color, LineWidth); break;
	case 'K': case 'k': vector_K(x, y, size1, color, LineWidth); break;
	case 'L': case 'l': vector_L(x, y, size1, color, LineWidth); break;
	case 'M': case 'm': vector_M(x, y, size1, color, LineWidth); break;
	case 'N': case 'n': vector_N(x, y, size1, color, LineWidth); break;
	case 'O': case 'o': vector_O(x, y, size1, color, LineWidth); break;
	case 'P': case 'p': vector_P(x, y, size1, color, LineWidth); break;
	case 'Q': case 'q': vector_Q(x, y, size1, color, LineWidth); break;
	case 'R': case 'r': vector_R(x, y, size1, color, LineWidth); break;
	case 'S': case 's': vector_S(x, y, size1, color, LineWidth); break;
	case 'T': case 't': vector_T(x, y, size1, color, LineWidth); break;
	case 'U': case 'u': vector_U(x, y, size1, color, LineWidth); break;
	case 'V': case 'v': vector_V(x, y, size1, color, LineWidth); break;
	case 'W': case 'w': vector_W(x, y, size1, color, LineWidth); break;
	case 'X': case 'x': vector_X(x, y, size1, color, LineWidth); break;
	case 'Y': case 'y': vector_Y(x, y, size1, color, LineWidth); break;
	case 'Z': case 'z': vector_Z(x, y, size1, color, LineWidth); break;
	case '0': vector_0(x, y, size1, color, LineWidth); break;
	case '1': vector_1(x, y, size1, color, LineWidth); break;
	case '2': vector_2(x, y, size1, color, LineWidth); break;
	case '3': vector_3(x, y, size1, color, LineWidth); break;
	case '4': vector_4(x, y, size1, color, LineWidth); break;
	case '5': vector_5(x, y, size1, color, LineWidth); break;
	case '6': vector_6(x, y, size1, color, LineWidth); break;
	case '7': vector_7(x, y, size1, color, LineWidth); break;
	case '8': vector_8(x, y, size1, color, LineWidth); break;
	case '9': vector_9(x, y, size1, color, LineWidth); break;
	case '.': vector_point(x, y, size1, color, LineWidth); break;
	case '-': vector_minus(x, y, size1, color, LineWidth); break;
	case '+': vector_plus(x, y, size1, color, LineWidth); break;
	case ' ': break;
	default:
		vector_Z(x, y, size1, color, LineWidth);
		break;
	}
}
