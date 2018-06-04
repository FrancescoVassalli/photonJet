#!/bin/csh 
#This is the condor shell script for 20GeV photons, note pTHatMin should be 15GeV

#!/bin/csh                                                                                                                                            

source /gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli/c2"

./photonjetmaker XjPhi_pT15_${1} 15 20


cp XjPhi_pT15_${1}.root $DESTINATION
cp XjPhi_pT15_${1}.dat $DESTINATION
rm XjPhi_pT15_${1}.root
rm XjPhi_pT15_${1}.root

exit 0

