#!/usr/bin/perl

#=========================================================================
# serverstatus.cgi  ver.1.01  by �Ӓ���
#	checkversion�����b�v�����A�T�[�o�[��Ԃ�\������cgi
#
# ** �ݒ���@ **
#
# - ����$checkv�ϐ���checkversion�ւ̃p�X��ݒ肷�邱�ƁB
# - perl�Ƀp�X���ʂ��Ă��Ȃ��ꍇ�� $perl ��perl�ւ̐������p�X�ɂ��邱�ƁB
# - ���͕��ʂ�CGI�Ɠ����B�i���s����cgi-bin�t�H���_�Ȃǁj
# - %servers��ip�����ꂼ�ꐳ�����ݒ肷��B�z�X�g���B���݂�inter��char�͓���
# - ����map�I�ɂ��Ă���ꍇ��,%servers�ɍs�������邱�ƂőΉ��\�����A
#   @serverorder��%servers�̃L�[�������邱�Ƃ�Y��Ȃ��悤�ɁB
# - @state1,@state2�ŕ\���𑽏��ύX�\�B
#
# ** ���̑� **
# - �����܂Ō��݂̏�ԕ\���Ń��O�͂Ƃ��Ă��Ȃ��̂ŉߋ��̃f�[�^�͎Q�Ƃł��Ȃ�
# - �L���b�V���Ȃǂ��Ă��Ȃ��̂ŃA�N�Z�X����邽�тɃ`�F�b�N����B
#   ����ăA�N�Z�X�������ƕ��ׂ��傫���Ȃ�̂Œ��ӁB
# - ping�ɂ��`�F�b�N�͂��܂����x�Btcp-ping�͕��ׂ������B
#	icmp-ping�͕��׌y�߂���root�����K�v�Ȃ̂Ŏ��������B
#   Net::Ping�K�{�BActivePerl�Ȃǂł�alarm���������œ����Ȃ��\������B
#   ���܂������Ȃ��Ȃ�ping���Ȃ��ق������ׂ��Ⴍ�Ȃ�B
#
#-------------------------------------------------------------------------


my($checkv)="../checkversion";	# checkversion�̃p�X(�����炭�ύX���K�v)
my($perl)="perl";				# perl�̃R�}���h��

my($checkping)="tcp";			# NG�̂Ƃ�ping�ɂ��`�F�b�N���s��ping�̎��
								# "tcp","udp","icmp"(root���K�v),""����I��
								# ""����ping���Ȃ��B
								# Net::Ping���Ȃ�/��������Ă��Ȃ��Ɩ���

my(@serverorder)=(				# �\����
	"login","char","inter","map"
);
my(%servers)=(					# �f�[�^(ip�Ɩ��O)
	"login"	=> { "ip"=>"127.0.0.1:6900", "desc"=>"Login Server" },
	"char"	=> { "ip"=>"127.0.0.1:6121", "desc"=>"Charactor Server" },
	"inter"	=> { "ip"=>"127.0.0.1:6121", "desc"=>"Inter Server" },
	"map"	=> { "ip"=>"127.0.0.1:5121", "desc"=>"Map Server" },
);

my(@state1)=(					# ��ԕ\��
	"Down",		# �ڑ��ł��Ȃ�
	"Good",		# ���퓮�쒆
	"Error",	# %servers�̐ݒ肪��������(�|�[�g�ԍ�)
	"Closed",	# ping�ɂ͉���
);
my(@state2)=(					# �F
	"#ffc0c0",	# �ڑ��ł��Ȃ�
	"#c0ffc0",	# ���퓮�쒆
	"#c0c0ff",	# %servers�̐ݒ肪��������(�|�[�g�ԍ�)
	"#ffffc0",	# ping�ɂ͉���
);

#--------------------------- �ݒ肱���܂� --------------------------------




use strict;
eval " use Net::Ping; ";


my($msg)=<<"EOD";
<html>
<head><title>Athena Server Status</title></head>
<body text="black" bgcolor="white" link="blue" vlink="blue" alink="blue">
<h1>Athena Server Status</h1>
<table border=1>
<tr><th>Server</th><th>Address</th><th>Status</th><th>Version</th></tr>
EOD

my(%langconv)=(
);

my($i);
foreach $i (@serverorder){
	my($state)=0;
	
	open PIPE,"$perl $checkv $servers{$i}->{ip} |"
		or HttpError("Can't execute checkversion.\n");
	my(@dat)=<PIPE>;
	close PIPE;
	
	if($dat[1]=~m/Athena/ && $dat[2]=~/server/){
		if($dat[2]=~/$i/ ){
			$state=1;
		}else{
			$state=2;
		}
	}elsif($checkping){
		eval { 
			$dat[1]="n/a";
			my($p) = Net::Ping->new($checkping);
			my($addr)=$servers{$i}->{ip};
			$addr=~s/\:\d+$//;
			$state=3 if $p->ping($addr);
			$p->close();
		};
	}
	
	$msg.= "<tr bgcolor=\"$state2[$state]\"><td>".$servers{$i}->{desc}.
		"</td><td>".$servers{$i}->{ip}."</td><td>$state1[$state]</td>".
		"<td>$dat[1]</td></tr>"
}
$msg.="</table></body></html>";

print "Content-type: text/html\n\n$msg";

sub LangConv {
	my(@lst)= @_;
	my($a,$b,@out)=();
	foreach $a(@lst){
		foreach $b(keys %langconv){
			$a=~s/$b/$langconv{$b}/g;
			my($rep1)=$1;
			$a=~s/\$1/$rep1/g;
		}
		push @out,$a;
	}
	return @out;
}

sub HttpMsg {
	my($msg)=join("", LangConv(@_));
	$msg=~s/\n/<br>\n/g;
	print LangConv("Content-type: text/html\n\n"),$msg;
	exit;
}

sub HttpError {
	my($msg)=join("", LangConv(@_));
	$msg=~s/\n/<br>\n/g;
	print LangConv("Content-type: text/html\n\n"),$msg;
	exit;
}


