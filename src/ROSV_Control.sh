#!/bin/bash
# Provides:          ROSV_Control
# Required-Start:    $syslog
# Required-Stop:     $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Example ROSV Control
# Description:       Start/Stops ROSV Control program

start() {
  # Start program
  ROSV_Control -d
  renice -15 $(pgrep ROSV_Control)
}

stop() {
  # Stop program
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
