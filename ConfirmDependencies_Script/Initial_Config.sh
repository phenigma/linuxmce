#!/bin/bash

echo "** Initial config script"

PHURL="http://www.plutohome.com/"
ACTIV="http://activate.plutohome.com"

activation_url="$ACTIV/activation.php"

DIR="/usr/pluto/install"
CDD="ConfirmDependencies_Debian.sh"
ICS="Initial_Config.sh"
wget -O "$DIR"/"$CDD"_new "$ACTIV/confirm_dependencies_debian.php" 2>/dev/null
[ "$?" -eq 0 ] && mv "$DIR"/"$CDD"_new "$DIR"/"$CDD"
wget -O "$DIR"/"$ICS"_new "$ACTIV/debian_install_script.php" 2>/dev/null
[ "$?" -eq 0 ] && mv "$DIR"/"$ICS"_new "$DIR"/"$ICS"
rm -f "$DIR"/*_new

ICS_MSG="When you have internet connection, run '$DIR/$ICS' to download your activation script from plutohome.com"
NOCODE_MSG="Whenever you get a code from $PHURL, you can run '$DIR/$ICS' to get back to this script"
ACTIV_MSG="There was a problem while retrieving/processing the activation data. You can re-run the script anytime later to try again by typing '$DIR/activation.sh' at your shells prompt"
BUILD_MSG="We created a file called '$DIR/build.sh' that you can run to make any required packages from source"
BUILD_ALL_MSG="We created a file called '$DIR/build_all.sh' which will make the entire Pluto system from source"

# ask "Try again?" question; return 1 on "Nn", 0 otherwise
try_again()
{
	echo -n "Do you want to try again? (Y/n): "
	read again
	[ "$again" == "N" -o "$again" == "n" ] && return 1
	return 0
}

ok=0
while [ "$ok" -eq 0 ]; do
# ask user for activation key
	echo -n "Please input your activation key: "
	read activation_key
	if [ -z "$activation_key" ]; then
		echo "Empty activation key. If you don't have a key, you can get one from $PHURL."
		try_again && continue
		echo "$NOCODE_MSG"
		break
	fi

# try to get data off the Internet
	ok_internet=0
	while [ "$ok_internet" -eq 0 ]; do
		answer=$(wget -O - "$activation_url?code=$activation_key" 2>/dev/null)
		if [ "$?" -ne 0 ]; then
			echo "Failed to contact activation server over the Internet"
			try_again && continue
			echo "$ICS_MSG"
			break
		fi
		ok_internet=1
	done

	[ "$ok_internet" -eq 0 ] && break

# verify activation answer
	result=$(echo "$answer" | head -1)
	if [ "$result" != "OK" ]; then
		echo "Failed getting activation data. Server answer was:"
		echo "$result"
		try_again && continue
		echo "$NOCODE_MSG"
		break
	fi

# everything went ok
	echo "Code accepted"
	echo "$answer" | awk 'NR>1' >"$DIR/activation.sh"
	ok=1
done

[ "$ok" -eq 0 ] && exit 1
chmod +x "$DIR"/activation.sh
if "$DIR"/activation.sh; then
	echo "Activation went ok"
else
	echo "$ACTIV_MSG"
fi

wget -O "$DIR"/build.sh "$ACTIV/build.php?code=$activation_key" 2>/dev/null || no_build=1
wget -O "$DIR"/build_all.sh "$ACTIV/build_all.php?code=$activation_key" 2>/dev/null || no_build_all=1

if [ -z "$no_build" -a -s "$DIR"/build.sh ]; then
	chmod +x "$DIR"/build.sh
	echo "$BUILD_MSG"
fi
if [ -z "$no_build_all" -a -s "$DIR"/build_all.sh ]; then
	chmod +x "$DIR"/build_all.sh
	echo "$BUILD_ALL_MSG"
fi

wget -O "$DIR/message.txt" "$ACTIV/message.php?code=$CODE" 2>/dev/null && cat "$DIR/message.txt"

