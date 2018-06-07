#!/bin/csh   

#-------------------
# Arguments
#-------------------

@ p = $1

#-------------------
# Variable Defs
#-------------------

set OUT_FILE="/sphenix/user/chase"

#set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"
set SCRATCH_AREA="/phenix/scratch/chase"
set SOURCE_PHOTONMAKER="/sphenix/user/vassalli/photonJet/photonjetmaker"

#-------------------
# Export Libraries
#-------------------

source /phenix/u/vassalli/.cshrc

#-------------------
# Set Scratch Area
#-------------------

mkdir $SCRATCH_AREA/fran_chase_photons
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/fran_chase_photons/

#-------------------
# Run Executable 
#-------------------

cd $SCRATCH_AREA/fran_chase_photons
./photonjetmaker XjPhi_pT5_${1} 5 10
cp XjPhi_pT5_${1}* $OUT_FILE


rm -r $SCRATCH_AREA/fran_chase_photons


exit 0
