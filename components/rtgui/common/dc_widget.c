/**
 * DC buffer for Window 
 */

#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/window.h>

#define _OWNER(dc)	(dc->owner)
#define _WIN(dc)	(RTGUI_WIDGET(dc->owner->toplevel))
#define _DC(dc)		((struct rtgui_dc*)(dc->buffer))

#define _DX(dc)		(dc->offset_x)
#define _DY(dc)		(dc->offset_y)

static rt_bool_t rtgui_dc_widget_fini(struct rtgui_dc *dc);
static void rtgui_dc_widget_draw_point(struct rtgui_dc *dc, int x, int y);
static void rtgui_dc_widget_draw_color_point(struct rtgui_dc *dc, int x, int y, rtgui_color_t color);
static void rtgui_dc_widget_draw_vline(struct rtgui_dc *dc, int x, int y1, int y2);
static void rtgui_dc_widget_draw_hline(struct rtgui_dc *dc, int x1, int x2, int y);
static void rtgui_dc_widget_fill_rect(struct rtgui_dc *dc, struct rtgui_rect *rect);
static void rtgui_dc_widget_blit_line(struct rtgui_dc *self, int x1, int x2, int y, rt_uint8_t *line_data);
static void rtgui_dc_widget_blit(struct rtgui_dc *self, struct rtgui_point *dc_point,
		struct rtgui_dc *dest, rtgui_rect_t *rect);

const static struct rtgui_dc_engine dc_widget_engine =
{
    rtgui_dc_widget_draw_point,
    rtgui_dc_widget_draw_color_point,
    rtgui_dc_widget_draw_vline,
    rtgui_dc_widget_draw_hline,
    rtgui_dc_widget_fill_rect,
    rtgui_dc_widget_blit_line,
    rtgui_dc_widget_blit,

    rtgui_dc_widget_fini,
};

static rt_bool_t rtgui_dc_widget_fini(struct rtgui_dc *dc)
{
	struct rtgui_dc_widget* dc_widget;

	RT_ASSERT(dc != RT_NULL);
	if (dc->type != RTGUI_DC_WIDGET) return RT_FALSE;

	dc_widget = (struct rtgui_dc_widget*)dc;

	dc_widget->buffer = RT_NULL;
	dc_widget->owner = RT_NULL;

	return RT_TRUE;
}

static void rtgui_dc_widget_draw_point(struct rtgui_dc *dc, int x, int y)
{
	struct rtgui_dc_widget* dc_widget;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);

	/* get logic coordinate */
	x += _DX(dc_widget);
	y += _DY(dc_widget);

	rtgui_dc_draw_point(_DC(dc_widget), x, y);
}

static void rtgui_dc_widget_draw_color_point(struct rtgui_dc *dc, int x, int y, rtgui_color_t color)
{
	struct rtgui_dc_widget* dc_widget;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);

	/* get logic coordinate */
	x += _DX(dc_widget);
	y += _DY(dc_widget);

	rtgui_dc_draw_color_point(_DC(dc_widget), x, y, color);
}

static void rtgui_dc_widget_draw_vline(struct rtgui_dc *dc, int x, int y1, int y2)
{
	struct rtgui_dc_widget* dc_widget;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);

	/* get logic coordinate */
	x  += _DX(dc_widget);
	y1 += _DY(dc_widget);
	y2 += _DY(dc_widget);

	rtgui_dc_draw_vline(_DC(dc_widget), x, y1, y2);
}

static void rtgui_dc_widget_draw_hline(struct rtgui_dc *dc, int x1, int x2, int y)
{
	struct rtgui_dc_widget* dc_widget;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);
	
	/* get logic coordinate */
	x1 += _DX(dc_widget);
	y  += _DY(dc_widget);
	x2 += _DX(dc_widget);

	rtgui_dc_draw_hline(_DC(dc_widget), x1, x2, y);
}

static void rtgui_dc_widget_fill_rect(struct rtgui_dc *dc, struct rtgui_rect *rect)
{
	struct rtgui_dc_widget* dc_widget;
	struct rtgui_rect r;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);

	r = *rect;
	rtgui_rect_moveto(&r, _DX(dc_widget), _DY(dc_widget));
	rtgui_dc_fill_rect(_DC(dc_widget), &r);
}

static void rtgui_dc_widget_blit_line(struct rtgui_dc *dc, int x1, int x2, int y, rt_uint8_t *line_data)
{
	struct rtgui_dc_widget* dc_widget;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);

	/* get logic coordinate */
	x1 += _DX(dc_widget);
	y  += _DY(dc_widget);
	x2 += _DX(dc_widget);

	(_DC(dc_widget))->engine->blit_line(_DC(dc_widget), x1, x2, y, line_data);
}

static void rtgui_dc_widget_blit(struct rtgui_dc *dc, struct rtgui_point *dc_point,
	struct rtgui_dc *dest, rtgui_rect_t *rect)
{
	struct rtgui_point p;
	struct rtgui_dc_widget* dc_widget;

	dc_widget = (struct rtgui_dc_widget*)dc;
	RT_ASSERT(dc_widget != RT_NULL);

	p = *dc_point;
	p.x += _DX(dc_widget);
	p.y += _DY(dc_widget);

	rtgui_dc_blit(_DC(dc_widget), &p, dest, rect);
}

struct rtgui_dc* rtgui_dc_widget_create_from_buffer(struct rtgui_widget* owner,
                                                    struct rtgui_widget *parent,
                                                    struct rtgui_dc_buffer* buffer)
{
	struct rtgui_dc_widget* dc;

	RT_ASSERT(owner != RT_NULL);
	RT_ASSERT(parent != RT_NULL);

    if (!buffer)
        return RT_NULL;

	dc = (struct rtgui_dc_widget*) rtgui_malloc (sizeof(struct rtgui_dc_widget));
	if (dc != RT_NULL)
	{
		dc->owner = owner;
		dc->offset_x = (_OWNER(dc)->extent.x1 - (parent->extent.x1));
		dc->offset_y = (_OWNER(dc)->extent.y1 - (parent->extent.y1));
		dc->buffer = buffer;

		dc->parent.type = RTGUI_DC_WIDGET;
		dc->parent.engine = &dc_widget_engine;
	}

	return (struct rtgui_dc*)dc;
}
RTM_EXPORT(rtgui_dc_widget_create_from_buffer);

struct rtgui_dc* rtgui_dc_widget_create(struct rtgui_widget* owner)
{
    return rtgui_dc_widget_create_from_buffer(owner,
                                              RTGUI_WIDGET(owner->toplevel),
                                              owner->toplevel->buffer);
}
RTM_EXPORT(rtgui_dc_widget_create);

