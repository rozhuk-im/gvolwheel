/*-
 * Copyright (C) 2012 - Dmitry Kosenkov
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

#include <sys/types.h>
#include <inttypes.h>
#include <alsa/asoundlib.h>


static const char *default_device = "default";
static const char *channel_map[2] = {
	"Master",
	"PCM"
};

static snd_mixer_t *mixer_id = NULL;
static snd_mixer_selem_id_t *sid = NULL;


int
vol_backend_init(const char *device, void **ctx)
{
	snd_mixer_open(&mixer_id, 0);
	snd_mixer_attach(mixer_id, (device ? device : default_device));
	snd_mixer_selem_register(mixer_id, NULL, NULL);
	snd_mixer_load(mixer_id);

	snd_mixer_selem_id_malloc(&sid);

	snd_mixer_selem_id_set_name(sid, "Master");
	snd_mixer_elem_t* elem = snd_mixer_find_selem(mixer_id, sid);
	if (!elem)
		return (-1);
	snd_mixer_selem_id_set_name(sid, "PCM");
	elem = snd_mixer_find_selem(mixer_id, sid);
	if (!elem)
		return (-1);
	return (0);
}

void
vol_backend_destroy(void *ctx)
{	
}

uint32_t
vol_backend_get(void *ctx, const uint32_t channel)
{
	snd_mixer_handle_events(mixer_id);
	snd_mixer_selem_id_set_name(sid, channel_map[channel]);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(mixer_id, sid);

	long min, max, vol;
	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_get_playback_volume(elem, 0, &vol);

	return (100 * vol / max);
}

void
vol_backend_set(void *ctx, const uint32_t channel, uint32_t value)
{
	snd_mixer_selem_id_set_name(sid, channel_map[channel]);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(mixer_id, sid);

	long min, max;
	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_set_playback_volume_all(elem, max * value / 100);
}
