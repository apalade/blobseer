#!/bin/bash 

IP=$1


ssh root@$IP "killall provider"
exit $?
