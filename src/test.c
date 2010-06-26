#include <stdio.h>
#include <stdlib.h>
#include "cshellsynth.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>

#define destroy_func(x, t)			\
    void destroy_ ## x() {			\
	int r = cs_ ## t ## _destroy(&x);	\
	if(r != 0) {				\
	    perror("Could not destroy " #x);	\
	}					\
    }
cs_seq_t seq1;
destroy_func(seq1, seq);
cs_key_t key1;
destroy_func(key1, key);
cs_seq_t seq2;
destroy_func(seq2, seq);
cs_key_t key2;
destroy_func(key2, key);
cs_sine_t sine;
destroy_func(sine, sine);
cs_cot_t cot;
destroy_func(cot, cot);
cs_envg_t envg1;
destroy_func(envg1, envg);
cs_modu_t envm1;
destroy_func(envm1, modu);
cs_envg_t envg2;
destroy_func(envg2, envg);
cs_modu_t envm2;
destroy_func(envm2, modu);
cs_lin2exp_t lin2exp1;
destroy_func(lin2exp1, lin2exp);
cs_lin2exp_t lin2exp2;
destroy_func(lin2exp2, lin2exp);
cs_clock_t clock1;
destroy_func(clock1, clock);
cs_mix_t mixer;
destroy_func(mixer, mix);
cs_bandpass_t bandpass;
destroy_func(bandpass, bandpass);
cs_modu_t lp_modu;
destroy_func(lp_modu, modu);
cs_modu_t lp_modu_scale;
destroy_func(lp_modu_scale, modu);
cs_distort_t distortion1;
destroy_func(distortion1, distort);
cs_distort_t distortion2;
destroy_func(distortion2, distort);

