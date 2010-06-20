#include <stdio.h>
#include <stdlib.h>
#include "cshellsynth.h"
#include <time.h>
#include <math.h>

int main(int argc, char **argv) {
    cs_seq_t seq1;
    cs_key_t key1;
    cs_seq_t seq2;
    cs_key_t key2;
    cs_sine_t sine;
    cs_cot_t cot;
    cs_envg_t envg1;
    cs_modu_t envm1;
    cs_envg_t envg2;
    cs_modu_t envm2;
    cs_lin2exp_t lin2exp1;
    cs_lin2exp_t lin2exp2;
    cs_clock_t clock;
    cs_mix_t mixer;

    int r = cs_key_init(&key1, "key1", 0, NULL);
    r |= cs_key_set_tuning(&key1, CS_MAJOR_TUNING, CS_MAJOR_TUNING_LENGTH);
    r |= cs_key_set_root(&key1, CS_G);
    if(r != 0) {
	perror("Could not initialize key1");
	exit(r);
    }

    r = cs_key_init(&key2, "key2", 0, NULL);
    r |= cs_key_set_tuning(&key2, CS_MAJOR_TUNING, CS_MAJOR_TUNING_LENGTH);
    r |= cs_key_set_root(&key2, CS_G);
    if(r != 0) {
	perror("Could not initialize key2");
	exit(r);
    }

    r = cs_mix_init(&mixer, "mixer", 0, NULL);
    if(r != 0) {
	perror("Could not initialize mixer");
	exit(r);
    }

    r = cs_sine_init(&sine, "sine", 0, NULL);
    if(r != 0) {
	perror("Could not initialize sine");
	exit(r);
    }
    r = cs_cot_init(&cot, "cot", 0, NULL);
    if(r != 0) {
	perror("Could not initialize cot");
	exit(r);
    }

    r = cs_envg_init(&envg1, "envg1", 0, NULL);
    r |= cs_envg_set_attack_t(&envg1, 0.1);
    r |= cs_envg_set_decay_t(&envg1, 0.25);
    r |= cs_envg_set_sustain_a(&envg1, 0.65f);
    r |= cs_envg_set_release_t(&envg1, 0.5);
    if(r != 0) {
	perror("Could not initialize envg1");
	exit(r);
    }
    r = cs_envg_init(&envg2, "envg2", 0, NULL);
    r |= cs_envg_set_attack_t(&envg2, 0.1);
    r |= cs_envg_set_decay_t(&envg2, 0.25);
    r |= cs_envg_set_sustain_a(&envg2, 0.65f);
    r |= cs_envg_set_release_t(&envg2, 0.5);
    if(r != 0) {
	perror("Could not initialize envg2");
	exit(r);
    }

    r = cs_lin2exp_init(&lin2exp1, "lin2exp1", 0, NULL);
    r |= cs_lin2exp_set_zero(&lin2exp1, 0.25);
    if(r != 0) {
	perror("Could not initialize lin2exp1");
	exit(r);
    }
    r = cs_lin2exp_init(&lin2exp2, "lin2exp2", 0, NULL);
    r |= cs_lin2exp_set_zero(&lin2exp2, 0.25);
    if(r != 0) {
	perror("Could not initialize lin2exp2");
	exit(r);
    }
    r = cs_modu_init(&envm1, "envm1", 0, NULL);
    if(r != 0) {
	perror("Could not initialize envm1");
	exit(r);
    }
    r = cs_modu_init(&envm2, "envm2", 0, NULL);
    if(r != 0) {
	perror("Could not initialize envm2");
	exit(r);
    }

    r = cs_seq_init(&seq1, "seq1", 0, NULL);
    if(r != 0) {
	perror("Could not initialize seq1");
	exit(r);
    }

    r = cs_seq_init(&seq2, "seq2", 0, NULL);
    if(r != 0) {
	perror("Could not initialize seq2");
	exit(r);
    }

    r = cs_clock_init(&clock, "clock", 0, NULL);
    r |= cs_clock_set_bpm(&clock, 240.0);
    r |= cs_clock_set_meter(&clock, 3.0);
    if(r != 0) {
	perror("Could not initialize clock");
	exit(r);
    }

    jack_connect(clock.client, "clock:clock", "seq1:clock");
    jack_connect(clock.client, "clock:clock", "seq2:clock");
    jack_connect(seq1.client, "seq1:out", "key1:note");
    jack_connect(seq1.client, "seq1:ctl", "envg1:ctl");
    jack_connect(seq2.client, "seq2:out", "key2:note");
    jack_connect(seq2.client, "seq2:ctl", "envg2:ctl");
    jack_connect(envg1.client, "envg1:out", "lin2exp1:in");
    jack_connect(lin2exp1.client, "lin2exp1:out", "envm1:in2");
    jack_connect(envg2.client, "envg2:out", "lin2exp2:in");
    jack_connect(lin2exp2.client, "lin2exp2:out", "envm2:in2");

    jack_connect(key1.client, "key1:freq", "sine:freq");
    jack_connect(sine.client, "sine:out", "envm1:in1");

    jack_connect(key2.client, "key2:freq", "cot:freq");
    jack_connect(cot.client, "cot:out", "envm2:in1");

    jack_connect(envm1.client, "envm1:out", "mixer:in1");
    jack_connect(envm2.client, "envm2:out", "mixer:in2");

    jack_connect(mixer.client, "mixer:out", "system:playback_1");
    jack_connect(mixer.client, "mixer:out", "system:playback_2");

    jack_default_audio_sample_t first_verse[10][3] = {
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
    jack_default_audio_sample_t second_verse[10][3] = {
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

    jack_default_audio_sample_t **first_verse_p = alloca(11 * sizeof(jack_default_audio_sample_t *));
    first_verse_p[10] = NULL;
    int i;
    for(i = 0; i < 10; i++) {
	first_verse_p[i] = alloca(3*sizeof(jack_default_audio_sample_t));
	int j;
	for(j = 0; j < 3; j++) {
	    first_verse_p[i][j] = first_verse[i][j];
	}
    }

    jack_default_audio_sample_t **second_verse_p = alloca(11 * sizeof(jack_default_audio_sample_t *));
    second_verse_p[10] = NULL;
    for(i = 0; i < 10; i++) {
	second_verse_p[i] = alloca(3*sizeof(jack_default_audio_sample_t));
	int j;
	for(j = 0; j < 3; j++) {
	    second_verse_p[i][j] = second_verse[i][j];
	}
    }

    r = cs_seq_sequence_once(&seq1, 0.0f, 21.0f, first_verse_p);
    r |= cs_seq_sequence_once(&seq2, 0.0f, 45.0f, second_verse_p);
    if(r != 0) {
    	perror("Could not create sequence");
    	exit(r);
    }

    printf("Hit return to quit");
    getchar();

    r = cs_key_destroy(&key1);
    if(r != 0) {
	perror("Could not destroy key1");
	exit(r);
    }
    r = cs_key_destroy(&key2);
    if(r != 0) {
	perror("Could not destroy key2");
	exit(r);
    }
    r = cs_sine_destroy(&sine);
    if(r != 0) {
	perror("Could not destroy sine");
	exit(r);
    }
    r = cs_cot_destroy(&cot);
    if(r != 0) {
	perror("Could not destroy cot");
	exit(r);
    }

    r = cs_envg_destroy(&envg1);
    if(r != 0) {
	perror("Could not destroy envg1");
	exit(r);
    }
    r = cs_lin2exp_destroy(&lin2exp1);
    if(r != 0) {
	perror("Could not destroy lin2exp1");
	exit(r);
    }
    r = cs_modu_destroy(&envm1);
    if(r != 0) {
	perror("Could not destroy envm1");
	exit(r);
    }
    r = cs_envg_destroy(&envg2);
    if(r != 0) {
	perror("Could not destroy envg2");
	exit(r);
    }
    r = cs_lin2exp_destroy(&lin2exp2);
    if(r != 0) {
	perror("Could not destroy lin2exp2");
	exit(r);
    }
    r = cs_modu_destroy(&envm2);
    if(r != 0) {
	perror("Could not destroy envm2");
	exit(r);
    }

    r = cs_seq_destroy(&seq1);
    if(r != 0) {
	perror("Could not destroy seq1");
	exit(r);
    }

    r = cs_seq_destroy(&seq2);
    if(r != 0) {
	perror("Could not destroy seq2");
	exit(r);
    }

    r = cs_clock_destroy(&clock);
    if(r != 0) {
	perror("Could not destroy clock");
	exit(r);
    }
    r = cs_mix_destroy(&mixer);
    if(r != 0) {
	perror("Could not destroy mixer");
	exit(r);
    }
    return 0;
}
