#!/bin/csh                                                                                                                                            

#source /phenix/u/vassalli/.cshrc

@ p = $1
#@ s = $2 + ($p / 100)
#@ p = $p % 100
#@ zcut = $3

#set truefile = "/phenix/plhf/dcm07e/dAuEnergyScan/sims/dAu39/ana/ampt_v265t5_dau39_ttree_${s}_"${p}".root"                                           
#set truefile = "/phenix/hhj/kurthill/Run16dAu_AMPT_Sims/simdata/dAu200_central/Run16dAu200_ampttrue_info_"$p".root"                                  
#set truefile = "/phenix/plhf/dcm07e/dAuEnergyScan/sims/dAu200/ana/ampt_v265t5_dau200_ttree_${s}_"${p}".root"

#set simsfile = "/phenix/hhj/kurthill/Run16dAu_AMPT_Sims/simdata/dAu200_central/Run16dAu200_ampt_dNdE_tuple_"$p".root"                                
#set simsfile = "/phenix/scratch/kurthill/Run16dAu_AMPT_Sims/simdata/dAu200_central/Run16dAu200_ampt_dNdE_tuple_"$p".root"                            
#set simsfile = "/phenix/scratch/kurthill/Run16dAu_AMPT_Sims/simdata/dAu39_central/Run16dAu39_ampt_recoTree_${s}_${p}.root"                           
#set simsfile = "/phenix/scratch/kurthill/Run16dAu_AMPT_Sims/simdata/dAu200_central/Run16dAu200_ampt_recoTree_noDCAcut_${s}_"${p}".root"

#set outfile = "/phenix/scratch/kurthill/Run16dAu_AMPT_Sims/simdata/dAu200_central/efficiency_histos_zcut_${zcut}_${s}_${p}.root"                     
set outfile = "/home/user/Dropbox/Nagel/project/photon/RCF/"

./photonjetmaker XjPhi.root
#mv XjPhi.root $outfile/XjPhi$1.root
#rm XjPhi.root

exit 0

