#include <stdio.h>
#include <stdlib.h>
#include "instrument.h"
#include "key.h"
#include "sine.h"
#include "triangle.h"
#include "rising_saw.h"
#include "falling_saw.h"
#include "square.h"
#include "modulator.h"
#include <time.h>

int main(int argc, char **argv) {
    cs_inst_t inst;
    cs_key_t key;
    cs_sine_t sine;
    cs_sine_t sinefm1;
    cs_sine_t sinefm2;
    cs_modu_t modu;
    cs_triangle_t triangle;
    cs_rsaw_t rsaw;
    cs_fsaw_t fsaw;
    cs_square_t square;
    int r = cs_inst_init(&inst, "inst", 0, NULL);
    if(r != 0) {
	perror("Could not initialize instrument");
	exit(r);
    }
    jack_activate(inst.client);
    r = cs_key_init(&key, "key", 0, NULL);
    if(r != 0) {
	perror("Could not initialize key");
	exit(r);
    }
    cs_key_set_tuning(&key, CS_MAJOR_TUNING, CS_MAJOR_TUNING_SIZE);
    cs_key_set_root(&key, CS_G);
    jack_activate(key.client);
    r = cs_sine_init(&sine, "sine", 0, NULL);
    if(r != 0) {
	perror("Could not initialize sine");
	exit(r);
    }
    jack_activate(sine.client);
    r = cs_sine_init(&sinefm1, "sinefm1", 0, NULL);
    if(r != 0) {
	perror("Could not initialize sinefm1");
	exit(r);
    }
    jack_activate(sinefm1.client);
    r = cs_sine_init(&sinefm2, "sinefm2", 0, NULL);
    if(r != 0) {
	perror("Could not initialize sinefm2");
	exit(r);
    }
    jack_activate(sinefm2.client);
    r = cs_modu_init(&modu, "modu", 0, NULL);
    if(r != 0) {
	perror("Could not initialize modu");
	exit(r);
    }
    jack_activate(modu.client);
    r = cs_triangle_init(&triangle, "triangle", 0, NULL);
    if(r != 0) {
	perror("Could not initialize triangle");
	exit(r);
    }
    jack_activate(triangle.client);
    r = cs_rsaw_init(&rsaw, "rsaw", 0, NULL);
    if(r != 0) {
	perror("Could not initialize rsaw");
	exit(r);
    }
    jack_activate(rsaw.client);
    r = cs_fsaw_init(&fsaw, "fsaw", 0, NULL);
    if(r != 0) {
	perror("Could not initialize fsaw");
	exit(r);
    }
    jack_activate(fsaw.client);
    r = cs_square_init(&square, "square", 0, NULL);
    if(r != 0) {
	perror("Could not initialize square");
	exit(r);
    }
    jack_activate(square.client);
    jack_connect(inst.client, "inst:out", "key:note");
    jack_connect(key.client, "key:freq", "sine:freq");
    jack_connect(key.client, "key:freq", "modu:in1");
    jack_connect(sinefm1.client, "sinefm1:out", "modu:in2");
    jack_connect(modu.client, "modu:out", "sinefm2:freq");
    jack_connect(sinefm2.client, "sinefm2:out", "system:playback_1");
    jack_connect(sinefm2.client, "sinefm2:out", "system:playback_2");
    jack_connect(sine.client, "sine:out", "system:playback_1");
    jack_connect(sine.client, "sine:out", "system:playback_2");
    jack_connect(triangle.client, "triangle:out", "system:playback_1");
    jack_connect(triangle.client, "triangle:out", "system:playback_2");
    jack_connect(rsaw.client, "rsaw:out", "system:playback_1");
    jack_connect(rsaw.client, "rsaw:out", "system:playback_2");
    jack_connect(fsaw.client, "fsaw:out", "system:playback_1");
    jack_connect(fsaw.client, "fsaw:out", "system:playback_2");
    jack_connect(square.client, "square:out", "system:playback_1");
    jack_connect(square.client, "square:out", "system:playback_2");
    r = cs_inst_play(&inst, 4.0f);
    if(r != 0) {
    	perror("Could not stop instrument");
    	exit(r);
    }
    usleep(750000);
    jack_connect(key.client, "key:freq", "triangle:freq");
    jack_disconnect(key.client, "key:freq", "sine:freq");
    r = cs_inst_play(&inst, 2.0f);
    usleep(750000);
    jack_connect(key.client, "key:freq", "rsaw:freq");
    jack_disconnect(key.client, "key:freq", "triangle:freq");
    r = cs_inst_play(&inst, 0.0f);
    usleep(750000);
    jack_connect(key.client, "key:freq", "fsaw:freq");
    jack_disconnect(key.client, "key:freq", "rsaw:freq");
    r = cs_inst_play(&inst, -3.0f);
    usleep(750000);
    jack_connect(key.client, "key:freq", "square:freq");
    jack_disconnect(key.client, "key:freq", "fsaw:freq");
    r = cs_inst_play(&inst, -2);
    usleep(250000);
    jack_connect(key.client, "key:freq", "sinefm1:freq");
    jack_disconnect(key.client, "key:freq", "square:freq");
    r = cs_inst_play(&inst, -1);
    usleep(250000);
    jack_connect(key.client, "key:freq", "sine:freq");
    jack_disconnect(key.client, "key:freq", "sinefm1:freq");
    r = cs_inst_play(&inst, 0);
    usleep(250000);
    jack_connect(key.client, "key:freq", "triangle:freq");
    jack_disconnect(key.client, "key:freq", "sine:freq");
    r = cs_inst_play(&inst, -2);
    usleep(500000);
    jack_connect(key.client, "key:freq", "rsaw:freq");
    jack_disconnect(key.client, "key:freq", "triangle:freq");
    r = cs_inst_play(&inst, 0);
    usleep(250000);
    jack_connect(key.client, "key:freq", "fsaw:freq");
    jack_disconnect(key.client, "key:freq", "rsaw:freq");
    r = cs_inst_play(&inst, -3);
    usleep(1250000);
    r = cs_inst_stop(&inst);
    if(r != 0) {
    	perror("Could not stop instrument");
    	exit(r);
    }
    usleep(250000);
    r = cs_inst_destroy(&inst);
    if(r != 0) {
	perror("Could not destroy instrument");
	exit(r);
    }
    r = cs_key_destroy(&key);
    if(r != 0) {
	perror("Could not destroy key");
	exit(r);
    }
    r = cs_sine_destroy(&sine);
    if(r != 0) {
	perror("Could not destroy sine");
	exit(r);
    }
    r = cs_sine_destroy(&sinefm1);
    if(r != 0) {
	perror("Could not destroy sinefm1");
	exit(r);
    }
    r = cs_sine_destroy(&sinefm2);
    if(r != 0) {
	perror("Could not destroy sinefm2");
	exit(r);
    }
    r = cs_modu_destroy(&modu);
    if(r != 0) {
	perror("Could not destroy modu");
	exit(r);
    }
    r = cs_triangle_destroy(&triangle);
    if(r != 0) {
	perror("Could not destroy triangle");
	exit(r);
    }
    r = cs_rsaw_destroy(&rsaw);
    if(r != 0) {
	perror("Could not destroy rsaw");
	exit(r);
    }
    r = cs_fsaw_destroy(&fsaw);
    if(r != 0) {
	perror("Could not destroy fsaw");
	exit(r);
    }
    r = cs_square_destroy(&square);
    if(r != 0) {
	perror("Could not destroy square");
	exit(r);
    }
    return 0;
}
