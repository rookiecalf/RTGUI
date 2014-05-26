#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/dc.h>
#include <rtgui/dc_trans.h>

struct rtgui_dc_trans
{
    struct rtgui_matrix m;
    struct rtgui_dc *owner;
};

struct rtgui_dc_trans* rtgui_dc_trans_create(struct rtgui_dc *owner)
{
    struct rtgui_dc_trans *dct;

    dct = (struct rtgui_dc_trans*)rtgui_malloc(sizeof(*dct));
    if (!dct)
        return RT_NULL;

    dct->owner = owner;
    rtgu_matrix_identity(&dct->m);

    return dct;
}

void rtgui_dc_trans_destroy(struct rtgui_dc_trans *dct)
{
    rtgui_free(dct);
}

void rtgui_dc_trans_rotate(struct rtgui_dc_trans *dct, int degree)
{
    rtgui_matrix_rotate(&dct->m, degree);
}

void rtgui_dc_trans_scale(struct rtgui_dc_trans *dct,
                          int sx,
                          int sy)
{
    rtgui_matrix_scale(&dct->m, sx, sy);
}

void rtgui_dc_trans_move(struct rtgui_dc_trans *dct,
                         int dx,
                         int dy)
{
    rtgui_matrix_move(&dct->m, dx, dy);
}

void rtgui_dc_trans_get_new_wh(struct rtgui_dc_trans *dct,
                               int *new_wp,
                               int *new_hp)
{
    struct rtgui_rect rect;
    struct rtgui_point topleft, topright, bottomright;

    RT_ASSERT(dct);

    if (!new_wp && !new_hp)
        return;

    rtgui_dc_get_rect(dct->owner, &rect);

    /* We ignore the movement components in the matrix. */
    /* Transform result of (0, h). */
    rtgui_matrix_mul_point_nomove(&topleft, 0, rect.y2,
                                  &dct->m);
    /* Transform result of (w, h). */
    rtgui_matrix_mul_point_nomove(&topright, rect.x2, rect.y2,
                                  &dct->m);
    /* Transform result of (w, 0). */
    rtgui_matrix_mul_point_nomove(&bottomright,
                                   rect.x2, 0, &dct->m);
    /* Transform result of (0, 0) is always (0, 0). */

    if (new_wp)
    {
        *new_wp = _UI_MAX(_UI_ABS(topright.x),
                          _UI_ABS(topleft.x - bottomright.x));
    }
    if (new_hp)
    {
        *new_hp = _UI_MAX(_UI_ABS(topright.y),
                          _UI_ABS(topleft.y - bottomright.y));
    }
}

void rtgui_dc_trans_blit(struct rtgui_dc_trans *dct,
                         struct rtgui_point *dc_point,
                         struct rtgui_dc *dest,
                         struct rtgui_rect *rect)
{
    struct rtgui_matrix invm;
    struct rtgui_point pscale;
    struct rtgui_rect orirect;
    int neww, newh, oriw, orih;
    int nx, ny;

    RT_ASSERT(dct);
    RT_ASSERT(dest);

    if (rtgui_matrix_inverse(&dct->m, &invm))
        return;

    rtgui_dc_trans_get_new_wh(dct, &neww, &newh);
    rtgui_matrix_mul_point_nomove(&pscale, 1, 1, &invm);

    rtgui_dc_get_rect(dct->owner, &orirect);
    oriw = orirect.x2;
    orih = orirect.y2;

    for (nx = dc_point->x; nx < neww; nx++)
    {
        for (ny = dc_point->y; ny < newh; ny++)
        {
            struct rtgui_point orip;

            rtgui_matrix_mul_point(&orip, nx, ny, &invm);
            if (oriw < orip.x || orip.x < 0 || orih < orip.y || orip.y < 0)
                continue;
        }
    }
}
