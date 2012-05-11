#!/bin/bash

#file_sizes=( 1MB )
file_sizes=( 128KB 1MB 10MB 100MB 1GB )


function install_tests() {

	./phoronix-test-suite install $@
}


function disable_smbtad() {
	ssh -l USER SERVER "sudo sed -i '/vfs objects = smb_traffic_analyzer/,/smb_traffic_analyzer:port = 3490/s/^\t/#\t/' /etc/samba/smb.conf" && \
	ssh -l USER SERVER "sudo /etc/init.d/samba restart&"
}

function enable_smbtad() {
	ssh -l USER SERVER "sudo sed -i '/vfs objects = smb_traffic_analyzer/,/smb_traffic_analyzer:port = 3490/s/^#//' /etc/samba/smb.conf" && \
	ssh -l USER SERVER "sudo /etc/init.d/samba restart&"
}

function run_tests() {
	./phoronix-test-suite batch-run $@
}

for a in ${file_sizes[@]}; do
	install_tests samba_${a}
done
disable_smbtad
for a in ${file_sizes[@]}; do
	echo Running pure samba tests ${a}
	run_tests samba_${a}
done
enable_smbtad
for a in ${file_sizes[@]}; do
	echo Running samba + smbtad ${a}
        run_tests samba_${a}
done    
        
