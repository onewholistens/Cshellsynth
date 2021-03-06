/*
 * sampler.c
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
#include <ruby.h>
#include <math.h>
#include <cshellsynth/sampler.h>
#include "jackruby.h"

static VALUE cCSSampler;

static VALUE rbcs_sampler_ctl(VALUE self) {
    cs_sampler_t *cself;
    Data_Get_Struct(self, cs_sampler_t, cself);
    VALUE ctl_port = rb_iv_get(self, "@ctl_port");
    if(NIL_P(ctl_port)) {
	ctl_port = Data_Wrap_Struct(cJackPort, 0, fake_free, cself->ctl_port);
	rb_iv_set(self, "@ctl_port", ctl_port);
    }
    return ctl_port;
}

static VALUE rbcs_sampler_set_ctl(VALUE self, VALUE ctl) {
    VALUE ctl_port = rb_iv_get(self, "@ctl_port");
    if(NIL_P(ctl_port)) {
	cs_sampler_t *cself;
	Data_Get_Struct(self, cs_sampler_t, cself);
	ctl_port = Data_Wrap_Struct(cJackPort, 0, fake_free, cself->ctl_port);
	rb_iv_set(self, "@ctl_port", ctl_port);
    }
    jr_client_connect(self, ctl, ctl_port);
    // ignore return value
    return ctl;
}

static VALUE rbcs_sampler_load(VALUE self, VALUE path) {
    cs_sampler_t *cself;
    Data_Get_Struct(self, cs_sampler_t, cself);
    int r = cs_sampler_load(cself, StringValueCStr(path));
    if(r != 0) {
	rb_raise(eJackFailure, "Overall operation failed: %d", r);
    }
    return path;
}

static VALUE rbcs_sampler_outL(VALUE self) {
    VALUE outL_port = rb_iv_get(self, "@outL_port");
    if(NIL_P(outL_port)) {
	cs_sampler_t *cself;
	Data_Get_Struct(self, cs_sampler_t, cself);
	outL_port = Data_Wrap_Struct(cJackPort, 0, fake_free, cself->outL_port);
	rb_iv_set(self, "@outL_port", outL_port);
    }
    return outL_port;
}

static VALUE rbcs_sampler_set_outL(VALUE self, VALUE outL) {
    cs_sampler_t *cself;
    Data_Get_Struct(self, cs_sampler_t, cself);
    VALUE outL_port = rb_iv_get(self, "@outL_port");
    if(NIL_P(outL_port)) {
	outL_port = Data_Wrap_Struct(cJackPort, 0, fake_free, cself->outL_port);
	rb_iv_set(self, "@outL_port", outL_port);
    }
    jr_client_connect(self, outL_port, outL);
    // ignore return value
    return outL;
}

static VALUE rbcs_sampler_outR(VALUE self) {
    VALUE outR_port = rb_iv_get(self, "@outR_port");
    if(NIL_P(outR_port)) {
	cs_sampler_t *cself;
	Data_Get_Struct(self, cs_sampler_t, cself);
	outR_port = Data_Wrap_Struct(cJackPort, 0, fake_free, cself->outR_port);
	rb_iv_set(self, "@outR_port", outR_port);
    }
    return outR_port;
}

static VALUE rbcs_sampler_set_outR(VALUE self, VALUE outR) {
    cs_sampler_t *cself;
    Data_Get_Struct(self, cs_sampler_t, cself);
    VALUE outR_port = rb_iv_get(self, "@outR_port");
    if(NIL_P(outR_port)) {
	outR_port = Data_Wrap_Struct(cJackPort, 0, fake_free, cself->outR_port);
	rb_iv_set(self, "@outR_port", outR_port);
    }
    jr_client_connect(self, outR_port, outR);
    // ignore return value
    return outR;
}

static void cs_sampler_free(void *mem) {
    cs_sampler_t *cself = (cs_sampler_t *) mem;
    cs_sampler_destroy(cself);
    xfree(cself);
}

static VALUE rbcs_sampler_new(int argc, VALUE *argv, VALUE klass) {
    VALUE rname;
    char *name = "sampler";
    if(rb_scan_args(argc, argv, "01", &rname)) {
	name = StringValueCStr(rname);
    }
    cs_sampler_t *cself = ALLOC(cs_sampler_t);
    int r = cs_sampler_init(cself, name, 0, NULL);
    JR_CHECK_INIT_ERROR(cself, r);
    return Data_Wrap_Struct(klass, 0, cs_sampler_free, cself);
}

void Init_sampler() {
    cCSSampler = rb_define_class("LLSampler", cJackClient);

    rb_define_singleton_method(cCSSampler, "new", rbcs_sampler_new, -1);
    rb_define_method(cCSSampler, "load", rbcs_sampler_load, 1);
    rb_define_method(cCSSampler, "ctl", rbcs_sampler_ctl, 0);
    rb_define_method(cCSSampler, "ctl=", rbcs_sampler_set_ctl, 1);
    rb_define_method(cCSSampler, "outL", rbcs_sampler_outL, 0);
    rb_define_method(cCSSampler, "outL=", rbcs_sampler_set_outL, 1);
    rb_define_method(cCSSampler, "outR", rbcs_sampler_outR, 0);
    rb_define_method(cCSSampler, "outR=", rbcs_sampler_set_outR, 1);
}
