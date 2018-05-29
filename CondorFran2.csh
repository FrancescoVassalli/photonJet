#!/bin/csh                                                                     
                                                                                
source /phenix/u/vassalli/.cshrc
@ p = $1
set outfile = "/direct/phenix+u/vassalli/photonJet/"

./photonjetmaker XjPhi${p}.root
cp XjPhi${p}.root $outfile
rm XjPhi${p}.root

exit 0
