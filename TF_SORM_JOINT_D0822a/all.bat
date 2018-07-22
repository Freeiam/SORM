if (%1) NEQ () GOTO end
rem 
C:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=TRUE /d_SORM_NO_MODEM=TRUE /d_SORM_MONO=TRUE tf_sorm > err.tmp
C:\tasm5.3\bin\tlink /3 /m @RESPFILE >> err.tmp 
C:\tasm5.3\bin\exe_bin.exe tf_sorm.exe 
del tf_sorm.exe
copy tf_sorm.bin EXECUTABLE\NO_MODEM_WHITE\
rem 
C:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=TRUE /d_SORM_NO_MODEM=TRUE /d_SORM_MONO=FALSE tf_sorm >> err.tmp
C:\tasm5.3\bin\tlink /3 /m @RESPFILE >> err.tmp 
C:\tasm5.3\bin\exe_bin.exe tf_sorm.exe 
del tf_sorm.exe
copy tf_sorm.bin EXECUTABLE\NO_MODEM_BLACK\
rem 
C:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=TRUE /d_SORM_NO_MODEM=FALSE tf_sorm >> err.tmp
C:\tasm5.3\bin\tlink /3 /m @RESPFILE >> err.tmp 
C:\tasm5.3\bin\exe_bin.exe tf_sorm.exe 
del tf_sorm.exe
copy tf_sorm.bin EXECUTABLE\MODEM\
rem
del tf_sorm.bin
del prog.exe
rem
C:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=FALSE /d_SORM_NO_MODEM=TRUE tf_sorm >> err.tmp
C:\tasm5.3\bin\tlink /3  /m @RESPMSPM >> err.tmp 
copy tf_sorm.exe EXECUTABLE\BUN\prog.exe
rem del tf_sorm.exe
rem
C:\tasm5.3\bin\hexview err.tmp
:end