@rem original version from eAthena, thanks. 

@echo off
if "%1" == "boot" goto boot
set __bin__=
if exist "bin\login-server.exe" set __bin__=bin\
start win32_start boot %__bin__%login-server.exe
start win32_start boot %__bin__%char-server.exe
start win32_start boot %__bin__%map-server.exe
goto end

:boot
if not exist %2 goto end
echo Athena �����ċN���X�N���v�g for WIN32
echo.
echo %2 �ُ̈�I�����Ď����ł��B
echo �T�[�o�[���I������ɂ́A�ŏ��ɂ��̃E�B���h�E����Ă��������B
start /wait %2
cls
echo %2 ���I�����܂����B�ċN�����܂��B
echo. | date /T
echo. | time /T
goto boot

:end
echo %2
