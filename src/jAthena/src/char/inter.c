#include <string.h>
#include <stdlib.h>
#include "mmo.h"
#include "char.h"
#include "socket.h"
#include "timer.h"
#include "db.h"
#include "lock.h"
#include "malloc.h"

#include "inter.h"
#include "int_party.h"
#include "int_guild.h"
#include "int_storage.h"
#include "int_pet.h"

#define WISDATA_TTL		(60*1000)	// Wis�f�[�^�̐�������(60�b)
#define WISDELLIST_MAX	128			// Wis�f�[�^�폜���X�g�̗v�f��

char inter_log_filename[1024]="log/inter.log";

char accreg_txt[1024]="save/accreg.txt";
static struct dbt *accreg_db=NULL;

struct accreg {
	int account_id,reg_num;
	struct global_reg reg[ACCOUNT_REG_NUM];
};

int party_share_level = 10;


// ���M�p�P�b�g�����X�g
int inter_send_packet_length[]={
	-1,-1,27, 0, -1, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	-1, 7, 0, 0,  0, 0, 0, 0, -1,11, 0, 0,  0, 0,  0, 0,
	35,-1,11,15, 34,29, 7,-1,  0, 0, 0, 0,  0, 0,  0, 0,
	10,-1,15, 0, 79,19, 7,-1,  0,-1,-1,-1, 14,67,186,-1,
	 9, 9,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	11,-1, 7, 3,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	31,51,51,-1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
};
// ��M�p�P�b�g�����X�g
int inter_recv_packet_length[]={
	-1,-1, 7, 0, -1, 6, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 6,-1, 0, 0,  0, 0, 0, 0, 10,-1, 0, 0,  0, 0,  0, 0,
	72, 6,52,14, 10,29, 6,-1, 34, 0, 0, 0,  0, 0,  0, 0,
	-1, 6,-1, 0, 55,19, 6,-1, 14,-1,-1,-1, 18,19,186,-1,
	 5, 9, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	48,14,-1, 6,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
	31,51,51,-1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,  0, 0,
};


struct WisData {
	int id,fd,count,len;
	unsigned long tick;
	unsigned char src[24],dst[24],msg[512];
};
static struct dbt * wis_db = NULL;
static int wis_dellist[WISDELLIST_MAX], wis_delnum;


// WIS�f�[�^�̐����`�F�b�N
int check_ttl_wisdata_sub(void *key,void *data,va_list ap)
{
	unsigned long tick;
	struct WisData *wd=(struct WisData *)data;
	tick=va_arg(ap,unsigned long);
	
	if( DIFF_TICK(tick,wd->tick)>WISDATA_TTL && wis_delnum< WISDELLIST_MAX ){
		wis_dellist[wis_delnum++]=wd->id;
	}
	return 0;
}
int check_ttl_wisdata()
{
	unsigned long tick=gettick();
	int i;
	
	do{
		wis_delnum=0;
		numdb_foreach( wis_db, check_ttl_wisdata_sub, tick );
		for(i=0;i<wis_delnum;i++){
			struct WisData *wd=numdb_search(wis_db,wis_dellist[i]);
			printf("inter: wis data id=%d time out : from %s to %s\n",
				wd->id,wd->src,wd->dst);
			numdb_erase(wis_db,wd->id);
			free(wd);
		}
	}while(wis_delnum>=WISDELLIST_MAX);
	return 0;
}

//--------------------------------------------------------

