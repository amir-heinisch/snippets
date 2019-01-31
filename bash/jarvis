#!/usr/bin/env bash

# Simple script to only send messages via telegram bot api.

# Get some vars.
source $(dirname $0)/.telegram

# Set some timeout.
TIMEOUT=60

# Set max arg number.
MAX_ARG_NR=3

# Check arg count.
if [[ $# -gt MAX_ARG_NR ]]; then
	echo "To many arguments. Use 'jarvis help' for more info."
	exit 1
fi

# Switch command.
case "$1" in
	"msg")
		# Check if msg is given.
		if [[ -z "$2" ]]; then
			echo "No message given. Use 'jarvis help' for more info."
			exit 1
		fi

		# Send message.	
		curl --max-time "$TIMEOUT" -g -X GET "$API_URL$API_TOKEN/sendMessage?chat_id=$CHAT_ID&text=$MSG_TITLE$2&parse_mode=html&disable_web_page_preview=True"
		echo
		;;
	"help" | *)
		echo '      _____                                 __                            '
		echo '     /     |                               /  |                           '
		echo '     $$$$$ |  ______    ______   __     __ $$/   _______                  '
		echo '        $$ | /      \  /      \ /  \   /  |/  | /       |                 '
		echo '   __   $$ | $$$$$$  |/$$$$$$  |$$  \ /$$/ $$ |/$$$$$$$/                  '
		echo '  /  |  $$ | /    $$ |$$ |  $$/  $$  /$$/  $$ |$$      \                  '
		echo '  $$ \__$$ |/$$$$$$$ |$$ |        $$ $$/   $$ | $$$$$$  |                 '
		echo '  $$    $$/ $$    $$ |$$ |         $$$/    $$ |/     $$/                  '
		echo '   $$$$$$/   $$$$$$$/ $$/           $/     $$/ $$$$$$$/                   '
		echo '                                                                          '
		echo '  --- usage: jarvis msg <message> ---                                     '
        ;;
esac
