import unittest
from smithsnmp_testcases import *

class SNMPv3TestCase(unittest.TestCase, SmithSNMPTestFramework, SmithSNMPTestCase):
	def setUp(self):
		self.snmp_setup("config/snmp.conf")
		self.version = "3"
		self.user = "rwAuthPrivUser"
		self.level = "authPriv"
		self.auth_protocol = "MD5"
		self.auth_key = "rwAuthPrivUser"
		self.priv_protocol = "AES"
		self.priv_key = "rwAuthPrivUser"
		self.ip = "127.0.0.1"
		self.port = 161
		if self.snmp.isalive() == False:
			self.snmp.read()
			raise Exception("SNMP daemon start error!")

	def tearDown(self):
		if self.snmp.isalive() == False:
			self.snmp.read()
			raise Exception("SNMP daemon start error!")
		self.snmp_teardown()

if __name__ == '__main__':
    unittest.main()
