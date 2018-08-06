#!/bin/csh 

@ p = $1

set OUT_FILE="/sphenix/user/vassalli/LHCfrag/LHCfrag_80_${1}"

set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                                                                              
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/photonJet/photonjetmaker"

source /phenix/u/vassalli/.cshrc

set SCRATCH_FOLDER="LHCfrag"

mkdir $SCRATCH_AREA/$SCRATCH_FOLDER
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/$SCRATCH_FOLDER/

cd $SCRATCH_AREA/$SCRATCH_FOLDER
./photonjetmaker $OUT_FILE 80 100 1000

rm -r $SCRATCH_AREA/$SCRATCH_FOLDER

exit 0
