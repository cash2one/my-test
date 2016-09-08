#!/usr/bin/expect -f
set IPADDR [lindex $argv 0]
set CMD [lindex $argv 1]
set timeout -1
spawn ssh -o StrictHostKeyChecking=no -l admin $IPADDR
expect "*password:*"
send "admin\r"
expect "*>*"
send "acl uninstall\r"
expect "*>*"
send "$CMD\r"
expect "*>*"
send "acl install\r"
expect "*>*"
send "save configuration\r"
expect "*>*"
send "exit\r"
interact
