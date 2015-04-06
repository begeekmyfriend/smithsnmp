How To Write Traps For SmithSNMP
================================

This document is a quick tutorial for MIB users to define their own traps in
SmithSNMP. It is hoped that you will learn the flexible features that SmithSNMP
provides as well as perceptual knowledge of Lua programming when you finish it.

Problems To Be Solved
---------------------

The trap feature derives from the feedback of an MIB user who threw me three
requirements he wants.

- Does SmithSNMP provide user-defined traps or notifications?

- Does SmithSNMP support enabling and disabling of traps or notifications?

- Does SmithSNMP implement activation of traps or notifications?

Further MIB development as follows is according to these three requirements.

Enabling And Disabling Traps
----------------------------

Let us take `mibs/snmptrap.lua` as an example:

    local snmpTrapPollInterv = 42
    local trapGroup = {
        [snmpTrapPollInterv] = mib.Timeticks(function () return trap_poll_interv end,
                                             function (v) trap_poll_set(v) return end),
    }

Variable "snmpTrapPollInterv" in "trapGroup" represents the time interval of
trap polling. By the way, the unit of a tick equals to 10 milliseconds. When the
interval is set to zero, all traps are disabled; When it is 100, that means all
traps will be polled once per second. So the clients can enable and disable all
traps through two SNMP commands as follows after the agent starts.

    snmpset -v2c -cprivate localhost .1.3.6.1.6.3.1.1.4.42.0 t 100  # enable traps
    snmpset -v2c -cprivate localhost .1.3.6.1.6.3.1.1.4.42.0 t 0    # disable traps

Those two commands will not bring system overhead in the agent. And when it is
permitted, SNMPv3 commands also work.

Furthermore, the user can set the trigger time interval for each specific trap
by setting its ratio to "snmpTrapPollInterv". For instance, when the status of
a variable is to be polled once per 1.5 second, you may set "snmpTrapPollInterv"
to 50 while in the trigger method of the variable one polling should be allowed
per three times. The trigger method will be shown in the following chapter.

User-defined Traps
------------------

In fact, any MIB development based on SmithSNMP starts by defining new MIB nodes
and so does trap development. Therefore, you may call it MIB-Oriented
programming (aka. MOP :-). It is assumed that you have read the artical -- "How
To Writ MIB Group For SmithSNMP" before, so we will begin from an instance in
point.

    local mib = require "smithsnmp"
    local trap = require "smithsnmp.trap"
    
    local alarm1 = 1
    local alarm2 = 2
    
    local alarmGroup = {
        [alarm1] = mib.ConstTimeticks(function() return os.difftime(os.time(), load_time) * 100 end),
        [alarm2] = mib.ConstTimeticks(function() return os.difftime(os.time(), load_time) * 100 end),
    }
    
    -- First step: register generic trap oid
    mib.module_methods.trap_generic_reg(trap.ENTERPRISE_SPECIFIC)
    
    -- Second step: register trap host address
    trap.host_register("public")
    
    -- Third step: register objects as SNMP trap varbind(s).
    trap.object_register(".1.3.6.1.4.1.2333.1.1.0", alarm_object1, level_trigger)
    trap.object_register(".1.3.6.1.4.1.2333.1.2.0", alarm_object2, edge_trigger)

We have defined two "alarm" variables representing the time ticks elapsed since
the loading of the MIB module by the agent. And the register of the alarm trap
will be finished through three steps.

Step One: Indicating the generic type of the trap. There are seven generic types
of traps defined in SNMP trap RFC including cold-start, warm-start, link-down,
link-up and so on. "trap.ENTERPRISE_SPECIFIC" represents the user-defined trap.
It is typically necessary before all trap variables to be registered.

Step Two: Registering communities and sockets of the clients. Typically the
clients will run their daemons at 162 port so called trapd. When the agent
raises a trap, the trapd will receive the trap datagram and parse it and maybe
do some handles like logging. So the communities and sockets of the trapd are
needed. In the example above, the community is "public" and the socket is not
given but the default values are "127.0.0.1" and 162 instead. The former is a
string and the latter is a number.

Step Three: Registering the trap object. This includes OID, the object and the
trigger method.

After these three steps, start the agent, enable the traps, and the agent will
automatically probe the traps periodically and then pack and send them to the
clients when their trigger conditions are met.

Activating Traps
----------------

Traps activation refers to user-defined trigger methods. We have shown two kinds
of trigger methods -- level trigger and edge trigger.

    -- 10 milliseconds per tick
    local alarm_time1 = 1000 -- 10 seconds
    local alarm_time2 = 1500 -- 15 seconds
    
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

The trigger method is a Lua function. We have used closure for dynamics for it
usually is a better option than callback is if you want something to be done in
the future. When you register your trigger, the agent will get the anonymous
function inside it which can get access to the variables defined out of the
range of it (e.x. the arguments of the trigger method). It is so called non-
local variable which is different from global variable.

