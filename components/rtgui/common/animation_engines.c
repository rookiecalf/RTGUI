#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/animation.h>

void rtgui_anim_engine_move(struct rtgui_dc *background,
                            struct rtgui_dc_buffer *background_buffer,
                            struct rtgui_dc_buffer *items,
                            int item_cnt,
                            int progress,
                            void *param)
{
    int cx, cy, w, h;
    struct rtgui_anim_engine_move_ctx *ctx = param;
    struct rtgui_rect dc_rect;

    if (!background)
        return;

    if (background_buffer)
        rtgui_dc_blit((struct rtgui_dc*)background_buffer,
                      NULL, background, NULL);

    rtgui_dc_get_rect(background, &dc_rect);

    cx = progress * (ctx->end.x - ctx->start.x) / RTGUI_ANIM_TICK_RANGE;
    cy = progress * (ctx->end.y - ctx->start.y) / RTGUI_ANIM_TICK_RANGE;
    w = rtgui_rect_width(dc_rect);
    h = rtgui_rect_height(dc_rect);

    dc_rect.x1 = cx + ctx->start.x;
    dc_rect.y1 = cy + ctx->start.y;
    dc_rect.x2 = dc_rect.x1 + w;
    dc_rect.y2 = dc_rect.y1 + h;

    /* To prevent overlapping, only one item can be drawn by
     * rtgui_anim_engine_move. */
    if (items)
        rtgui_dc_blit((struct rtgui_dc*)(items), NULL, background, &dc_rect);
}