// �A�J�E���g�ϐ��𕶎���֕ϊ�
int inter_accreg_tostr(char *str,struct accreg *reg)
{
	int j;
	char *p=str;
	p+=sprintf(p,"%d\t",reg->account_id);
	for(j=0;j<reg->reg_num;j++){
		p+=sprintf(p,"%s,%d ",reg->reg[j].str,reg->reg[j].value);
	}
	return 0;
}
// �A�J�E���g�ϐ��𕶎��񂩂�ϊ�
int inter_accreg_fromstr(const char *str,struct accreg *reg)
{
	int j,v,n;
	char buf[128];
	const char *p=str;
	if( sscanf(p,"%d\t%n",&reg->account_id,&n )!=1 || reg->account_id<=0)
		return 1;
	
	for(j=0,p+=n;j<ACCOUNT_REG_NUM;j++,p+=n){
		if( sscanf(p,"%[^,],%d %n",buf,&v,&n)!=2 )
			break;
		memcpy(reg->reg[j].str,buf,32);
		reg->reg[j].value=v;
	}
	reg->reg_num=j;
	return 0;
}

// �A�J�E���g�ϐ��̓ǂݍ���
int inter_accreg_init()
{
	char line[8192];
	FILE *fp;
	int c=0;
	struct accreg *reg;
	
	accreg_db=numdb_init();
	
	if( (fp=fopen(accreg_txt,"r"))==NULL )
		return 1;
	while(fgets(line,sizeof(line),fp)){
	
		reg=(struct accreg *)aCalloc(1,sizeof(struct accreg));
		if(inter_accreg_fromstr(line,reg)==0 && reg->account_id>0){
			numdb_insert(accreg_db,reg->account_id,reg);
		}else{
			printf("inter: accreg: broken data [%s] line %d\n",accreg_txt,c);
			free(reg);
		}
		c++;
	}
	fclose(fp);
//	printf("inter: %s read done (%d)\n",accreg_txt,c);
	return 0;
	
}
// �A�J�E���g�ϐ��̃Z�[�u�p
int inter_accreg_save_sub(void *key,void *data,va_list ap)
{
	char line[8192];
	FILE *fp;
	struct accreg *reg=(struct accreg *)data;
	if(reg->reg_num>0){
		inter_accreg_tostr(line,reg);
		fp=va_arg(ap,FILE *);
		fprintf(fp,"%s" RETCODE,line);
	}
	return 0;
}
// �A�J�E���g�ϐ��̃Z�[�u
int inter_accreg_save()
{
	FILE *fp;
	int  lock;
	if( (fp=lock_fopen(accreg_txt,&lock))==NULL ){
		printf("int_accreg: cant write [%s] !!! data is lost !!!\n",accreg_txt);
		return 1;
	}
	numdb_foreach(accreg_db,inter_accreg_save_sub,fp);
	lock_fclose(fp,accreg_txt,&lock);
//	printf("inter: %s saved.\n",accreg_txt);
	return 0;
}
//--------------------------------------------------------

/*==========================================
 * �ݒ�t�@�C����ǂݍ���
 *------------------------------------------
 */
int inter_config_read(const char *cfgName)
{
	int i;
	char line[1024],w1[1024],w2[1024];
	FILE *fp;

	fp=fopen(cfgName,"r");
	if(fp==NULL){
		printf("file not found: %s\n",cfgName);
		return 1;
	}
	while(fgets(line,1020,fp)){
		i=sscanf(line,"%[^:]: %[^\r\n]",w1,w2);
		if(i!=2)
			continue;
		if(strcmpi(w1,"storage_txt")==0){
			strncpy(storage_txt,w2,sizeof(storage_txt));
		}
		else if(strcmpi(w1,"party_txt")==0){
			strncpy(party_txt,w2,sizeof(party_txt));
		}
		else if(strcmpi(w1,"guild_txt")==0){
			strncpy(guild_txt,w2,sizeof(guild_txt));
		}
		else if(strcmpi(w1,"pet_txt")==0){
			strncpy(pet_txt,w2,sizeof(pet_txt));
		}
		else if(strcmpi(w1,"castle_txt")==0){
			strncpy(castle_txt,w2,sizeof(castle_txt));
		}
		else if(strcmpi(w1,"accreg_txt")==0){
			strncpy(accreg_txt,w2,sizeof(accreg_txt));
		}
		else if(strcmpi(w1,"guild_storage_txt")==0){
			strncpy(guild_storage_txt,w2,sizeof(guild_storage_txt));
		}
		else if(strcmpi(w1,"party_share_level")==0){
			party_share_level=atoi(w2);
			if(party_share_level < 0) party_share_level = 0;
		}
		else if(strcmpi(w1,"inter_log_filename")==0){
			strncpy(inter_log_filename,w2,1024);
		}
		else if(strcmpi(w1,"import")==0){
			inter_config_read(w2);
		}
	}
	fclose(fp);

	return 0;
}

