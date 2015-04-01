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
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--

local core = require "smithsnmp.core"
local mib = require "smithsnmp"
local utils = require "smithsnmp.utils"

local _T = {}
_T.core = core
_T._NAME = "snmptrap"
_T._VERSION = "dev"

-- Generic trap type
_T.SNMP_TRAPS             = "1.3.6.1.6.3.1.1.5"
_T.COLD_START             = _T.SNMP_TRAPS .. ".1"
_T.WARM_START             = _T.SNMP_TRAPS .. ".2"
_T.LINK_DOWN              = _T.SNMP_TRAPS .. ".3"
_T.LINK_UP                = _T.SNMP_TRAPS .. ".4"
_T.AUTHENTICATION_FAILURE = _T.SNMP_TRAPS .. ".5"
_T.EPG_NEIGHBOR_LOSS      = _T.SNMP_TRAPS .. ".6" 
_T.ENTERPRISE_SPECIFIC    = _T.SNMP_TRAPS .. ".7"

-- { oid = {}, object = nil, trigger = function() }
local trap_objects = {}
local trap_object_indexes = {}
-- { community = "public", ip = {127,0,0,1}, port = 162 }
local trap_hosts = {}
local trap_host_indexes = {}

-- Trap host register.
_T.host_register = function(community, ip, port)
    if ip == nil then ip = "127.0.0.1" end
    if port == nil then port = 162 end
    assert(type(community) == 'string' and type(ip) == 'string' and type(port) == 'number')

    if trap_host_indexes[ip] == nil then
        local entry = {}
        entry['community'] = community
        entry['ip'] = utils.str2oid(ip)
        assert(#entry['ip'] == 4, "Trap host: Only IPv4 address supported!")
        entry['port'] = port
        table.insert(trap_hosts, entry)

        -- Host index
        trap_host_indexes[ip] = #trap_hosts
    end
end

-- Trap host unregister.
_T.host_unregister = function(ip)
    local host_idx = trap_host_indexes[ip]
    if trap_hosts[host_idx] ~= nil then
        table.remove(trap_hosts, host_idx)
    end
    trap_host_indexes[ip] = nil
end

-- Trap hosts clear.
_T.host_clear = function()
    trap_hosts = {}
end

-- Trap object register.
_T.object_register = function(oid, object, trigger)
    assert(type(oid) == 'string' and
           type(object.tag) == 'number' and type(object.get_f) == 'function' and
           type(trigger) == 'function')

    if trap_object_indexes[oid] == nil then
        local entry = {}
        entry['oid'] = utils.str2oid(oid)
        entry['variable'] = object
        entry['trigger'] = trigger(object)
        table.insert(trap_objects, entry)

        -- Object index
        trap_object_indexes[oid] = #trap_objects
    end
end

-- Trap object unregister.
_T.object_unregister = function(oid)
    assert(type(oid) == 'string')
    local obj_idx = trap_object_indexes[oid]
    if trap_objects[obj_idx] ~= nil then
        table.remove(trap_objects, obj_idx)
    end
    trap_object_indexes[oid] = nil
end

-- Trap objects clear.
_T.object_clear = function()
    trap_objects = {}
end

-- Trap handler function
local trap_handler = function()
    -- Traverse all hosts
    for _, host in ipairs(trap_hosts) do
        -- Build valbinds
        local oid1, tag1, value1
        local oid2, tag2, value2
        local send = false
        for i, object in ipairs(trap_objects) do
            -- Check trigger
            local trigger = object.trigger
            if type(trigger) == 'function' then
                trigger = trigger()
            end
            -- Make packet
            if trigger == true then
                -- sysUpTime
                if i == 1 then
                    oid1 = object.oid
                    tag1 = object.variable.tag
                    value1 = object.variable.get_f()
                elseif i == 2 then
                    oid2 = object.oid
                    tag2 = object.variable.tag
                    value2 = object.variable.get_f()
                else
                    local oid = object.oid
                    local tag = object.variable.tag
                    local value = object.variable.get_f()
                    if send == false then
                        core.trap_varbind(oid1, tag1, value1)
                        core.trap_varbind(oid2, tag2, value2)
                    end
                    core.trap_varbind(oid, tag, value)
                    send = true
                end
            end
        end

        -- Can be sent
        if send == true then
            -- Only trapv2 supported
            local version = 2
            -- Send Trap
            core.trap_send(version, host.community, host.ip, host.port)
        end
    end
end

-- Enable trap feature
_T.enable = function(poll_interv)
    assert(type(poll_interv) == 'number')
    core.trap_open(poll_interv, trap_handler)
end

-- Disable trap feature
_T.disable = function()
    core.trap_close()
end

return _T
