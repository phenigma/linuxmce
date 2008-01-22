#!/bin/bash -x
. /etc/lmce-build/builder.conf

while : ;do
	echo "$(date -R) Started" >> /var/log/loop-build.log	

	# Start Build
	BuildFinished="true"
	/usr/local/lmce-build/build.sh || BuildFinished="false"

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
	rm -f /var/www/*.iso

	# Cleanup the Build Dir of Private Sources
	rm -rf /var/lmce-build/svn/trunk/src/ZWave
	rm -rf /var/lmce-build/svn/trunk/src/Fiire_Scripts
	rm -rf /var/lmce-build/svn/trunk/src/RFID_Interface
	rm -rf /var/lmce-build/svn/trunk/src/lmce_launch_manager	

	# Backup the Build Dir
	mkdir -p "${build_ftp_dir}/BUILD"
	cp -r /var/lmce-build/svn/trunk/src/* "${build_ftp_dir}/BUILD"
	rm -rf /var/lmce-build/svn/*

	# Backup the Debs
	mkdir -p "${build_ftp_dir}/DEBS"
	cp -r /var/www/*.deb "${build_ftp_dir}/DEBS"
	rm -f /var/www/*.deb
	
	# Backup the log
	mkdir -p "${build_ftp_dir}/LOGS"
	cp /var/log/lmce-build.log "${build_ftp_dir}/LOGS"
	
	echo "$(date -R) Done" >> /var/log/loop-build.log

	# Send mail informing that the build had finished
	if [[ "$BuildFinished" == "true" ]] ;then
		mail_txt_file=$(mktemp)
		echo                                                     >$mail_txt_file
		echo "Arch   : ${arch}"                                 >>$mail_txt_file
		echo "Flavor : ${flavor}"                               >>$mail_txt_file
		echo "Date   : $(date -R)"                              >>$mail_txt_file
		echo                                                    >>$mail_txt_file
		echo "Build Completed"					>>$mail_txt_file
		echo                                                    >>$mail_txt_file
		echo " http://builder32.linuxmce.com/AutoBuilds/$(basename $build_ftp_dir)" >>$mail_txt_file
		echo
		cat $mail_txt_file | mail -s "$mail_subject_prefix Build Completed" "${mail_to}"
		rm -rf $mail_txt_file
	fi
done 
