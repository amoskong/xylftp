#!/bin/bash
#Write for xylftp server 
#Author:cocobear

xylftp_conf="/etc/xylftp/xylftp.conf"
xylftp_pass="/etc/xylftp/xylftp.pass"
xylftp_dir="/usr/loca/xylftp"
xylftp="xylftp"
xylftp_bin="$xylftp_dir/bin/$xylftp"
syslog="/etc/syslog.conf"
add_opt="local1.*\t\t\t\t\t\t/var/log/xylftp.log"
function init() {
	grep 'local1.*' $syslog > /dev/null
	if (($? != 0)) ; then
		echo -e "Now add a option int /etc/syslog.conf!"
		echo -e $add_opt >> $syslog
		killall -HUP syslogd > /dev/null
	else
		echo -e "/etc/syslog.conf is ok!"
	fi
	
	return 0
	
}

function start() {
	if [ -e $xylftp_conf ] ; then
		continue
	else
		echo -e "xylftp.conf file not exist!"	
		exit 1
	fi
	./xylftp	
	
}

function stop() {
	killall $xylftp > /dev/null 2>&1
	if (($? == 0)) ; then
		echo -e "Stop xylftp success!"
	else
		echo -e "Stop xylftp failed!"
	fi 
	
}

function status() {
	ps -e | grep $xylftp > /dev/null
	if (($? == 0)) ; then
		echo -e "xylftp is running"
	else
		echo -e "xylftp is not running"
	fi

}
function help() {
	echo -e "Usage:run.sh\tstart|stop|restart|status|init"
}


if (($# == 0)) ; then 
	help
	exit 1
fi

if (($UID != 0)) ; then
	echo "You MUST be ROOT run this script!"
	exit 1
fi


case "$1" in 
	init)
		init	
		;;
	start)
		init
		start
		;;
	stop)
		stop
		;;
	restart)
		stop
		start
		;;	
	status)
		status
		;;
	*)
		help
		exit 1
esac

exit 0
