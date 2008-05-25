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

#include <dab_qpsk_mapper_vbc.h>
#include <gr_io_signature.h>

/*
 * Create a new instance of dab_qpsk_mapper_vbc and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_qpsk_mapper_vbc_sptr 
dab_make_qpsk_mapper_vbc (int symbol_length)
{
  return dab_qpsk_mapper_vbc_sptr (new dab_qpsk_mapper_vbc (symbol_length));
}

dab_qpsk_mapper_vbc::dab_qpsk_mapper_vbc (int symbol_length) : 
  gr_sync_block ("qpsk_mapper_vbc",
             gr_make_io_signature (1, 1, sizeof(char)*symbol_length/4),
             gr_make_io_signature (1, 1, sizeof(gr_complex)*symbol_length)),
  d_symbol_length(symbol_length)
{
  assert(symbol_length>0);
  assert(symbol_length%4==0);
}


int 
dab_qpsk_mapper_vbc::work (int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  char const *in = (const char *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  gr_complex symbol;
  char byte;

  for (int i=0; i<noutput_items; i++) {
    for (int j=0; j<d_symbol_length/4; j++) {
      byte = in[j];
      out[0] = gr_complex((byte&128)>0?-I_SQRT2:I_SQRT2,(byte&64)>0?-I_SQRT2:I_SQRT2);
      out[1] = gr_complex((byte&32)>0?-I_SQRT2:I_SQRT2, (byte&16)>0?-I_SQRT2:I_SQRT2);
      out[2] = gr_complex((byte&8)>0?-I_SQRT2:I_SQRT2,  (byte&4)>0?-I_SQRT2:I_SQRT2);
      out[3] = gr_complex((byte&2)>0?-I_SQRT2:I_SQRT2,  (byte&1)>0?-I_SQRT2:I_SQRT2);
      out+=4;
    }
    in  += d_symbol_length/4;
  }
 
  return noutput_items;
}
