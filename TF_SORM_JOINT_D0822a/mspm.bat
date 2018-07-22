if (%1) NEQ () GOTO end
d:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=FALSE /d_SORM_NO_MODEM=TRUE tf_sorm > err.tmp
d:\tasm5.3\bin\tlink /3  /m @RESPMSPM >> err.tmp 
copy tf_sorm.exe d:\Dop_pult_win_8C01\PROG.EXE
copy tf_sorm.exe d:\Dop_pult_win_8C01_COM5\PROG.EXE
copy tf_sorm.exe d:\Dop_pult_win_8C01_COM6\PROG.EXE
copy tf_sorm.exe C:\SIDS_ALS\ATSControlCenter\Pult_ATS\PROG.EXE
del prog.exe
rename tf_sorm.exe prog.exe
d:\tasm5.3\bin\hexview err.tmp
:end