// ���O�����o��
int inter_log(char *fmt,...)
{
	FILE *logfp;
	va_list ap;
	va_start(ap,fmt);
	logfp=fopen(inter_log_filename,"a");
	if(logfp){
		vfprintf(logfp,fmt,ap);
		fclose(logfp);
	}
	va_end(ap);
	return 0;
}

// �Z�[�u
int inter_save()
{
	inter_party_save();
	inter_guild_save();
	inter_storage_save();
	inter_guild_storage_save();
	inter_pet_save();
	inter_accreg_save();

	return 0;
}

// ������
int inter_init(const char *file)
{
	inter_config_read(file);

	wis_db = numdb_init();

	inter_party_init();
	inter_guild_init();
	inter_storage_init();
	inter_pet_init();
	inter_accreg_init();

	return 0;
}

// �}�b�v�T�[�o�[�ڑ�
int inter_mapif_init(int fd)
{
	inter_guild_mapif_init(fd);
	return 0;
}

//--------------------------------------------------------

// GM���b�Z�[�W���M
int mapif_GMmessage(unsigned char *mes,int len)
{
	unsigned char *buf = (unsigned char*)malloc(len);
	WBUFW(buf,0)=0x3800;
	WBUFW(buf,2)=len;
	memcpy(WBUFP(buf,4),mes,len-4);
	mapif_sendall(buf,len);
//	printf("inter server: GM:%d %s\n",len,mes);
	free(buf);
	return 0;
}

// Wis���M
int mapif_wis_message(struct WisData *wd)
{
	unsigned char buf[1024];
	WBUFW(buf, 0)=0x3801;
	WBUFW(buf, 2)=8 + 48 +wd->len;
	WBUFL(buf, 4)=wd->id;
	memcpy(WBUFP(buf, 8),wd->src,24);
	memcpy(WBUFP(buf,32),wd->dst,24);
	memcpy(WBUFP(buf,56),wd->msg,wd->len);
	wd->count = mapif_sendall(buf,WBUFW(buf,2));
	
	return 0;
}
// Wis���M����
int mapif_wis_end(struct WisData *wd,int flag)
{
	unsigned char buf[32];
	
	WBUFW(buf, 0)=0x3802;
	memcpy(WBUFP(buf, 2),wd->src,24);
	WBUFB(buf,26)=flag;
	mapif_send(wd->fd,buf,27);
//	printf("inter server wis_end %d\n",flag);
	return 0;
}

// �A�J�E���g�ϐ����M
int mapif_account_reg(int fd,unsigned char *src)
{
	unsigned char buf[4096];
	memcpy(WBUFP(buf,0),src,WBUFW(src,2));
	WBUFW(buf, 0)=0x3804;
	mapif_sendallwos(fd,buf,WBUFW(buf,2));
	return 0;
}
// �A�J�E���g�ϐ��v���ԐM
int mapif_account_reg_reply(int fd,int account_id)
{
	struct accreg *reg=numdb_search(accreg_db,account_id);
	WFIFOW(fd,0)=0x3804;
	WFIFOL(fd,4)=account_id;
	if(reg==NULL){
		WFIFOW(fd,2)=8;
	}else{
		int j,p;
		for(j=0,p=8;j<reg->reg_num;j++,p+=36){
			memcpy(WFIFOP(fd,p),reg->reg[j].str,32);
			WFIFOL(fd,p+32)=reg->reg[j].value;
		}
		WFIFOW(fd,2)=p;
	}
	WFIFOSET(fd,WFIFOW(fd,2));
	return 0;
}

