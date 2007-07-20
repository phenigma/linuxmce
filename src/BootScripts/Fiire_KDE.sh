#!/bin/bash

#<-mkr_b_via_b->
awk 'NR == 1 { print "0"; next; } {print}' /boot/grub/default >/boot/grub/default.new
mv /boot/grub/default{.new,}
reboot
#<-mkr_b_via_e->
