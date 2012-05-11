#!/bin/sh

echo $? > ~/install-exit-status

echo "#!/bin/sh
smbclient //rob/HDD -U USER%PASSWORD -c \"get 1GB\" 2>&1 | grep getting > \$LOG_FILE
echo \$? > ~/test-exit-status" > samba_1GB
chmod +x samba_1GB

