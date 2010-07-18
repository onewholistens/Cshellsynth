/*
 * mixer.c
 * 
 * Copyright 2010 Evan Buswell
 * 
 * This file is part of Cshellsynth.
 * 
 * Cshellsynth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Cshellsynth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Cshellsynth.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <jack/jack.h>
#include <math.h>
#include "cshellsynth/mixer.h"
#include "cshellsynth/jclient.h"
#include "atomic-float.h"

static int cs_mix_process(jack_nframes_t nframes, void *arg) {
    cs_mix_t *self = (cs_mix_t *) arg;
    float *in1_buffer = in1_buffer; /* suppress uninitialized warning */
    float *in2_buffer = in2_buffer; /* suppress uninitialized warning */
    float *out_buffer = (float *)jack_port_get_buffer(self->out_port, nframes);
    if(out_buffer == NULL) {
	return -1;
    }
    float in1 = atomic_float_read(&self->in1);
    if(isnanf(in1)) {
	in1_buffer = (float *)jack_port_get_buffer(self->in1_port, nframes);
	if(in1_buffer == NULL) {
	    return -1;
	}
    }
    float in2 = atomic_float_read(&self->in2);
    if(isnanf(in2)) {
	in2_buffer = (float *)jack_port_get_buffer(self->in2_port, nframes);
	if(in2_buffer == NULL) {
	    return -1;
	}
    }
    float in1_amp = atomic_float_read(&self->in1_amp);
    float in2_amp = atomic_float_read(&self->in2_amp);
    jack_nframes_t i;
    for(i = 0; i < nframes; i++) {
	out_buffer[i] = (isnanf(in1) ? in1_buffer[i] : in1) * in1_amp + (isnanf(in2) ? in2_buffer[i] : in2) * in2_amp;
    }
    return 0;
}

void cs_mix_set_in1(cs_mix_t *self, float in1) {
    atomic_float_set(&self->in1, in1);
}

void cs_mix_set_in1_amp(cs_mix_t *self, float in1_amp) {
    atomic_float_set(&self->in1_amp, in1_amp);
}

void cs_mix_set_in2(cs_mix_t *self, float in2) {
    atomic_float_set(&self->in2, in2);
}

void cs_mix_set_in2_amp(cs_mix_t *self, float in2_amp) {
    atomic_float_set(&self->in2_amp, in2_amp);
}

int cs_mix_subclass_init(cs_mix_t *self, const char *client_name, jack_options_t flags, char *server_name) {
    int r = jclient_init((jclient_t *) self, client_name, flags, server_name);
    if(r != 0) {
	return r;
    }

    self->in1_port = jack_port_register(self->client, "in1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    if(self->in1_port == NULL) {
	jclient_destroy((jclient_t *) self);
	return -1;
    }

    self->in2_port = jack_port_register(self->client, "in2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    if(self->in2_port == NULL) {
	jclient_destroy((jclient_t *) self);
	return -1;
    }

    self->out_port = jack_port_register(self->client, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    if(self->out_port == NULL) {
	jclient_destroy((jclient_t *) self);
	return -1;
    }

    atomic_float_set(&self->in1, NAN);
    atomic_float_set(&self->in2, NAN);
    atomic_float_set(&self->in1_amp, 1.0);
    atomic_float_set(&self->in2_amp, 1.0);

    return 0;
}

int cs_mix_init(cs_mix_t *self, const char *client_name, jack_options_t flags, char *server_name) {
    int r = cs_mix_subclass_init(self, client_name, flags, server_name);
    if(r != 0) {
	return r;
    }

    r = jack_set_process_callback(self->client, cs_mix_process, self);
    if(r != 0) {
	jclient_destroy((jclient_t *) self);
	return r;
    }

    r = jack_activate(self->client);
    if(r != 0) {
	jclient_destroy((jclient_t *) self);
	return r;
    }

    return 0;
}
