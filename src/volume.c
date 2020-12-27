/*-
 * Copyright (C) Dmitry Kosenkov 2011 <junker@front.ru>
 * Copyright (c) 2020 Rozhuk Ivan <rozhuk.im@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include "gvolwheel.h"


int vol_backend_init(const char *device, void **ctx);
void vol_backend_destroy(void *ctx);
void vol_backend_set(void *ctx, uint32_t channel, uint32_t value);
uint32_t vol_backend_get(void *ctx, uint32_t channel);


int
vol_init(gvw_p gvw)
{
	return (vol_backend_init(gvw->device, &gvw->sound_backend_ctx));
}

void
vol_up(gvw_p gvw)
{
	vol_set(gvw, (vol_get(gvw) + gvw->s.incr));
}

void
vol_down(gvw_p gvw)
{
	uint32_t cur_vol = vol_get(gvw);

	if (cur_vol < gvw->s.incr) {
		vol_set(gvw, 0);
	} else {
		vol_set(gvw, (uint32_t)(cur_vol - gvw->s.incr));
	}
}

void
vol_mute_toggle(gvw_p gvw)
{
	uint32_t cur_vol = vol_get(gvw);

	if (cur_vol) { /* Alvays mute sound if unmuted. */
		gvw->sound_mute_val = 0;
	}
	vol_set(gvw, gvw->sound_mute_val);
	gvw->sound_mute_val = cur_vol;
}

void
vol_set(gvw_p gvw, uint32_t value)
{
	if (value > 100) {
		value = 100;
	}
	vol_backend_set(gvw->sound_backend_ctx, gvw->s.channel, value);
	tray_icon_update(gvw);
}

uint32_t
vol_get(gvw_p gvw)
{
	return (vol_backend_get(gvw->sound_backend_ctx, gvw->s.channel));
}
