How To Write MIB Groups For SmithSNMP
=====================================

This document is a simple tutorial on how to write mib groups in SmithSNMP. You
will learn to build your own management information bases in Lua instead of a
pile of APIs. Let us go when you get ready!

Configuration
-------------

First we would tell you on how to write the configuration file. Now we have two
examples shown in `config` directroy indicating some fields as protocol name,
port number and mib module path.

    protocol = 'snmp'
    port = 161
    mib_module_path = 'mibs'

Good! Next you can register your mib modules which are shown as Lua files under
"mib_module_path". And they will be loaded at the start of the agent. As we all
know, Lua has a very useful data structure called Table which contains a pair of
key and value. Now we provide a Lua table called "mib_modules" to contain the
group oid as the key and the group name as the value.

    mib_modules = {
        [".1.3.6.1.2.1.1"] = 'system',
        [".1.3.6.1.2.1.2"] = 'interfaces',
        [".1.3.6.1.2.1.4"] = 'ip',
        [".1.3.6.1.2.1.6"] = 'tcp',
        [".1.3.6.1.2.1.7"] = 'udp',
        ...
    }

A pair of group oid and name represents a mib module. The group oid is the
prefix of the whole oid of an object and will be registered in core module just
after the start of the agent. And the group name can be whatever as you like.

**Note:** In AgentX mode the group oid should NOT conflict with those in master
agent, otherwise the access control of the master will keep those mib groups in
sub-agent from registry.

Very good! As for access control, Different SNMP versions have different access
methods. For example, SNMPv2c is based on community string and SNMPv3 is on user
authentication. You may regard it as a passport in which there are not only a
name but also an access domain called "view". An mib view is also a Lua table
containing an oid as the key and the read-write permission as the value. The oid
key indicates the nodes you can access under that oid cover. Especially "."
means all nodes registered are available. The permission have two value, 'ro'
means read-only and 'rw' means read-write. You can specify permissions for
different mib views. There two users called Jack and Rose, Jack can write in Rose
read-only mib view and vice versa.

    communities = {
        { community = 'public', views = { ["."] = 'ro' } },
        { community = 'private', views = { ["."] = 'rw' } }
    }

    users = {
        {
            user = 'Jack',
            views = {
                        [".1.3.6.1.2.1.1"] = 'ro',
                        [".1.3.6.1.2.1.4"] = 'rw',
                    }
        },
        {
            user = 'Rose',
            views = {
                        [".1.3.6.1.2.1.4"] = 'ro',
                        [".1.3.6.1.2.1.1"] = 'rw',
                    }
        }
    }

Excellent! Here are the whole configuration you need to know.

MIB Examples
------------

The real challenge begins! Just build your own MIB right now. MIB group examples
are shown in `mibs` directory. It is noted that each group should be the bottom
node in the mib tree. Nested groups are not allowed in our examples. A group is
represented by a Lua table (hope you still remember this omnipotent data
structure) and will be returned by the mib file when loaded. Now please build
the "sysGroup".

    local sysGroup = {
        ...
    }

    return sysGroup

Tips: You would better always use "local" key word to decorate "sysGroup" in
case of polluting the Lua global environment.

In the group container, we can define four objects as mentioned in SNMP RFC:
Scalar, Table, Entry and Variable. SmithSNMP also provides relevant constructor
methods for each object.

Variable Constructor
--------------------

The constructor methods of each group variable are defined in `init.lua`. The
signature of each constructor indicates the ASN.1 tags and the access
permissions of each mib variable. The arguments of the constructor are get/set
methods of the data defined by MIB users since function is the first-class value
in Lua. Before we start to write it, we shall require the `init.lua` to get
access to these methods.

Let us take `system.lua` as an example. In this file we have defined a Lua table
called "sysGroup" representing the system mib group. In this group, we need to
constructure a scalar object named "sysDesc" which can show the full name and
version identification of the system hardware type, software OS and networking.
So we can write the variable constructor like this.

    local mib = require "smithsnmp"
    local sysDesc = 1
    local sysGroup = {
        [sysDesc] = mib.ConstString(function() reuturn mib.sh_call("uname -a", "*line") end),
        ...
    }
    return sysGroup

The "mib" is the reference of the constructor methods defined in `init.lua`. The
"sysDesc" is the group table indice as a scalar object id. "mib.ConstString"
shows that the variable is read-only and string type. And the get method which
returns a new method called "mib.sh_call" provided by "mib" as required before.
When the get method invoked, Lua VM will execute a shell command and return a
string value. We do not need to write a set method because the scalar object is
read-only.

Table and Entry
---------------

Next we will implement 'sysORTable' and 'sysOREntry'. In SmithSNMP Table and
Entry are also represented by Lua tables. However, the get/set methods are
somewhat different from those in scalar object because Table and Entry are
sequence objects which you can regard as columns of a two-dimensional table.

    local function or_entry_get(i, name)
        assert(type(name) == 'string')
        local value
        if or_entry_cache[i] then
            if name == 'uptime' then
                value = os.difftime(os.time(), or_entry_cache[i][name]) * 100
            else
                value = or_entry_cache[i][name]
            end
        end
        return value
    end

    local sysGroup = {
        [sysORTable] = {
            [sysOREntry]  = {
                indexes = or_entry_cache,
                [sysORIndex]  = mib.ConstInt(function() return nil, mib.SNMP_ERR_STAT_UNACCESS end),
                [sysORID]     = mib.ConstOid(function(i) return or_entry_get(i, 'oid') end),
                [sysORDesc]   = mib.ConstString(function(i) return or_entry_get(i, 'desc') end),
                [sysORUpTime] = mib.ConstTimeticks(function(i) return or_entry_get(i, 'uptime') end),
            }
        }
    }

