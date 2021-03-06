# $Id: webServer.py,v 1.3 1999/08/03 01:58:35 janssen Exp $
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

import Bank, ilu, Bank__skel, http, http__skel, rand

mainloopvar = ilu.CreateLoopHandle()

class realAccount (Bank__skel.Account):

	def __init__(self, manager, name, balance):
		self.manager = manager
		self.IluServer = manager.IluServer
		self.IluInstHandle = manager.IluInstHandle + ' - ' + name
		self.name = name
		self.bank_balance = balance

	def balance (self):
		return self.bank_balance

class realAccountManager (Bank__skel.AccountManager):

	def __init__(self, name, srvr=None):
		self.IluInstHandle = name
		self.IluServer = srvr
		self.accounts = {}

	def open (self, name):
		if not self.accounts.has_key(name):
			initial_balance = rand.rand() / 32.768
			account = realAccount (self, name, initial_balance)
			self.accounts[name] = account
		else:
			account = self.accounts[name]
		return account

class bankDoc (http__skel.Resource):

	prefix = '<h1>Client Applet</h1><hr><center><applet code=BankAppletVisi.class width=200 height=80><h2>This would be the VisiBroker for Java Bank Applet, but you are not running a Java enabled browser...</h2><PARAM NAME=IOR VALUE="'
	suffix = '"></applet></center><hr>'

	def __init__(self, name, srvr, ior):
		self.IluInstHandle = name
		self.IluServer = srvr
		self.IOR = ior

	def GET(self, request):
		text = self.prefix + self.IOR + self.suffix
		return http.Response(http.StatusCode.OK,
				     [http.Header('Content-Type', 'text/html'),],
				     text);

	def HEAD(self, request):
		return http.Response(http.StatusCode.OK, [], None)

	def POST(self, request):
		return http.Response(http.StatusCode.OK, [], None)

def main():

  instHandle = "Post-Modern Bank"
  serverID = "bankserver.somedept.somecompany.com"
  webServerID = "bankAppletServer"

  s = ilu.CreateServer(serverID, ("tcp_0_0",), "iiop_1_0_1");
  uc = realAccountManager(instHandle, s)

  wsrvr = ilu.CreateServer(webServerID, ("tcp_0_8777",), "http_1_0p");
  wobj = bankDoc("/bankApplet", wsrvr, ilu.IOROfObject(uc));
  print wobj.IluSBH()

  ilu.RunMainLoop(mainloopvar)

if __name__ == '__main__':
	main()
