#!/bin/csh 
#This is the condor shell script for >40GeV photons, note pTHatMin should be 35GeV
                                                                                                                                            

#-------------------                                                                                                                                 
# Arguments                                                                                                               
#-------------------                                                                                                                                  

@ p = $1

#-------------------                                                                                                                                 # Variable Defs                                                                                                                                      
#-------------------                                                                                                                                  

set OUT_FILE="/sphenix/user/vassalli/XjPhi2"

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
./photonjetmaker XjPhi2_pT35_${1} 35 40 3000000
cp XjPhi2_pT35_${1}* $OUT_FILE


rm -r $SCRATCH_AREA/fran_chase_photons


exit 0
