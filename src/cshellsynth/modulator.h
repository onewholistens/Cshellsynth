/** @file modulator.h
 *
 * Modulator
 *
 * Ruby version: @c Modulator
 *
 * Multiplies its inputs together.
 */
/*
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
#ifndef CSHELLSYNTH_MODULATOR_H
#define CSHELLSYNTH_MODULATOR_H 1

#include <jack/jack.h>
#include <cshellsynth/atomic-types.h>
#include <cshellsynth/mixer.h>

/**
 * Modulator
 *
 * Ruby version: @c Modulator
 *
 * See @ref cs_mix_t
 */
typedef struct cs_modu_struct {
    jack_client_t *client;
    jack_port_t *in1_port;
    atomic_float_t in1;
    jack_port_t *in2_port;
    atomic_float_t in2;
    jack_port_t *out_port;
    atomic_float_t in1_amp;
    atomic_float_t in2_amp;
} cs_modu_t;

/**
 * Destroy modulator
 *
 * See @ref cs_mix_destroy
 */
#define cs_modu_destroy(cs_modu) cs_mix_destroy((cs_mix_t *) (cs_modu))

/**
 * Initialize modulator
 *
 * See @ref cs_mix_init
 */
int cs_modu_init(cs_modu_t *self, const char *client_name, jack_options_t flags, char *server_name);

/**
 * @ref cs_mix_set_in1
 */
#define cs_modu_set_in1(self, in1) cs_mix_set_in1((cs_mix_t *) (self), in1);

/**
 * @ref cs_mix_set_in2
 */
#define cs_modu_set_in2(self, in2) cs_mix_set_in2((cs_mix_t *) (self), in2);

/**
 * @ref cs_mix_set_in1_amp
 */
#define cs_modu_set_in1_amp(self, in1_amp) cs_mix_set_in1((cs_mix_t *) (self), in1_amp);

/**
 * @ref cs_mix_set_in2_amp
 */
#define cs_modu_set_in2_amp(self, in2_amp) cs_mix_set_in2((cs_mix_t *) (self), in2_amp);

#endif
