#!/usr/bin/perl

exit;

$BAND=5120;	#Maximum bandwidth measured in kbit
$DEVICE="eth0"; #Device on which we are doing shaping
$PAck=60;	#Percentage for minimum bandwidth 4 small datagrams
$PVoIP=50;	#Percentage for minimum bandwidth 4 VoIP
$PHTTP=20;	#Percentage for minimum bandwidth 4 HTTP
$PSSH=20;	#Percentage for minimum bandwidth 4 SSH
$PRadio=5;	#Percentage for minimum bandwidth 4 Radio
$PFTP=5;	#percentage for minimum bandwidth 4 FTP

#solving percentage 2 bandwidth
$BAck = ($BAND/100)*$PAck;
$BVoIP = ($BAND / 100) * $PVoIP;
$BHTTP = ($BAND / 100) * $PHTTP;
$BSSH = ($BAND / 100) * $PSSH;
$BRadio = ($BAND / 100) * $PRadio;
$BFTP = ($BAND / 100) * $PFTP;
#Done

print "-> Shaping\n";
print "--> Clearing shaper\n";
  $cmd="tc qdisc del dev $DEVICE root";
  system($cmd);
print "--> Building Shaper\n";
  $cmd="tc qdisc add dev $DEVICE root handle 1: htb default 70";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1: classid 1:1 htb rate $BAND"."kbit";
  system($cmd);
                        
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:10 htb rate $BAck"."kbit ceil $BAND"."kbit prio 1";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:20 htb rate $BVoIP"."kbit ceil $BAND"."kbit prio 2";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:30 htb rate $BHTTP"."kbit ceil $BAND"."kbit prio 3";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:40 htb rate $BSSH"."kbit ceil $BAND"."kbit prio 4";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:50 htb rate $BRadio"."kbit ceil $BAND"."kbit prio 5";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:60 htb rate $BFTP"."kbit ceil $BAND"."kbit prio 6";
  system($cmd);
  $cmd="tc class add dev $DEVICE parent 1:1 classid 1:70 htb rate 10kbit ceil $BAND"."kbit prio 7";
  system($cmd);
print "--> Securing shaper\n";
  $cmd="tc qdisc add dev $DEVICE parent 1:10 handle 10: sfq perturb 10";
  system($cmd);
  $cmd="tc qdisc add dev $DEVICE parent 1:20 handle 20: sfq perturb 10";
  system($cmd);
  $cmd="tc qdisc add dev $DEVICE parent 1:30 handle 30: sfq perturb 10";
  system($cmd);
  $cmd="tc qdisc add dev $DEVICE parent 1:40 handle 40: sfq perturb 10";
  system($cmd);
  $cmd="tc qdisc add dev $DEVICE parent 1:50 handle 50: sfq perturb 10";
  system($cmd);
  $cmd="tc qdisc add dev $DEVICE parent 1:60 handle 60: sfq perturb 10";
  system($cmd);
  $cmd="tc qdisc add dev $DEVICE parent 1:70 handle 70: sfq perturb 10";
  system($cmd);
print "--> Filtering\n";
  $cmd="tc filter add dev $DEVICE protocol ip parent 1:0 prio 1 handle 1 fw flowid 1:10";
  system($cmd);
  $cmd="tc filter add dev $DEVICE protocol ip parent 1:0 prio 2 handle 2 fw flowid 1:20";
  system($cmd);
  $cmd="tc filter add dev $DEVICE protocol ip parent 1:0 prio 3 handle 3 fw flowid 1:30";
  system($cmd);
  $cmd="tc filter add dev $DEVICE protocol ip parent 1:0 prio 4 handle 4 fw flowid 1:40";
  system($cmd);
  $cmd="tc filter add dev $DEVICE protocol ip parent 1:0 prio 5 handle 5 fw flowid 1:50";
  system($cmd);
  $cmd="tc filter add dev $DEVICE protocol ip parent 1:0 prio 6 handle 6 fw flowid 1:60";
  system($cmd);
print "-> [DONE]\n";

print "-> Seting Firewall\n";
  $cmd="iptables -t mangle -F";
  system($cmd);

  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --syn -m length --length 40:68 -j MARK --set-mark 1";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --tcp-flags ALL SYN,ACK -m length --length 40:68 -j MARK --set-mark 1";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --tcp-flags ALL ACK -m length --length 40:100 -j MARK --set-mark 1";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --tcp-flags ALL RST -j MARK --set-mark 1";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --tcp-flags ALL ACK,RST -j MARK --set-mark 1";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --tcp-flags ALL ACK,FIN -j MARK --set-mark 1";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p udp --sport 4569 -j MARK --set-mark 2";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --sport http -j MARK --set-mark 3";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --sport https -j MARK --set-mark 3";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --sport 22 -j MARK --set-mark 4";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --sport 20 -j MARK --set-mark 5";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --sport 21 -j MARK --set-mark 5";
  system($cmd);
  $cmd="iptables -t mangle -A PREROUTING -i $DEVICE -p tcp --sport 8000 -j MARK --set-mark 6";
  system($cmd);
print "-> [DONE]\n";
