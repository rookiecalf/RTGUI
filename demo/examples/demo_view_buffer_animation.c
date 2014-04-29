#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/animation.h>
#include <rtgui/widgets/container.h>
#include "demo_view.h"

/*
 * 直接在DC上绘图以实现动画效果
 */

static struct rtgui_dc *dc_buffer;
static struct rtgui_animation *anim;
static struct rtgui_anim_engine_move_ctx engctx;

static rt_bool_t animation_on_show(struct rtgui_object *object, struct rtgui_event *event)
{
    rt_kprintf("buffer animation on show\n");
    if (anim)
        rtgui_anim_start(anim);

    return RT_TRUE;
}

static rt_bool_t animation_on_hide(struct rtgui_object *object, struct rtgui_event *event)
{
    rt_kprintf("buffer animation on hide\n");
    if (anim)
        rtgui_anim_stop(anim);

    return RT_TRUE;
}

static rt_bool_t animation_event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    if (event->type == RTGUI_EVENT_SHOW)
    {
        rtgui_container_event_handler(object, event);
        animation_on_show(object, event);
    }
    else if (event->type == RTGUI_EVENT_HIDE)
    {
        rtgui_container_event_handler(object, event);
        animation_on_hide(object, event);
    }
    else
    {
        /* 调用默认的事件处理函数 */
        return rtgui_container_event_handler(object, event);
    }

    return RT_FALSE;
}

static void _swap(rt_int16_t *i1, rt_int16_t *i2)
{
    rt_uint16_t tmp;

    tmp = *i1;
    *i1 = *i2;
    *i2 = tmp;
}

static void _update_engctx(struct rtgui_animation *anim)
{
    _swap(&engctx.start.y, &engctx.end.y);
    rtgui_anim_start(anim);
}

struct rtgui_container *demo_view_buffer_animation(void)
{
    struct rtgui_container *container;
    static rtgui_rect_t text_rect;
    rtgui_rect_t bufrect;

    /*
     *int i = 1;
     *while (i);
     */

    container = demo_view("DC 缓冲区动画");
    if (container != RT_NULL)
        rtgui_object_set_event_handler(RTGUI_OBJECT(container), animation_event_handler);

    rtgui_font_get_metrics(RTGUI_WIDGET_FONT(container), "缓冲动画", &text_rect);
    if (dc_buffer == RT_NULL)
    {

        bufrect.x1 = 0;
        bufrect.x2 = rtgui_rect_width(text_rect) + 2;
        bufrect.y1 = 0;
        bufrect.y2 = rtgui_rect_height(text_rect) + 2;

        dc_buffer = rtgui_dc_buffer_create(rtgui_rect_width(bufrect), rtgui_rect_height(bufrect));
        RTGUI_DC_FC(dc_buffer) = RTGUI_WIDGET_BACKGROUND(container);
        rtgui_dc_fill_rect(dc_buffer, &bufrect);
        RTGUI_DC_FC(dc_buffer) = black;
        bufrect.x1 = 1;
        bufrect.y1 = 1;
        rtgui_dc_draw_text(dc_buffer, "缓冲动画", &bufrect);
    }

    if (anim == RT_NULL)
    {
        struct rtgui_rect rect;

        demo_view_get_logic_rect(container, &rect);

        /* 25 frames per second should be reasonable. */
        anim = rtgui_anim_create(RTGUI_WIDGET(container), RT_TICK_PER_SECOND/25);
        RT_ASSERT(anim);

        rtgui_anim_set_dc_buffer(anim, (struct rtgui_dc_buffer*)dc_buffer, 1);
        rtgui_anim_set_onfinish(anim, _update_engctx);
        rtgui_anim_set_duration(anim, 1 * RT_TICK_PER_SECOND);
        rtgui_anim_set_motion(anim, rtgui_anim_motion_insquare);
        rtgui_anim_set_engine(anim, rtgui_anim_engine_move, &engctx);

        engctx.start.x = rect.x1 + 2;
        engctx.start.y = rect.y1 + 2;
        engctx.end.x = rect.x2 - bufrect.x2 - 2;
        engctx.end.y = rect.y2 - bufrect.y2 - 2;
    }

    return container;
}
