#!/bin/bash
###  BEGIN INIT INFO
# Provides:          ROSV_Control
# Required-Start:    $syslog $SnotraDaemon
# Required-Stop:     $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Example ROSV Control /dev/ttyS1
# Description:       Start/Stops ROSV Control daemon
###  END INIT INFO

start() {
  # Start daemon
  /usr/bin/ROSV_Control -d
  renice -n -5 $(pgrep ROSV_Control)
}

stop() {
  # Stop daemon
  killall ROSV_Control
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    retart)
        stop
        start
        ;;
    *)
echo "Usage: $0 {start|stop|restart}"
esac
exit 0
