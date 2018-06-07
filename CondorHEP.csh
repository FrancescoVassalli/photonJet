#!/bin/csh   

#-------------------
# Arguments
#-------------------

@ p = $1

#-------------------
# Variable Defs
#-------------------

set OUT_FILE="/sphenix/user/vassalli/HEPOut"

#set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"
set SCRATCH_AREA="/phenix/scratch/chase"
set SOURCE="/direct/phenix+u/vassalli/sWork/Fun4All_G4_sPHENIX.C"
set OUT_NAME="G4sPHENIX${1}.root"
set G4IMPORT="/direct/phenix+u/vassalli/sWork/"
#-------------------
# Export Libraries
#-------------------

source /phenix/u/vassalli/.cshrc

#-------------------
# Set Scratch Area
#-------------------

mkdir $SCRATCH_AREA/fran_chase_HEP
cp  $SOURCE $SCRATCH_AREA/fran_chase_HEP/
cp $G4IMPORT/* $SCRATCH_AREA/fran_chase_HEP/
#-------------------
# Run Executable 
#-------------------



cd $SCRATCH_AREA/fran_chase_HEP
root Fun4All_G4_sPHENIX.C\(\"$outfile\"\)
cp $OUT_NAME $OUT_FILE
rm -r $SCRATCH_AREA/fran_chase_HEP
                                                       
exit 0 

