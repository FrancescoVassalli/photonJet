#!/bin/csh 
#This is the condor shell script for 30-40GeV photons, note pTHatMin should be 25GeV
                                                                                                                                            

#-------------------                                                                                                                                 
# Arguments                                                                                                               
#-------------------                                                                                                                                  

@ p = $1

#-------------------                                                                                                                                 # Variable Defs                                                                                                                                      
#-------------------                                                                                                                                  

set OUT_FILE="/sphenix/user/vassalli/XjPhi3"

set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                                                                              
#set SCRATCH_AREA="/phenix/scratch/chase"

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

#-------------------                                                                                                                                 # Run Executable                                                                                                                                     
#-------------------                                                                                                                                  

cd $SCRATCH_AREA/fran_chase_photons
./photonjetmaker XjPhi2_pT25_${1} 25 30 3000000
cp XjPhi3_pT25_${1}* $OUT_FILE


rm -r $SCRATCH_AREA/fran_chase_photons


exit 0
