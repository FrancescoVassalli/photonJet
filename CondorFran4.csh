#!/bin/csh 
#This is the condor shell script for 10GeV photons, note pTHatMin should be 5GeV

                                                                                                                                          

source /phenix/u/vassalli/.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet/"

./photonjetmaker XjPhi_pT35_${1} 35 40


cp XjPhi* $DESTINATION
rm XjPhi*
exit 0