int main(int argc, char **argv) {
    int r;

#define init_and_check(x, t)					\
    r = cs_ ## t ## _init(&x, #x, 0, NULL);			\
    if(r != 0) {						\
	perror("could not initialize " #x);			\
	exit(r);						\
    }								\
    atexit(destroy_ ## x);

    init_and_check(bandpass, bandpass);
    init_and_check(lp_modu, modu);
    init_and_check(lp_modu_scale, modu);
    cs_modu_set_in2(&lp_modu_scale, 50.0);

    init_and_check(distortion1, distort);
    cs_distort_set_gain(&distortion1, 0.9);
    cs_distort_set_sharpness(&distortion1, 4.0);

    init_and_check(distortion2, distort);
    cs_distort_set_gain(&distortion2, 0.9);
    cs_distort_set_sharpness(&distortion2, 4.0);

    init_and_check(key1, key);
    cs_key_set_tuning(&key1, CS_MAJOR_TUNING, CS_MAJOR_TUNING_LENGTH);
    cs_key_set_root(&key1, CS_G);

    init_and_check(key2, key);
    cs_key_set_tuning(&key2, CS_MAJOR_TUNING, CS_MAJOR_TUNING_LENGTH);
    cs_key_set_root(&key2, CS_G);

    init_and_check(mixer, mix);

    init_and_check(sine, sine);
    init_and_check(cot, cot);

    init_and_check(envg1, envg);
    cs_envg_set_attack_t(&envg1, 0.5);
    cs_envg_set_decay_t(&envg1, 0.25);
    cs_envg_set_sustain_a(&envg1, 0.65f);
    cs_envg_set_release_t(&envg1, 0.25);
    init_and_check(envg2, envg);
    cs_envg_set_attack_t(&envg2, 0.25);
    cs_envg_set_decay_t(&envg2, 0.5);
    cs_envg_set_sustain_a(&envg2, 0.05f);
    cs_envg_set_release_t(&envg2, 0.5);

    init_and_check(lin2exp1, lin2exp);
    cs_lin2exp_set_zero(&lin2exp1, 0.0625);
    init_and_check(lin2exp2, lin2exp);
    cs_lin2exp_set_zero(&lin2exp2, 0.0625);
    init_and_check(envm1, modu);
    init_and_check(envm2, modu);

    init_and_check(seq1, seq);

    init_and_check(seq2, seq);

    init_and_check(clock1, clock);
    cs_clock_set_bpm(&clock1, 240.0);
    cs_clock_set_meter(&clock1, 3.0);

    jack_connect(clock1.client, "clock1:clock", "seq1:clock");
    jack_connect(clock1.client, "clock1:clock", "seq2:clock");
    jack_connect(seq1.client, "seq1:out", "key1:note");
    jack_connect(seq1.client, "seq1:ctl", "envg1:ctl");
    jack_connect(seq2.client, "seq2:out", "key2:note");
    jack_connect(seq2.client, "seq2:ctl", "envg2:ctl");
    jack_connect(envg1.client, "envg1:out", "lin2exp1:in");
    jack_connect(lin2exp1.client, "lin2exp1:out", "envm1:in2");
    jack_connect(envg2.client, "envg2:out", "lin2exp2:in");
    jack_connect(lin2exp2.client, "lin2exp2:out", "envm2:in2");
    jack_connect(lin2exp2.client, "lin2exp2:out", "lp_modu_scale:in1");
    jack_connect(lp_modu_scale.client, "lp_modu_scale:out", "lp_modu:in2");

    jack_connect(key1.client, "key1:freq", "sine:freq");
    jack_connect(sine.client, "sine:out", "envm1:in1");
    jack_connect(envm1.client, "envm1:out", "distortion1:in");

    jack_connect(key2.client, "key2:freq", "cot:freq");
    jack_connect(key2.client, "key2:freq", "lp_modu:in1");
    jack_connect(lp_modu.client, "lp_modu:out", "bandpass:freq");
    jack_connect(cot.client, "cot:out", /* "envm2:in1"); */
    /* jack_connect(envm2.client, "envm2:out", */ "bandpass:in");
    jack_connect(bandpass.client, "bandpass:out", "distortion2:in");

    jack_connect(distortion1.client, "distortion1:out", "mixer:in1");
    jack_connect(distortion2.client, "distortion2:out", "mixer:in2");

    jack_connect(mixer.client, "mixer:out", "system:playback_1");
    jack_connect(mixer.client, "mixer:out", "system:playback_2");

    float first_verse[10][3] = {
	{0.0f, 2.75f, 4.0f},
	{3.0f, 5.75f, 2.0f},
	{6.0f, 8.75f, 0.0f},
	{9.0f, 11.75f,-3.0f},
	{12.0f, 12.95f, -2.0f},
	{13.0f, 13.95f, -1.0f},
	{14.0f, 14.95f, 0.0f},
	{15.0f, 16.95f, -2.0f},
	{17.0f, 17.95f, 0.0f},
	{18.0f, 20.75f, -3.0f},
    };
    float second_verse[10][3] = {
	{24.0f + 0.0f, 24.0f + 2.75f, 1.0f},
	{24.0f + 3.0f, 24.0f + 5.75f, 4.0f},
	{24.0f + 6.0f, 24.0f + 8.75f, 2.0f},
	{24.0f + 9.0f, 24.0f + 11.75f, 0.0f},
	{24.0f + 12.0f, 24.0f + 12.95f, -2.0},
	{24.0f + 13.0f, 24.0f + 13.95f, -1.0},
	{24.0f + 14.0f, 24.0f + 14.95f, 0.0f},
	{24.0f + 15.0f, 24.0f + 16.95f, 1.0f},
	{24.0f + 17.0f, 24.0f + 17.95f, 2.0f},
	{24.0f + 18.0f, 24.0f + 20.75f, 1.0f}
    };

    float **first_verse_p = alloca(11 * sizeof(float *));
    first_verse_p[10] = NULL;
    int i;
    for(i = 0; i < 10; i++) {
	first_verse_p[i] = alloca(3*sizeof(float));
	int j;
	for(j = 0; j < 3; j++) {
	    first_verse_p[i][j] = first_verse[i][j];
	}
    }

    float **second_verse_p = alloca(11 * sizeof(float *));
    second_verse_p[10] = NULL;
    for(i = 0; i < 10; i++) {
	second_verse_p[i] = alloca(3*sizeof(float));
	int j;
	for(j = 0; j < 3; j++) {
	    second_verse_p[i][j] = second_verse[i][j];
	}
    }

    cs_seq_sequence_once(&seq1, 0.0f, 21.0f, first_verse_p);
    cs_seq_sequence_once(&seq2, 0.0f, 45.0f, second_verse_p);

    printf("Hit return to quit");
    getchar();

    return 0;
}
