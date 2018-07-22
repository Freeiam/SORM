if (%1) NEQ () GOTO end
d:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=TRUE /d_SORM_NO_MODEM=TRUE /d_SORM_MONO=TRUE tf_sorm > err.tmp
d:\tasm5.3\bin\tlink /3 /m @RESPFILE >> err.tmp 
d:\tasm5.3\bin\exe_bin.exe tf_sorm.exe
del	tf_sorm.exe
copy tf_sorm.bin d:\Dop_pult_win_8C01\
copy tf_sorm.bin d:\Dop_pult_win_8C01_COM5\
copy tf_sorm.bin d:\Dop_pult_win_8C01_COM6\
copy tf_sorm.bin C:\SIDS_ALS\ATSControlCenter\Pult_ATS\
d:\tasm5.3\bin\hexview err.tmp
:end