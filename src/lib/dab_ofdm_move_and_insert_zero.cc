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

#include <dab_ofdm_move_and_insert_zero.h>
#include <gr_io_signature.h>

/*
 * Create a new instance of dab_ofdm_move_and_insert_zero and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_ofdm_move_and_insert_zero_sptr 
dab_make_ofdm_move_and_insert_zero (unsigned int fft_length, unsigned int num_carriers)
{
  return dab_ofdm_move_and_insert_zero_sptr (new dab_ofdm_move_and_insert_zero (fft_length, num_carriers));
}

dab_ofdm_move_and_insert_zero::dab_ofdm_move_and_insert_zero (unsigned int fft_length, unsigned int num_carriers) : 
  gr_sync_block ("ofdm_move_and_insert_zero",
             gr_make_io_signature (1, 1, sizeof(gr_complex)*num_carriers),
             gr_make_io_signature (1, 1, sizeof(gr_complex)*fft_length)),
  d_fft_length(fft_length), d_num_carriers(num_carriers)
{
  d_zeros_on_left = (d_fft_length-d_num_carriers)/2;
}

int 
dab_ofdm_move_and_insert_zero::work (int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  int i;
  unsigned int j,k;
  /* partially adapted from gr_ofdm_frame_acquisition.cc */
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  for (i=0; i<noutput_items; i++) {
    /* zeros on left */
    for (j=0; j<d_zeros_on_left; j++)
      out[j] = 0;
    /* first half of symbols */
    for (k=0; j<d_zeros_on_left+d_num_carriers/2; j++,k++)
      out[j] = in[k];
    /* zentral carrier (zero) */
    out[j++] = 0;
    /* second half of symbols */
    for (; j<d_zeros_on_left+d_num_carriers+1; j++,k++)
      out[j] = in[k];
    /* zeros on rigth */
    for (; j<d_fft_length; j++)
      out[j] = 0;
    in+=d_num_carriers;
    out+=d_fft_length;
    assert(k==d_num_carriers);
  }

  return noutput_items;
}