//--------------------------------------------------------

// GM���b�Z�[�W���M
int mapif_parse_GMmessage(int fd)
{
	mapif_GMmessage(RFIFOP(fd,4),RFIFOW(fd,2));
	return 0;
}


// Wis���M�v��
int mapif_parse_WisRequest(int fd)
{
	struct WisData* wd;
	static int wisid=0;
	
	if( RFIFOW(fd,2)-52 >= sizeof(wd->msg) ){
		printf("inter: Wis message size too long.\n");
		return 0;
	}
	
	wd = (struct WisData *)aCalloc(1,sizeof(struct WisData));
	check_ttl_wisdata();
	
	wd->id = ++wisid;
	wd->fd = fd;
	wd->len= RFIFOW(fd,2)-52;
	memcpy(wd->src, RFIFOP(fd, 4), 24);
	memcpy(wd->dst, RFIFOP(fd,28), 24);
	memcpy(wd->msg, RFIFOP(fd,52), wd->len);
	wd->tick = gettick();
	numdb_insert(wis_db, wd->id, wd);
	mapif_wis_message(wd);

	return 0;
}

// Wis���M����
int mapif_parse_WisReply(int fd)
{
	int id=RFIFOL(fd,2),flag=RFIFOB(fd,6);
	
	struct WisData *wd = numdb_search(wis_db, id);
	
	if(wd==NULL)
		return 0;	// �^�C���A�E�g������ID�����݂��Ȃ�
	
	if( (--wd->count)==0 || flag!=1){
		mapif_wis_end(wd,flag);
		numdb_erase(wis_db, id);
		free(wd);
	}
	return 0;
}

// �A�J�E���g�ϐ��ۑ��v��
int mapif_parse_AccReg(int fd)
{
	int j,p;
	struct accreg *reg=numdb_search(accreg_db,RFIFOL(fd,4));
	if(reg==NULL){
		reg=(struct accreg *)aCalloc(1,sizeof(struct accreg));
		reg->account_id=RFIFOL(fd,4);
		numdb_insert(accreg_db,RFIFOL(fd,4),reg);
	}
	
	for(j=0,p=8;j<ACCOUNT_REG_NUM && p<RFIFOW(fd,2);j++,p+=36){
		memcpy(reg->reg[j].str,RFIFOP(fd,p),32);
		reg->reg[j].value=RFIFOL(fd,p+32);
	}
	reg->reg_num=j;
	
	inter_accreg_save();	// �ۑ�
	mapif_account_reg(fd,RFIFOP(fd,0));	// ����MAP�T�[�o�[�ɑ��M
	return 0;
}
// �A�J�E���g�ϐ����M�v��
int mapif_parse_AccRegRequest(int fd)
{
//	printf("mapif: accreg request\n");
	return mapif_account_reg_reply(fd,RFIFOL(fd,2));
}
/*==========================================
 * �L�����N�^�[�̈ʒu�v��
 *------------------------------------------
 */
int mapif_parse_CharPosReq(int fd)
{
	unsigned char buf[40];
	WBUFW(buf,0)=0x3890;
	memcpy(WBUFP(buf,2),RFIFOP(fd,2),29);
	mapif_sendall(buf,31);
	//printf("mapif_parse_CharPosReq: %d %s\n",RFIFOL(fd,2),RFIFOP(fd,6));
	return 0;
}
/*==========================================
 * �L�����N�^�[�̈ʒu��v���҂ɒʒm
 *------------------------------------------
 */
int mapif_parse_CharPos(int fd)
{
	unsigned char buf[60];
	WBUFW(buf,0)=0x3891;
	memcpy(WBUFP(buf,2),RFIFOP(fd,2),49);
	mapif_sendall(buf,51);
	//printf("mapif_parse_CharPos: %d %s %s %d %d\n",RFIFOL(fd,2),RFIFOP(fd,6),RFIFOP(fd,31),RFIFOW(fd,47),RFIFOW(fd,49));
	return 0;
}
/*==========================================
 * �L�����N�^�[�̈ړ��v��
 *------------------------------------------
 */
