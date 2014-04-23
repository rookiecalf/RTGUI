/**
 * DC buffer for Window 
 */

#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>

#define _OWNER(dc)	RTGUI_WIDGET(dc->owner)
#define _DC(dc)		(struct rtgui_dc*)(dc->buffer)

static rt_bool_t rtgui_dc_win_fini(struct rtgui_dc *dc);
static void rtgui_dc_win_draw_point(struct rtgui_dc *dc, int x, int y);
static void rtgui_dc_win_draw_color_point(struct rtgui_dc *dc, int x, int y, rtgui_color_t color);
static void rtgui_dc_win_draw_vline(struct rtgui_dc *dc, int x, int y1, int y2);
static void rtgui_dc_win_draw_hline(struct rtgui_dc *dc, int x1, int x2, int y);
static void rtgui_dc_win_fill_rect(struct rtgui_dc *dc, struct rtgui_rect *rect);
static void rtgui_dc_win_blit_line(struct rtgui_dc *self, int x1, int x2, int y, rt_uint8_t *line_data);
static void rtgui_dc_win_blit(struct rtgui_dc *self, struct rtgui_point *dc_point,
                                 struct rtgui_dc *dest, rtgui_rect_t *rect);

const static struct rtgui_dc_engine dc_win_engine =
{
    rtgui_dc_win_draw_point,
    rtgui_dc_win_draw_color_point,
    rtgui_dc_win_draw_vline,
    rtgui_dc_win_draw_hline,
    rtgui_dc_win_fill_rect,
    rtgui_dc_win_blit_line,
    rtgui_dc_win_blit,

    rtgui_dc_win_fini,
};

static rt_bool_t rtgui_dc_win_fini(struct rtgui_dc *dc)
{
	struct rtgui_dc_win* dc_win;

	RT_ASSERT(dc != RT_NULL);
	if (dc->type != RTGUI_DC_WIN) return RT_FALSE;

	dc_win = (struct rtgui_dc_win*)dc;
	rtgui_dc_destory(_DC(dc_win));

	return RT_TRUE;
}

static void rtgui_dc_win_draw_point(struct rtgui_dc *dc, int x, int y)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;

	RT_ASSERT(dc != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);

	/* get logic coordinate */
	x -= owner->extent.x1;
	y -= owner->extent.y1;

	rtgui_dc_draw_point(_DC(dc_win), x, y);
}

static void rtgui_dc_win_draw_color_point(struct rtgui_dc *dc, int x, int y, rtgui_color_t color)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;

	RT_ASSERT(dc != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);

	/* get logic coordinate */
	x -= owner->extent.x1;
	y -= owner->extent.y1;

	rtgui_dc_draw_color_point(_DC(dc_win), x, y, color);
}

static void rtgui_dc_win_draw_vline(struct rtgui_dc *dc, int x, int y1, int y2)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;

	RT_ASSERT(dc != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);

	/* get logic coordinate */
	x  -= owner->extent.x1;
	y1 -= owner->extent.y1;
	y2 -= owner->extent.y1;

	rtgui_dc_draw_vline(_DC(dc_win), x, y1, y2);
}

static void rtgui_dc_win_draw_hline(struct rtgui_dc *dc, int x1, int x2, int y)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;

	RT_ASSERT(dc != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);
	
	/* get logic coordinate */
	x1 -= owner->extent.x1;
	y  -= owner->extent.y1;
	x2 -= owner->extent.x1;

	rtgui_dc_draw_hline(_DC(dc_win), x1, x2, y);
}

static void rtgui_dc_win_fill_rect(struct rtgui_dc *dc, struct rtgui_rect *rect)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;
	struct rtgui_rect r;
	
	RT_ASSERT(dc != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);

	r = *rect;
	rtgui_widget_rect_to_logic(owner, &r);
		
	rtgui_dc_fill_rect(_DC(dc_win), &r);
}

static void rtgui_dc_win_blit_line(struct rtgui_dc *dc, int x1, int x2, int y, rt_uint8_t *line_data)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;

	RT_ASSERT(dc != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);

	/* get logic coordinate */
	x1 -= owner->extent.x1;
	y  -= owner->extent.y1;
	x2 -= owner->extent.x1;

	(_DC(dc_win))->engine->blit_line(_DC(dc_win), x1, x2, y, line_data);
}

static void rtgui_dc_win_blit(struct rtgui_dc *dc, struct rtgui_point *dc_point,
	struct rtgui_dc *dest, rtgui_rect_t *rect)
{
	struct rtgui_dc_win* dc_win;
	struct rtgui_widget* owner;
	struct rtgui_point p;
	
	RT_ASSERT(dc != RT_NULL);
	RT_ASSERT(dest != RT_NULL);

	dc_win = (struct rtgui_dc_win*)dc;
	owner = _OWNER(dc_win);
	p = *dc_point;

	rtgui_widget_point_to_logic(owner, &p);
	rtgui_dc_blit(_DC(dc_win), &p, dest, rect);
}

struct rtgui_dc* rtgui_dc_win_create(struct rtgui_win* window)
{
	struct rtgui_widget* owner;
	struct rtgui_dc_win* dc;
	RT_ASSERT(window != RT_NULL);

	dc = (struct rtgui_dc_win*) rtgui_malloc (sizeof(struct rtgui_dc_win));
	if (dc != RT_NULL)
	{
		int w, h;

		owner = RTGUI_WIDGET(window);
		w = owner->extent.x2 - owner->extent.x1;
		h = owner->extent.y2 - owner->extent.y1;

		dc->buffer = (struct rtgui_dc_buffer*)rtgui_dc_buffer_create(w, h);
		if (dc->buffer == RT_NULL)
		{
			/* out of memory */
			rtgui_free(dc);
			dc = RT_NULL;
		}
		else
		{
			/* initialize DC window buffer */
			dc->owner = window;

			dc->parent.type = RTGUI_DC_WIN;
			dc->parent.engine = &dc_win_engine;
		}
	}
	
	return (struct rtgui_dc*)dc;
}
