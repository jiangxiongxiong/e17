/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Enesim.h"
#include "enesim_private.h"
/* TODO
 * + add support for sw and sh
 * + add support for qualities (good scaler, interpolate between the four neighbours)
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Surface
{
	Enesim_Renderer r;
	Enesim_Surface *s;
	int x, y;
	unsigned int w, h;
	int *yoff;
	int *xoff;
} Surface;

static inline void _offsets(unsigned int cs, unsigned int cl, unsigned int sl, int *off)
{
	int c;

	for (c = 0; c < cl; c++)
	{
		off[c] = ((c + cs) * sl) / cl;
	}
}

#if 0
static void _scale_good(Surface *s, int x, int y, unsigned int len, uint32_t *dst)
{
	uint32_t sstride;
	uint32_t *src;
	Eina_Rectangle ir, dr;
	int sy;
	int sw, sh;

	if (y < s->r.oy || y > s->r.oy + s->w)
	{
		while (len--)
			*dst++ = 0;
		return;
	}

	src = enesim_surface_data_get(s->s);
	sstride = enesim_surface_stride_get(s->s);
	enesim_surface_size_get(s->s, &sw, &sh);
	sy = s->yoff[y - s->r.oy];
	src += sstride * sy;
	x -= s->r.ox;

	while (len--)
	{
		if (x >= 0 && x < s->w)
		{
			uint32_t p0, p1, p2, p3;
			uint32_t *ssrc;
			int sx;


			sx = s->xoff[x];
			ssrc = src + sx;
			p0 = *(ssrc);
			if ((sx + 1) < sw)
				p1 = *(ssrc + 1);
			if ((sy + 1) < sh)
			{
				if (sx > -1)
					p2 = *(ssrc + sstride);
				if ((sx + 1) < sw)
					p3 = *(ssrc + sstride + 1);
			}
			p0 = INTERP_256(128, p1, p0);
			p2 = INTERP_256(128, p3, p2);
			p0 = INTERP_256(128, p2, p0);
			*dst = p0;
		}
		else
			*dst = 0;
		x++;
		dst++;
	}
}
#endif

static void _scale_fast(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Surface *s = (Surface *)r;
	uint32_t sstride;
	uint32_t *src;
	Eina_Rectangle ir, dr;

	if (y < s->r.oy || y > s->r.oy + s->w)
	{
		while (len--)
			*dst++ = 0;
		return;
	}

	src = enesim_surface_data_get(s->s);
	sstride = enesim_surface_stride_get(s->s);
	src += sstride * s->yoff[y - s->r.oy];

	while (len--)
	{
		if (x >= s->r.ox && x < s->r.ox + s->w)
			*dst = *(src + s->xoff[x - s->r.ox]);
		else
			*dst = 0;
		x++;
		dst++;
	}
}

static void _noscale(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Surface *s = (Surface *)r;
	uint32_t sstride;
	uint32_t *src;

	if (y < s->r.oy || y > s->r.oy + s->w)
	{
		while (len--)
			*dst++ = 0;
		return;
	}

	src = enesim_surface_data_get(s->s);
	sstride = enesim_surface_stride_get(s->s);
	src += sstride * (y - s->r.oy);
	x -= s->r.ox;

	while (len--)
	{
		if (x >= s->r.ox && x < s->r.ox + s->w)
			*dst = *src;
		else
			*dst = 0;
		x++;
		dst++;
	}
}

static void _state_cleanup(Surface *s)
{
	if (s->xoff)
	{
		free(s->xoff);
		s->xoff = NULL;
	}

	if (s->yoff)
	{
		free(s->yoff);
		s->yoff = NULL;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	int sw, sh;
	Surface *s = (Surface *)r;

	if (s->w < 1 || s->h < 1)
		return EINA_FALSE;

	_state_cleanup(s);
	enesim_surface_size_get(s->s, &sw, &sh);

	if (sw != s->w && sh != s->h)
	{
		s->xoff = malloc(sizeof(int) * s->w);
		s->yoff = malloc(sizeof(int) * s->h);
		_offsets(s->x, s->w, sw, s->xoff);
		_offsets(s->y, s->h, sh, s->yoff);
		r->span = ENESIM_RENDERER_SPAN_DRAW(_scale_fast);
	}
	else
	{
		r->span = ENESIM_RENDERER_SPAN_DRAW(_noscale);
	}
	return EINA_TRUE;
}

static void _free(Surface *s)
{
	_state_cleanup(s);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_surface_new(void)
{
	Enesim_Renderer *r;
	Surface *s;

	s = calloc(1, sizeof(Surface));
	r = (Enesim_Renderer *)s;

	r->free = ENESIM_RENDERER_DELETE(_free);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}

EAPI void enesim_renderer_surface_x_set(Enesim_Renderer *r, int x)
{
	Surface *s = (Surface *)r;

	if (s->x == x)
		return;
	s->x = x;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_surface_y_set(Enesim_Renderer *r, int y)
{
	Surface *s = (Surface *)r;

	if (s->y == y)
		return;
	s->y = y;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_surface_w_set(Enesim_Renderer *r, int w)
{
	Surface *s = (Surface *)r;

	if (s->w == w)
		return;
	s->w = w;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_surface_h_set(Enesim_Renderer *r, int h)
{
	Surface *s = (Surface *)r;

	if (s->h == h)
		return;
	s->h = h;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_surface_src_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Surface *s = (Surface *)r;

	s->s = src;
	r->changed = EINA_TRUE;
}
