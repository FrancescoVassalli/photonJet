#This is the condor shell script for 30GeV photons, note pTHatMin should be 25GeV

#!/bin/csh                                                                                                                                            

source /gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli"

./photonjetmaker XjPhi_pT25_${1} HepMC_pT25_${1} 25 30


cp XjPhi.root $DESTINATION
rm XjPhi.root

exit 0

