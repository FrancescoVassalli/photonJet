#!/bin/csh 
#This is the condor shell script for 20GeV photons, note pTHatMin should be 15GeV

#!/bin/csh                                                                                                                                            

source /gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli"

./photonjetmaker XjPhi_pT15_${1} 15 20


mv XjPhi* $DESTINATION
exit 0

