#!/bin/csh   

#-------------------
# Arguments
#-------------------

@ p = $1

#-------------------
# Variable Defs
#-------------------

set OUT_FILE="/sphenix/user/chase"

set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"
#set SCRATCH_AREA="/phenix/scratch/chase"
set SOURCE="/direct/phenix+u/vassalli/sWork/Fun4All_G4_sPHENIX.C"

#-------------------
# Export Libraries
#-------------------

source /phenix/u/vassalli/.cshrc

#-------------------
# Set Scratch Area
#-------------------

mkdir $SCRATCH_AREA/fran_chase_HEP
cp  $SOURCE $SCRATCH_AREA/fran_chase_HEP/

#-------------------
# Run Executable 
#-------------------

cd $SCRATCH_AREA/fran_chase_HEP
root Fun4All_G4_sPHENIX.C
# what is the name of the output 

rm -r $SCRATCH_AREA/fran_chase_photons
                                                       
exit 0 