**Note:** One table object can hold only one entry object. If you want one more
entry objects, define another table object to contain it.

In "sysORTable" we have defined "or_entry_cache" recording the ORTable
information. It is referred to by the "indexes" field so as to tell SmithSNMP
the lexicographical traversal order of "sysORTable" by generating a temporal
indexes table in `init.lua`.

If you do not want some variables to be shown in the query, define their get
methods to return [value, err_stat] pair so as to indicate that the variable is
unaccessible (invisible in SNMP response). The "err_stat" can be a dummy value
as defaut which will be ignored in return. That can be regarded as a further
access control method.

Variables stored in "or_entry_cache" can load data from configuration files or
other non-RAM places. The "or_entry_cache" is comprised of several rows, each of
them corresponds to the lexicographical sorted variable in "sysOREntry".

    local row = {
        oid = { 1, 3, 6, 1, 2, 1, 1 },
        desc = "The MIB module for managing IP and ICMP inplementations",
        uptime = os.time(),
    }
    table.insert(or_entry_cache, row)

Above all are the MIB object constructors. There is no need worring about APIs.
Hurrah!

Data Indexes
------------

Now let us proceed to more advanced features! There are three styles of variable
indexes referred to by the "indexes" field in entry objects: single index, long
index, and cascaded index.

**Single index** has only one oid and it increments according to the natural
number when a new row is inserted.

**Long index** is comprised of multiple oid numbers such as an IP address (maybe
following a port number). In "udpTable" we examplify that with two entry objects
and their indexes will be concatenated in "udp_entry_cache" referred to by the
"indexes" field. By the way, you would better check if the type of "sub_oid"
argument is a Lua table when it is passed down.

    local udp_entry_cache = {
        ["0.0.0.0.67"] = {},
        ["0.0.0.0.68"] = {},
        ["0.0.0.0.161"] = {},
        ["0.0.0.0.5353"] = {},
        ["0.0.0.0.44681"] = {},
        ["0.0.0.0.51586"] = {},
        ["127.0.0.1.53"] = {},
        ["192.168.122.1.53"] = {},
    }

    [udpTable] = {
        [1] = {
            indexes = udp_entry_cache,
            [1] = mib.ConstIpaddr(function (sub_oid)
                                     local ipaddr
                                     if type(sub_oid) == 'table' and udp_entry_cache[table.concat(sub_oid, ".")] ~= nil then
                                         ipaddr = {}
                                         for i = 1, 4 do
                                             table.insert(ipaddr, sub_oid[i])
                                         end
                                     end
                                     return ipaddr
                                  end),
            [2] = mib.ConstInt(function (sub_oid)
                                   local port
                                   if type(sub_oid) == 'table' and udp_entry_cache[table.concat(sub_oid, ".")] ~= nil then
                                       port = sub_oid[5]
                                   end
                                   return port
                               end),
        }
    }

SmithSNMP also supports **cascaded index** in Table and Entry. "TwoIndexTable"
and "ThreeIndexTable" group respectively show the two-index-cascaded and
three-index-cascaded indexes. The entry cache that "indexes" field refers to is
written as below, note we need to use a bool indicator called "cascaded".

    local two_dim_entry_cache = {
        cascade = true,
        { 1, 2 },
        { 2, 3 },
    }

    [TwoIndexEntry] = {
        indexes = two_dim_entry_cache,
        ...
    }

    local three_dim_entry_cache = {
        cascade = true,
        { 1, 3, 5, 7},
        { 2, 3 },
        { 32, 123, 87 },
    }

    [ThreeIndexEntry] = {
        indexes = three_dim_entry_cache,
        ...
    }

OR Table Register
-----------------

A new registered mib group can be stored as a record in "sysORTable" (if there
are any) by calling "mib.module_methods.or_table_reg" with arguments of a string
of group oid and a piece of description:

    mib.module_methods.or_table_reg("1.3.6.1.2.1.4", "The MIB module for managing IP and ICMP inplementations")

Then the registry of the new group will be shown as three fields of "sysORID",
"sysORDesc" and "sysORUpTime" stored in "sysORTable" and shown in SNMP query
response later.

Debugging
---------

Here coming the end of the whole work! After you finish your own mibs, you may
urge to know whether they can be walked in lexicographical order correctly. Eh,
just add "mib.group_index_table_check" method in the place before the group to
be returned in the mib file and then pray to God...

    local udpGroup = {
        ...
    }
    mib.group_index_table_check(udpGroup, 'udpGroup')
    return udpGroup

This method will print all the indexes of the mib group on the terminal and help
you locate the invalid indexes that you have mistaken. When it echoes "OK", then
congratulations! You have made them all and just go home as the first guy in
your office!
