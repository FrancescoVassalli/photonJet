#This is the condor shell script for 10GeV photons, note pTHatMin should be 5GeV

#!/bin/csh                                                                                                                                            

source /gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet.cshrc

@ p = $1

                
set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassalli"

./photonjetmaker XjPhi_pT10_${1} HepMC_pT10_${1} 5 10


cp XjPhi.root $DESTINATION
rm XjPhi.root

exit 0

