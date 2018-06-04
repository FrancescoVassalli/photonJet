#!/bin/csh 
#This is the condor shell script for 30GeV photons, note pTHatMin should be 25GeV

#!/bin/csh                                                                                                                                            

source /gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli/c3"

./photonjetmaker XjPhi_pT25_${1} 25 30


cp XjPhi* $DESTINATION
exit 0

