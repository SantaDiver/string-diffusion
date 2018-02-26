#!/bin/bash
#
# sys.xsession:	Login for an X session, will be executed
#		by the Xsession script of the xdm with
#		the help of the login shell of the user.
#

#
# If ssh is configured and ssh-agent is wanted set "yes"
#
usessh="yes"

#
# If gpg is configured and gpg-agent is wanted set "yes"
#
usegpg="yes"

#
# What we do if we fail at least ... emergency fall back.
#
failsafe="xterm -ls -T Failsafe -geometry 80x24+0+0"
trap "exec $failsafe" EXIT SIGHUP SIGINT SIGPIPE SIGTERM SIGIO

#
# Some bash (1 and 2) settings to avoid trouble on a
# failed program call.
#
set +e > /dev/null 2>&1
set +u > /dev/null 2>&1
set +o posix  > /dev/null 2>&1
if type shopt > /dev/null 2>&1 ; then
    shopt -s execfail
else
    no_exit_on_failed_exec=1
fi

#
# Run ssh-agent only if ssh is configured and avaliable.
#
sshagent="no"
SSH_ASKPASS=""
if test "$usessh" = "yes" -a -d $HOME/.ssh ; then
    type -p ssh-agent > /dev/null 2>&1 && sshagent="yes"
    if test -x /usr/lib/ssh/x11-ssh-askpass ; then
	SSH_ASKPASS="/usr/lib/ssh/x11-ssh-askpass"
	export SSH_ASKPASS
    elif test -x /usr/lib64/ssh/x11-ssh-askpass ; then
	SSH_ASKPASS="/usr/lib64/ssh/x11-ssh-askpass"
	export SSH_ASKPASS
    fi
fi

#
# Run gpg-agent only if gpg-agent is configured and avaliable.
# Note that the method in the following few lines is necessary
# because the gpg2 developers force every user to use the
# daemon mode regardless of the staying gpg-agent after leaving
# the X session.  If the method of the ssh-agent of getting
# a secure X session would be used, the number of running
# gpg-agent would increase on every new X session.
#
if test "$usegpg" = "yes" -a -d $HOME/.gnupg ; then
    GPG_AGENT="$(type -p gpg-agent 2>/dev/null)"
    if test -n "$GPG_AGENT" -a -x "$GPG_AGENT"; then
	GPG_AGENT_FILE=$HOME/.gnupg/agent.info
	GPG_AGPID_FILE=$HOME/.gnupg/agent.pid
	if /sbin/checkproc -p "$GPG_AGPID_FILE" "$GPG_AGENT" ; then
	    read -t1 GPG_AGENT_INFO < "$GPG_AGENT_FILE"
	    export GPG_AGENT_INFO
	else
	    eval $($GPG_AGENT --sh --daemon 2>/dev/null)
	    GPG_AGENT_PID="${GPG_AGENT_INFO#*:}"
	    GPG_AGENT_PID="${GPG_AGENT_PID%%:*}"
	    echo "$GPG_AGENT_INFO" > "$GPG_AGENT_FILE"
	    echo "$GPG_AGENT_PID"  > "$GPG_AGPID_FILE"
	fi
    fi
fi

XINITRCFILE=/etc/X11/xinit/xinitrc
test -r $HOME/.xinitrc && XINITRCFILE=$HOME/.xinitrc

if test -f $XINITRCFILE ; then
    if test "$sshagent" = "yes" ; then
	exec ssh-agent /bin/bash $XINITRCFILE
    else
	exec /bin/bash $XINITRCFILE
    fi
else
    #
    # Source common code shared between the
    # X session and X init scripts
    #
    . /etc/X11/xinit/xinitrc.common

    unset WINDOW_MANAGER STARTUP
    if test "$sshagent" = "yes" ; then
	exec ssh-agent $WINDOWMANAGER
    else
	exec $WINDOWMANAGER
    fi
fi

# call failsafe
exit 0
