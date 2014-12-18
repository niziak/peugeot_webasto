#!/bin/sh
DATE=`date "+%Y%m%d_%H%M%S"`
putty -sercfg 8,1,115200,n,N -log ../out/${DATE}_putty.log -serial /dev/ttyUSB0
exit 0

   -sercfg configuration-string
              Specify the configuration parameters for the serial port, in -serial mode. configuration-string should be a comma-separated list of configuration parameters as follows:

              ·      Any single digit from 5 to 9 sets the number of data bits.

              ·      `1', `1.5' or `2' sets the number of stop bits.

              ·      Any other numeric string is interpreted as a baud rate.

              ·      A single lower-case letter specifies the parity: `n' for none, `o' for odd, `e' for even, `m' for mark and `s' for space.

              ·      A single upper-case letter specifies the flow control: `N' for none, `X' for XON/XOFF, `R' for RTS/CTS and `D' for DSR/DTR.

