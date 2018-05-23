#!/bin/bash
# run this with $bash CominationShell.sh
# run this script to get files from your RCF that are <2GB
RUN=1
FRONT="photonTree_" #put the file name here 
BACK=".root" #put the extension here
START=0 #put the number of the file you want to start iternating at here
STOP=50 #last number here 
USRNAME=vassalli #your RCF username here
DIRECTORY=/direct/phenix+u/$USRNAME/
LOCAL=/home/user/Dropbox/Nagel/project/photon/
FILENUMBER=$START
let "START++"
while [ $FILENUMBER -le $STOP ]; do
	NAME=$FRONT$FILENUMBER$BACK
	scp $USRNAME@rftpexp.rhic.bnl.gov:$DIRECTORY/$NAME $LOCAL$NAME
	let "FILENUMBER++"
done