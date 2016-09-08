#!/usr/bin/expect -f
set IPADDR [lindex $argv 0]
set CMD [lindex $argv 1]
set timeout -1
spawn ssh -o StrictHostKeyChecking=no -l root $IPADDR
expect "*password:*"
send "chanct-gms123\r"
expect "*#*"
send "$CMD\r"
expect "*#*"
send "exit\r"
interact
