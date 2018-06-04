#!/bin/csh 
#This is the condor shell script for 10GeV photons, note pTHatMin should be 5GeV

                                                                                                                                          

source /phenix/u/vassalli/.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli/"

./photonjetmaker XjPhi_pT5_${1} 5 10


cp XjPhi_pT5_${1} $DESTINATION
rm XjPhi_pT5_${1}
exit 0

