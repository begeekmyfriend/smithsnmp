import unittest
from smithsnmp_testcases import *

class AgentXv3TestCase(unittest.TestCase, SmithSNMPTestFramework, SmithSNMPTestCase):
	def setUp(self):
		self.agentx_setup("config/agentx.conf")
		self.version = "3"
		self.user = "rwAuthUser"
		self.level = "authNoPriv"
		self.auth_protocol = "MD5"
		self.auth_key = "rwAuthUser"
		self.ip = "127.0.0.1"
		self.port = 161
		if self.netsnmp.isalive() == False:
			self.netsnmp.read()
			raise Exception("NetSNMP daemon start error!")
		if self.agentx.isalive() == False:
			self.agentx.read()
			raise Exception("AgentX daemon start error!")

	def tearDown(self):
		if self.netsnmp.isalive() == False:
			self.netsnmp.read()
			raise Exception("NetSNMP daemon start error!")
		if self.agentx.isalive() == False:
			self.agentx.read()
			raise Exception("AgentX daemon start error!")
		self.agentx_teardown()


if __name__ == '__main__':
    unittest.main()
