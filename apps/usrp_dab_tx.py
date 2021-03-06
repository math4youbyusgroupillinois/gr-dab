#!/usr/bin/env python
# _*_ coding: utf8 _*_

# Andreas Müller, 2008
# andrmuel@ee.ethz.ch
#
# this code may be freely used under GNU GPL conditions

"""
send DAB with USRP
"""

from gnuradio import gr, usrp, blks2
from gnuradio.eng_option import eng_option
import dab
from optparse import OptionParser
import sys

class usrp_dab_tx(gr.top_block):
	def __init__(self):
		gr.top_block.__init__(self)
        
		
		parser = OptionParser(option_class=eng_option, usage="%prog: [options] input-filename")
		parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=(0, 0),
		     help="select USRP Tx side A or B [default=A]")
		parser.add_option("-f", "--freq", type="eng_float", default=227.36e6,
		     help="set frequency to FREQ [default=%default]")
		parser.add_option("-g", "--tx-gain", type="eng_float", default=None,
		     help="set transmit gain in dB (default is midpoint)")
		parser.add_option('-v', '--verbose', action="store_true", default=False,
		     help="verbose output")

        	(options, args) = parser.parse_args ()
		if len(args)!=1:
			parser.print_help()
			sys.exit(1)
		else:
			self.filename = args[0]

		# if gr.enable_realtime_scheduling() != gr.RT_OK:
		#       print "-> failed to enable realtime scheduling"

		interp = 64
		self.dab_params = dab.parameters.dab_parameters(mode=1, sample_rate=2000000, verbose=options.verbose)

		self.src = gr.file_source(gr.sizeof_char, self.filename)
		self.trigsrc = gr.vector_source_b([1]+[0]*(self.dab_params.symbols_per_frame-1),True)
		
		self.s2v = gr.stream_to_vector(gr.sizeof_char, 384)
		
		self.mod = dab.ofdm_mod(self.dab_params, verbose=options.verbose) 

		self.sink = usrp.sink_c(interp_rate = interp)
        	self.sink.set_mux(usrp.determine_tx_mux_value(self.sink, options.tx_subdev_spec))
        	self.subdev = usrp.selected_subdev(self.sink, options.tx_subdev_spec)
		self.sample_rate = self.sink.dac_rate()/interp
		
		print "--> using sample rate: " + str(self.sample_rate)


		self.connect(self.src, self.s2v, self.mod, self.sink)
		self.connect(self.trigsrc, (self.mod,1))

		# tune frequency
		self.sink.tune(0, self.subdev, options.freq)

		# set gain      
		if options.tx_gain is None:
			# if no gain was specified, use the mid-point in dB
			g = self.subdev.gain_range()
			options.tx_gain = float(g[0]+g[1])/2
		self.subdev.set_gain(options.tx_gain)


if __name__=='__main__':
	try:
		usrp_dab_tx().run()
	except KeyboardInterrupt:
		pass



