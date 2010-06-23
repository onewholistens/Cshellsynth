#include <jack/jack.h>
#include <math.h>
#include "cshellsynth/mixer.h"
#include "cshellsynth/jclient.h"
#include "atomic-float.h"

void cs_mix_set_in1(cs_mix_t *self, float in1) {
    atomic_float_set(&self->in1, in1);
}

void cs_mix_set_in2(cs_mix_t *self, float in2) {
    atomic_float_set(&self->in2, in2);
}

static int cs_mix_process(jack_nframes_t nframes, void *arg) {
    cs_mix_t *self = (cs_mix_t *) arg;
    float *in1_buffer;
    float *in2_buffer;
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
    int i;
    for(i = 0; i < nframes; i++) {
	out_buffer[i] = (isnanf(in1) ? in1_buffer[i] : in1) + (isnanf(in2) ? in2_buffer[i] : in2);
    }
    return 0;
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
