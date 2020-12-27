/*-
 * Copyright (C) 2012 - Dmitry Kosenkov
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


#include <sys/param.h>
#include <sys/types.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

static const char *default_device = "/dev/mixer";

static const uint32_t channel_map[2] = {
	SOUND_MIXER_VOLUME,
	SOUND_MIXER_PCM
};


int
vol_backend_init(const char *device, void **ctx)
{
	(*ctx) = (device ? device : default_device);

	return (0);
}

void
vol_backend_destroy(void *ctx)
{	
}

uint32_t
vol_backend_get(void *ctx, const uint32_t channel)
{
	int fd, vol;

	memset(&vol, 0x00, sizeof(vol));
	fd = open(ctx, O_RDWR, 0);
	ioctl(fd, MIXER_READ(channel_map[channel]), &vol);
	close(fd);

	return (vol & 0x7f);
}

void
vol_backend_set(void *ctx, const uint32_t channel, uint32_t value)
{
	int fd, vol;

	vol = (int)(value | (value << 8));
	fd = open(ctx, O_RDWR, 0);
	ioctl(fd, MIXER_WRITE(channel_map[channel]), &vol);
	close(fd);
}
