#!/bin/csh                                                                      
#This is the condor shell script for 10GeV photons, note pTHatMin should be 5Ge\
V                                                                               

                                                                               \
set OUT_FILE="/gpfs/mnt/gpfs04/sphenix/user/chase"

set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"

set SOURCE_PHOTONMAKER="/gpfs/mnt/gpfs04/sphenix/user/vassalli/photonJet/photon\
jetmaker"

source /phenix/u/vassalli/.cshrc

@ p = $1

mkdir $SCRATCH_AREA/fran_chase_photons
cp -r $SOURCE_PHOTONMAKER $SCRATCH_AREA/fran_chase_photons

set DESTINATION = "/gpfs/mnt/gpfs04/sphenix/user/vassall"


cd $SCRATCH_AREA/fran_chase_photons
root /direct/phenix+u/vassalli/sWork/Fun4All_G4_sPHENIX.C $1

exit 0

