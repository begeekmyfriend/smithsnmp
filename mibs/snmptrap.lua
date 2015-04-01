-- 
-- This file is part of SmithSNMP
-- Copyright (C) 2014, Credo Semiconductor Inc.
-- Copyright (C) 2015, Leo Ma <begeekmyfriend@gmail.com>
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FTrap A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
-- 
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-- 

local mib = require "smithsnmp"
local trap = require "smithsnmp.trap"

-- scalar index
local snmpTrapOID        = 1
local snmpTrapEnterprise = 3
local snmpTrapPollInterv = 42

-- Trap OID
local trap_generic_oid = {}
local trap_specific_oid = {}

-- Trap group
local trapGroup = {}

local trap_poll_interv = 0
local trap_poll_set = function(v)
    assert(type(v) == 'number')
    if v ~= 0 then
        trap.disable()
        trap_poll_interv = v
        trap.enable(v)
    else
        trap.disable()
    end
end

local trapGroup = {
    [snmpTrapOID]        = mib.ConstOid(function () return trap_generic_oid end),
    [snmpTrapEnterprise] = mib.ConstOid(function () return trap_specific_oid end),
    [snmpTrapPollInterv] = mib.Timeticks(function () return trap_poll_interv end,
                                         function (v) trap_poll_set(v) return end),
}

-- register generic trap oid
local trap_generic_reg = function (oid)
    assert(type(oid) == 'string')
    trap_generic_oid = {}
    for i in string.gmatch(oid, "%d") do
        table.insert(trap_generic_oid, tonumber(i))
    end
    -- Register snmpTrapOID as a default SNMP trap varbind.
    trap.object_register(".1.3.6.1.6.3.1.1.4.1.0", trapGroup[snmpTrapOID], function() return true end)
end

-- unregister generic trap oid
local trap_generic_unreg = function (oid)
    assert(type(oid) == 'string')
    trap_generic_oid = {}
    -- Unregister snmpTrapOID SNMP trap varbind.
    trap.object_unregister(".1.3.6.1.6.3.1.1.4.1.0")
end

-- register specific trap oid
local trap_specific_reg = function (oid)
    assert(type(oid) == 'string')
    trap_specific_oid = {}
    for i in string.gmatch(oid, "%d") do
        table.insert(trap_specific_oid, tonumber(i))
    end
    -- Register snmpTrapEnterprise as a default SNMP trap varbind.
    trap.object_register(".1.3.6.1.6.3.1.1.4.3.0", trapGroup[snmpTrapEnterprise], function() return true end)
end

-- unregister specific trap oid
local trap_specific_unreg = function (oid)
    assert(type(oid) == 'string')
    trap_specific_oid = {}
    -- Unregister snmpTrapEnterprise SNMP trap varbind.
    trap.object_unregister(".1.3.6.1.6.3.1.1.4.3.0")
end

local snmpTrapMethods = {
    ["trap_generic_reg"] = trap_generic_reg,
    ["trap_generic_unreg"] = trap_generic_unreg,
    ["trap_specific_reg"] = trap_specific_reg,
    ["trap_specific_unreg"] = trap_specific_unreg,
}

mib.module_method_register(snmpTrapMethods)

return trapGroup
