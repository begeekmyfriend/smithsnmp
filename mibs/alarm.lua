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

-- 10 milliseconds per tick
local alarm_time1 = 1000 -- 10 seconds
local alarm_time2 = 1500 -- 15 seconds

local load_time = os.time()

local level_trigger = function(variable)
                          return function() return variable.get_f() >= alarm_time1 end
                      end

local edge_trigger = function(variable)
                         local last = 0
                         return function()
                             local now = variable.get_f()
                             local trigger = last < alarm_time2 and now >= alarm_time2
                             last = now
                             return trigger
                         end
                     end

local alarm1  = 1
local alarm2  = 2
local switch1 = 3
local switch2 = 4
local switch1_state = 0
local switch2_state = 0

local alarmGroup
alarmGroup = {
    [alarm1] = mib.ConstTimeticks(function() return os.difftime(os.time(), load_time) * 100 end),
    [alarm2] = mib.ConstTimeticks(function() return os.difftime(os.time(), load_time) * 100 end),
    [switch1] = mib.Int(function() return switch1_state end,
                        function(v)
                            switch1_state = v
                            if v ~= 0 then
                                trap.object_register(".1.3.6.1.4.1.2333.1.1.0", alarmGroup[alarm1], level_trigger)
                            else
                                trap.object_unregister(".1.3.6.1.4.1.2333.1.1.0")
                            end
                        end),
    [switch2] = mib.Int(function() return switch2_state end,
                        function(v)
                            switch2_state = v
                            if v ~= 0 then
                                trap.object_register(".1.3.6.1.4.1.2333.1.2.0", alarmGroup[alarm2], edge_trigger)
                            else
                                trap.object_unregister(".1.3.6.1.4.1.2333.1.2.0")
                            end
                        end),
}

-- First: register generic trap oid
mib.module_methods.trap_generic_reg(trap.ENTERPRISE_SPECIFIC)

-- Second: register trap host address
trap.host_register("public")

-- Third: register objects as SNMP trap varbind(s).
trap.object_register(".1.3.6.1.4.1.2333.1.1.0", alarmGroup[alarm1], level_trigger)
trap.object_register(".1.3.6.1.4.1.2333.1.2.0", alarmGroup[alarm2], edge_trigger)

return alarmGroup