int mapif_parse_CharMoveReq(int fd)
{
	unsigned char buf[60];
	WBUFW(buf,0)=0x3892;
	memcpy(WBUFP(buf,2),RFIFOP(fd,2),49);
	mapif_sendall(buf,51);
	//printf("mapif_parse_CharMoveReq: %d %s %s %d %d\n",RFIFOL(fd,2),RFIFOP(fd,6),RFIFOP(fd,31),RFIFOW(fd,47),RFIFOW(fd,49));
	return 0;
}
/*==========================================
 * �Ώ�ID�Ƀ��b�Z�[�W�𑗐M
 *------------------------------------------
 */
int mapif_parse_DisplayMessage(int fd)
{
	int len=RFIFOW(fd,2);
	unsigned char *buf = (unsigned char*)malloc(len);
	
	WBUFW(buf,0)=0x3893;
	memcpy(WBUFP(buf,2),RFIFOP(fd,2),len-2);
	mapif_sendall(buf,len);
	free(buf);
	return 0;
}
//--------------------------------------------------------

// map server ����̒ʐM�i�P�p�P�b�g�̂݉�͂��邱�Ɓj
// �G���[�Ȃ�0(false)�A�����ł����Ȃ�1�A
// �p�P�b�g��������Ȃ����2���������Ȃ���΂Ȃ�Ȃ�
int inter_parse_frommap(int fd)
{
	int cmd=RFIFOW(fd,0);
	int len=0;

	// inter�I�Ǌ����𒲂ׂ�
	if(cmd<0x3000 || cmd>=0x3000+( sizeof(inter_recv_packet_length)/
		sizeof(inter_recv_packet_length[0]) ) )
		return 0;

	// �p�P�b�g���𒲂ׂ�
	if(	(len=inter_check_length(fd,inter_recv_packet_length[cmd-0x3000]))==0 )
		return 2;
	
	switch(cmd){
	case 0x3000: mapif_parse_GMmessage(fd); break;
	case 0x3001: mapif_parse_WisRequest(fd); break;
	case 0x3002: mapif_parse_WisReply(fd); break;
	case 0x3004: mapif_parse_AccReg(fd); break;
	case 0x3005: mapif_parse_AccRegRequest(fd); break;
	case 0x3090: mapif_parse_CharPosReq(fd); break;
	case 0x3091: mapif_parse_CharPos(fd); break;
	case 0x3092: mapif_parse_CharMoveReq(fd); break;
	case 0x3093: mapif_parse_DisplayMessage(fd); break;
	default:
		if( inter_party_parse_frommap(fd) )
			break;
		if( inter_guild_parse_frommap(fd) )
			break;
		if( inter_storage_parse_frommap(fd) )
			break;
		if( inter_pet_parse_frommap(fd) )
			break;
		return 0;
	}
	RFIFOSKIP(fd, len );
	return 1;
}

// RFIFO�̃p�P�b�g���m�F
// �K�v�p�P�b�g��������΃p�P�b�g���A�܂�����Ȃ����0
int inter_check_length(int fd,int length)
{
	if(length==-1){	// �σp�P�b�g��
		if(RFIFOREST(fd)<4)	// �p�P�b�g��������
			return 0;
		length = RFIFOW(fd,2);
	}
	
	if(RFIFOREST(fd)<length)	// �p�P�b�g������
		return 0;
	
	return length;
}

static int wis_db_final(void *key,void *data,va_list ap)
{
	struct WisData *wd=data;

	free(wd);

	return 0;
}
static int accreg_db_final(void *key,void *data,va_list ap)
{
	struct accreg *reg=data;

	free(reg);

	return 0;
}

void do_final_inter(void)
{
	if(wis_db)
		numdb_final(wis_db,wis_db_final);
	if(accreg_db)
		numdb_final(accreg_db,accreg_db_final);

}
