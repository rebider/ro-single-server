set __common__=..\common\core.c ..\common\db.c ..\common\grfio.c ..\common\lock.c ..\common\malloc.c ..\common\nullpo.c ..\common\socket.c ..\common\timer.c
set __define__=-DPACKETVER=6 -DNEW_006b -DFD_SETSIZE=4096
set __include__=-I../common/

@rem Warning ��900��o�Ă��ĉ����Ȃ񂾂�������Ȃ��̂ŁA�ꕔ�}���B
@rem �C������C�͋N���Ȃ��̂ő��͖{�胂�[�h���ۂ��ł��B
@rem     W7035 : �����t���l�ƕ����Ȃ��l�̔�r
@rem     W8004 : **** �ɑ�������l�͎g���Ă��Ȃ�
@rem     W8012 : �����t���l�ƕ����Ȃ��l�̔�r
@rem     W8057 : �p�����[�^ **** �͈�x���g�p����Ȃ�

set __warning__=-w-7035 -w-8004 -w-8012 -w-8057

cd src\login
bcc32 -j255 %__warning__% %__define__% %__include__% login.c md5calc.c %__common__%
copy login.exe ..\..\login-server.exe

cd ..\char
bcc32 -j255 %__warning__% %__define__% %__include__% char.c int_pet.c int_guild.c int_party.c int_storage.c inter.c %__common__%
copy char.exe ..\..\char-server.exe

cd ..\map
bcc32 -j255 %__warning__% %__define__% %__include__% map.c npc.c battle.c chat.c chrif.c clif.c guild.c intif.c itemdb.c mob.c atcommand.c party.c path.c pc.c pet.c status.c script.c skill.c storage.c trade.c vending.c %__common__%
copy map.exe ..\..\map-server.exe

cd ..\..\
