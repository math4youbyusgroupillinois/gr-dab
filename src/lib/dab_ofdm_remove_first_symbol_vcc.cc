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

#include <dab_ofdm_remove_first_symbol_vcc.h>
#include <gr_io_signature.h>

/*
 * Create a new instance of dab_ofdm_remove_first_symbol_vcc and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_ofdm_remove_first_symbol_vcc_sptr 
dab_make_ofdm_remove_first_symbol_vcc (unsigned int vlen)
{
  return dab_ofdm_remove_first_symbol_vcc_sptr (new dab_ofdm_remove_first_symbol_vcc (vlen));
}

dab_ofdm_remove_first_symbol_vcc::dab_ofdm_remove_first_symbol_vcc (unsigned int vlen) : 
  gr_block ("ofdm_remove_first_symbol_vcc",
             gr_make_io_signature2 (2, 2, sizeof(gr_complex)*vlen, sizeof(char)),
             gr_make_io_signature2 (2, 2, sizeof(gr_complex)*vlen, sizeof(char))),
  d_vlen(vlen), d_start(0)
{
}

void 
dab_ofdm_remove_first_symbol_vcc::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  //int in_req  = noutput_items; + 1 + noutput_items/76; // at most every 76th symbol is thrown away (depends on the DAB mode)
  int in_req = noutput_items; // altough more may be needed, try to produce output even with only one input - if it's a pilot, we can just consume it ...
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned int i = 0; i < ninputs; i++)
    ninput_items_required[i] = in_req;
}


int 
dab_ofdm_remove_first_symbol_vcc::general_work (int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  const gr_complex *iptr = (const gr_complex *) input_items[0];
  const char *frame_start = (const char *) input_items[1];
  
  gr_complex *optr = (gr_complex *) output_items[0];
  char *o_frame_start = (char *) output_items[1];

  int n_consumed = 0;
  int n_produced = 0;

  for (n_consumed=0; n_consumed<ninput_items[0] && n_consumed<ninput_items[1] && n_produced<noutput_items; n_consumed++) {
    if (*frame_start == 1) {
      d_start = 1;
      iptr += d_vlen;
    } else {
      *o_frame_start++ = d_start;
      n_produced++;
      d_start = 0;
      for (unsigned int j=0; j<d_vlen; j++)
        *optr++ = *iptr++;
    }
    frame_start++;
  }

  // printf("ninput_items[0]: %d, ninput_items[1]: %d, noutput_items: %d, consumed: %d, produced: %d\n", ninput_items[0], ninput_items[1], noutput_items, n_consumed, n_produced);

  consume_each(n_consumed);
  return n_produced;
}
