LUA_PATH="lualib/?/init.lua;lualib/?.lua;./?.lua;$SYS_LUA_PATH" LUA_CPATH="build/?.so" lua5.1 ./bin/smithsnmpd -c config/agentx.conf
