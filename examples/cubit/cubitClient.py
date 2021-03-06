# $Id: cubitClient.py,v 1.5 1999/08/03 01:57:52 janssen Exp $
# BeginILUCopyright

# Copyright (c) 1991-1999 Xerox Corporation.  All Rights Reserved.

# Unlimited use, reproduction, modification, and distribution of this
# software and modified versions thereof is permitted.  Permission is
# granted to make derivative works from this software or a modified
# version thereof.  Any copy of this software, a modified version
# thereof, or a derivative work must include both the above copyright
# notice of Xerox Corporation and this paragraph.  Any distribution of
# this software, a modified version thereof, or a derivative work must
# comply with all applicable United States export control laws.  This
# software is made available AS IS, and XEROX CORPORATION DISCLAIMS ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
# LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
# EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
# NEGLIGENCE) OR STRICT LIABILITY, EVEN IF XEROX CORPORATION IS ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGES.

# EndILUCopyright

import sys
import ilu
import cubit
import traceback

def trunc (val, limit):
	if (val < 0): sign, val = -1, -val
	else: sign = 1
	return sign * (val % limit)

def do_tests (handle):

	# cube byte

	try:
		val = trunc(13, 0x100)
		retval = handle.cube_octet(val)
		print "cube octet:  %d --> %d" % (val, retval)
	except:
		print "exception from cube_octet(%s)" % val
		traceback.print_exc()

	# cube short

	try:
		val = trunc(-117, 0x10000)
		retval = handle.cube_short(val)
		print "cube short:  %d --> %d" % (val, retval)
	except:
		print "exception from cube_short(%s)" % val
		traceback.print_exc()

	# cube long

	try:
		val = trunc(-117, 0x100000000L)
		retval = handle.cube_long(val)
		print "cube long:  %d --> %d" % (val, retval)
	except:
		print "exception from cube_long(%s):" % val
		traceback.print_exc()

	# cube struct

	try:
		origval = {'o' : trunc(13, 0x100), 's' : trunc(-117, 0x10000), 'l' : trunc(-117, 0x100000000L) }
		retval = handle.cube_struct(origval)
		print "cube struct:  %s --> %s" % (origval, retval)
	except:
		print "exception from cube_struct(%s):" % origval
		traceback.print_exc()

def main(sbh):

	handle = ilu.ObjectOfSBH (cubit.Cubit, sbh)
	if not handle:
		print "Can't parse URL", sbh
		sys.exit(2)
	do_tests (handle)

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "Usage:  %s OBJECT-SBH" % sys.argv[0]
		sys.exit(1)
	else:
		main (sys.argv[1])

