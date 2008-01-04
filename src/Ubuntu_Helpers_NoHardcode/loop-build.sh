#!/bin/bash -x
. /etc/lmce-build/builder.conf

while : ;do
	echo "$(date -R) Started" >> /var/log/loop-build.log	

	# Start Build
	/usr/local/lmce-build/build.sh

	# Remove old Dir and keep last 5 build (hint: tail +5)
	for remove_dir in $(for dir in $(ls -r -d -X /home/ftp/AutoBuilds/build-*-${arch}); do echo $dir ;done | tail +5) ;do
		rm -rf "$remove_dir"
	done

	# Create Backup Dir
	build_ftp_dir="/home/ftp/AutoBuilds/build-$(date +%y_%m_%d_%s)-${arch}"
	mkdir -p "${build_ftp_dir}"

	# Backup the ISO
	mkdir -p "${build_ftp_dir}/ISO"
	cp /var/www/*.iso "${build_ftp_dir}/ISO/"

	# Cleanup the Build Dir of Private Sources
	rm -rf /var/lmce-build/svn/trunk/src/ZWave
	rm -rf /var/lmce-build/svn/trunk/src/Fiire_Scripts
	rm -rf /var/lmce-build/svn/trunk/src/RFID_Interface
	rm -rf /var/lmce-build/svn/trunk/src/lmce_launch_manager	

	# Backup the Build Dir
	mkdir -p "${build_ftp_dir}/BUILD"
	cp -r /var/lmce-build/svn/trunk/src/* "${build_ftp_dir}/BUILD"
	
	# Backup the log
	mkdir -p "${build_ftp_dir}/LOGS"
	cp /var/log/lmce-build.log "${build_ftp_dir}/LOGS"
	
	echo "$(date -R) Done" >> /var/log/loop-build.log
done 
