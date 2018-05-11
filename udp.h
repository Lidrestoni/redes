#include <string.h>
static int messageTotalLen = 3*sizeof(int)+101*sizeof(char);
struct UDPMessage{
	int idMes,idOrig,idDest;
	char mess[101];
};

char* UDPMessageToStr (struct UDPMessage m){
	char *apstr = malloc(messageTotalLen);
	snprintf (apstr, messageTotalLen+1, "%d;%d;%d;%s;", m.idMes,m.idOrig, m.idDest, m.mess); 
	return apstr;
}
void StrToUDPMessage (char *a, struct UDPMessage *m){
	int i=0, x[3], v=3;
	char *p, temp[103];
	while(v--){
		p = &a[i];
		while(a[i]!=';')
			i++;
		a[i]='\0';
		x[v] = atoi(p);
		a[i]=';';
		i++;
	}
	m->idMes=x[2]; m->idOrig = x[1]; m->idDest = x[0];
	int j=0;
	while(a[i]!=';'&&a[i]!='\0')
		temp[j++]=a[i++];
	temp[j]='\0';
	strcpy(m->mess,temp);
}
