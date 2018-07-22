if (%1) NEQ () GOTO GO
call tf_sorm.bat MSPM
goto end
:GO
C:\tasm5.3\bin\tasm32 /m9 /l /kh40000 /d_PUMA=TRUE /d_MSP=FALSE /d_SORM_NO_MODEM=TRUE /d_SORM_MONO=TRUE tf_sorm > err.tmp
echo on
if  (%1) == (MSPM) goto MSPM 
rem pause
rem --- Get MSP ---
C:\tasm5.3\bin\tlink /3  /m @RESPFILE >> err.tmp 
C:\tasm5.3\bin\exe_bin.exe tf_sorm.exe
del	tf_sorm.exe
goto exit
:MSPM
rem --- Get MSP-M ---
C:\tasm5.3\bin\tlink /3  /m @RESPMSPM >> err.tmp
IF EXIST PROG.EXE del PROG.EXE
rename tf_sorm.exe PROG.EXE
:exit 
C:\tasm5.3\bin\hexview err.tmp
:end