Now let us take a look at the level trigger method. It is simple and just
returns a boolean condition judgement. It will be continually invoked during the
later traps polling. And then comes the edge trigger method, it just activates
the trap when the object status jumps. It will not continue to activate the trap
until the next jump comes.

Below is the register of two trap objects and the logging from the trapd. It
shows since 10 seconds after the agent starts, "alarm1" has been logged once per
second while "alarm2" just logged once at the 15th second.

    # sudo ./tests/net-snmp-release/sbin/snmptrapd -f -Lo -m "" -C -c tests/snmptrapd.conf
    2015-04-01 11:48:16 localhost [UDP: [127.0.0.1]:41670->[127.0.0.1]:162]:
    iso.3.6.1.2.1.1.3.0 = Timeticks: (1000) 0:00:10.00  iso.3.6.1.6.3.1.1.4.1.0 = OID: iso.3.6.1.6.3.1.1.5.7
    iso.3.6.1.4.1.2.3.3.3.1.1.0 = Timeticks: (1000) 0:00:10.00
    
    2015-04-01 11:48:17 localhost [UDP: [127.0.0.1]:41670->[127.0.0.1]:162]:
    iso.3.6.1.2.1.1.3.0 = Timeticks: (1100) 0:00:11.00  iso.3.6.1.6.3.1.1.4.1.0 = OID: iso.3.6.1.6.3.1.1.5.7
    iso.3.6.1.4.1.2.3.3.3.1.1.0 = Timeticks: (1100) 0:00:11.00
    
    2015-04-01 11:48:18 localhost [UDP: [127.0.0.1]:41670->[127.0.0.1]:162]:
    iso.3.6.1.2.1.1.3.0 = Timeticks: (1200) 0:00:12.00  iso.3.6.1.6.3.1.1.4.1.0 = OID: iso.3.6.1.6.3.1.1.5.7
    iso.3.6.1.4.1.2.3.3.3.1.1.0 = Timeticks: (1200) 0:00:12.00
    
    ......
    
    2015-04-01 11:48:21 localhost [UDP: [127.0.0.1]:41670->[127.0.0.1]:162]:
    iso.3.6.1.2.1.1.3.0 = Timeticks: (1500) 0:00:15.00  iso.3.6.1.6.3.1.1.4.1.0 = OID: iso.3.6.1.6.3.1.1.5.7
    iso.3.6.1.4.1.2.3.3.3.1.1.0 = Timeticks: (1500) 0:00:15.00  iso.3.6.1.4.1.2.3.3.3.1.2.0 = Timeticks: (1500) 0:00:15.00
    
    2015-04-01 11:48:22 localhost [UDP: [127.0.0.1]:41670->[127.0.0.1]:162]:
    iso.3.6.1.2.1.1.3.0 = Timeticks: (1600) 0:00:16.00  iso.3.6.1.6.3.1.1.4.1.0 = OID: iso.3.6.1.6.3.1.1.5.7
    iso.3.6.1.4.1.2.3.3.3.1.1.0 = Timeticks: (1600) 0:00:16.00
    
    2015-04-01 11:48:23 localhost [UDP: [127.0.0.1]:41670->[127.0.0.1]:162]:
    iso.3.6.1.2.1.1.3.0 = Timeticks: (1700) 0:00:17.00  iso.3.6.1.6.3.1.1.4.1.0 = OID: iso.3.6.1.6.3.1.1.5.7
    iso.3.6.1.4.1.2.3.3.3.1.1.0 = Timeticks: (1700) 0:00:17.00

Note: The configuration of trapd is not involved in this tutorial. You may refer
to the official manual at Net-SNMP web site.

More Details Control
--------------------

If you want to control the activation of the single trap object, please refer to
the MIB-Oriented Programming paradigm I have told you before:-) What you need is
just to add a new MIB node and do something in its get/set method.

    local alarm1 = 1
    local switch1 = 3
    local switch1_state = 0
    
    local alarmGroup
    alarmGroup = {
        [alarm1] = mib.ConstTimeticks(function() return os.difftime(os.time(), load_time) * 100 end),
        [switch1] = mib.Int(function() return switch1_state end,
                            function(v)
                                switch1_state = v
                                if v ~= 0 then
                                    trap.object_register(".1.3.6.1.4.1.2333.1.1.0", alarmGroup[alarm1], level_trigger)
                                else
                                    trap.object_unregister(".1.3.6.1.4.1.2333.1.1.0")
                            end
                            end),
    }

Now you may activate "alarm1" trap by setting "switch1". By the way, since the
internal set method of "swithc1" refers to "alarmGroup" which is not yet
instantiated, so we need to use lazy evaluation sementic feature of Lua to solve
it. In the internal set method of "switch", we have binded "alarmGroup" during
its declaration regardless its real value. The evaluation will not be executed
until the MIB module to be loaded and the set method to be invoked. That is the
benefit from the Functional Programming paradigm.
