(**
$Id: BlobExample.isl,v 1.3 1999/08/03 01:57:59 janssen Exp $
 BeginILUCopyright

 Copyright (c) 1991-1999 Xerox Corporation.  All Rights Reserved.

 Unlimited use, reproduction, modification, and distribution of this
 software and modified versions thereof is permitted.  Permission is
 granted to make derivative works from this software or a modified
 version thereof.  Any copy of this software, a modified version
 thereof, or a derivative work must include both the above copyright
 notice of Xerox Corporation and this paragraph.  Any distribution of
 this software, a modified version thereof, or a derivative work must
 comply with all applicable United States export control laws.  This
 software is made available AS IS, and XEROX CORPORATION DISCLAIMS ALL
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
 LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
 EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
 NEGLIGENCE) OR STRICT LIABILITY, EVEN IF XEROX CORPORATION IS ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGES.

 EndILUCopyright
*)
(* Last edited by Mike Spreitzer October 8, 1998 10:31 pm PDT *)

INTERFACE BlobExample;

TYPE coord = SHORT INTEGER;

TYPE blobid = OBJECT
	METHODS
		ReqMove (x : coord, y : coord),
		ReqGrab (bv : blobvis) : BOOLEAN,
		ReqRelease (bv : blobvis)
	END;

TYPE blobvis = OBJECT
	METHODS
		Move(x : coord, y : coord),
		Grab (),
		Release ()
	END;

TYPE blobcli = OBJECT
	METHODS
		NewBlobvis (x : coord,
			    y : coord,
			    n : blobid) : blobvis,
		RemoveBlobvis(n : blobid)
	END;

TYPE blobserv = OBJECT
	METHODS
		RegisterClient(c : blobcli),
		RemoveClient(c : blobcli),
		NewBlobId(x : coord, y : coord),
		RemoveBlobId(n : blobid)
	END;
