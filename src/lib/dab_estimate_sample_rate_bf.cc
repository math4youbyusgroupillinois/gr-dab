/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * config.h is generated by configure.  It contains the results
 * of probing for features, options etc.  It should be the first
 * file included in your .cc file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dab_estimate_sample_rate_bf.h>
#include <gr_io_signature.h>

/*
 * Create a new instance of dab_estimate_sample_rate_bf and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_estimate_sample_rate_bf_sptr 
dab_make_estimate_sample_rate_bf (float expected_sample_rate, int frame_length)
{
  return dab_estimate_sample_rate_bf_sptr (new dab_estimate_sample_rate_bf (expected_sample_rate, frame_length));
}

/*
 * Specify constraints on number of input and output streams.
 * This info is used to construct the input and output signatures
 * (2nd & 3rd args to gr_block's constructor).  The input and
 * output signatures are used by the runtime system to
 * check that a valid number and type of inputs and outputs
 * are connected to this block.  In this case, we accept
 * only 1 input and 1 output.
 */
static const int MIN_IN = 1;  // mininum number of input streams
static const int MAX_IN = 1;  // maximum number of input streams
static const int MIN_OUT = 1;  // minimum number of output streams
static const int MAX_OUT = 1;  // maximum number of output streams

/*
 * The private constructor
 */
dab_estimate_sample_rate_bf::dab_estimate_sample_rate_bf (float expected_sample_rate, int frame_length)
  : gr_sync_block ("estimate_sample_rate_bf",
       gr_make_io_signature (MIN_IN, MAX_IN, sizeof (char)),
       gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (float))),
    d_zeros(0), d_expected_sample_rate(expected_sample_rate), d_real_sample_rate(expected_sample_rate), d_found_first_frame(0), d_frame_length(frame_length)
{
}

/*
 * Our virtual destructor.
 */
dab_estimate_sample_rate_bf::~dab_estimate_sample_rate_bf ()
{
  // nothing else required in this example
}

int 
dab_estimate_sample_rate_bf::work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items)
{
  const char *in = (const char *) input_items[0];
  float *out = (float *) output_items[0];

  int i=0;

  if (d_found_first_frame) {
    for(i=0; i<noutput_items; i++) {
      d_zeros++;
      if (*in == 1) {
        d_real_sample_rate = (float)d_zeros / (float)d_frame_length * d_expected_sample_rate;
        printf("estimated sample rate: %f\n", d_real_sample_rate);
        d_zeros = 0;
      }
      in++;
      *out++ = d_real_sample_rate;
    } 
  } else {
    while (i<noutput_items && d_found_first_frame==0) {
      i++;
      if (*in == 1)
        d_found_first_frame = 1;
      in++;
      *out++ = d_real_sample_rate;
    }
  }

  // Tell runtime system how many output items we produced.
  return i;
}
