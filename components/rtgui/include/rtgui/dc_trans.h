#ifndef __RTGUI_DC_TRANS_H__
#define __RTGUI_DC_TRANS_H__

#include <rtgui/dc.h>
#include <rtgui/matrix.h>

struct rtgui_dc_trans;

struct rtgui_dc_trans* rtgui_trans_create(struct rtgui_dc *owner);

void rtgui_dc_trans_rotate(struct rtgui_dc_trans *dct, int degree);

void rtgui_dc_trans_scale(struct rtgui_dc_trans *dct,
                          int sx,
                          int xy);

void rtgui_dc_trans_move(struct rtgui_dc_trans *dct,
                         int dx,
                         int dy);

void rtgui_dc_trans_get_new_wh(struct rtgui_dc_trans *dct,
                               int *new_w,
                               int *new_h);

void rtgui_dc_trans_blit(struct rtgui_dc_trans *dct,
                         struct rtgui_point *dc_point,
                         struct rtgui_dc *dest,
                         struct rtgui_rect *rect);

void rtgui_dc_trans_destroy(struct rtgui_dc_trans *dct);

#endif
