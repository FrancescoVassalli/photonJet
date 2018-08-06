#!/bin/csh 

@ p = $1

set OUT_FILE="LHCfrag_80_${1}.root"

set OUT_LOCATION="/sphenix/user/vassalli/LHCfrag/"

set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/photonJet/photonjetmaker"

source /phenix/u/vassalli/.cshrc

./photonjetmaker $OUT_FILE 80 100 1000

exit 0
