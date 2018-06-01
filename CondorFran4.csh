#This is the condor shell script for 40GeV photons, note pTHatMin should be 35GeV

#!/bin/csh                                                                                                                                            

source /gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli"

./photonjetmaker XjPhi_pT15_${1} 35 40


cp XjPhi* $DESTINATION
exit 0

