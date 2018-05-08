#include <string.h>
struct UDPMessage{
	int idMes,idOrig,idDest;
	char mess[101];
};

char* UDPMessageToStr (struct UDPMessage m){
	int len = 3*sizeof(int)+101*sizeof(char);
	char *apstr = malloc(len);
	//snprintf (apstr, len+1, "%d-%d-%d-%d-%d-%s", m.id,m.fromPORT, m.fromIP, m.toPORT, m.toIP, m.mess);
	snprintf (apstr, len+1, "%d;%d;%d;%s;", m.idMes,m.idOrig, m.idDest, m.mess); printf(">>%s<<", apstr);
	return apstr;
}
void StrToUDPMessage (char *a, struct UDPMessage *m){
	int i=0, x[3], v=3;
	char *p;
	while(v--){
		p = &a[i];
		while(a[i]!=';')
			i++;
		a[i]='\0';
		x[v-1] = atoi(p);
		i++;
	}
	m->idMes=x[2]; m->idOrig = x[1]; m->idDest = x[0];
	//m->idOrig = atoi(strtok (NULL,";"));
	//m->idDest = atoi(strtok (NULL,";"));
	//snprintf(m->mess, 101, "%s", strtok (NULL,";"));printf("???%d+%d+%d+%s???", m->idMes, m->idOrig, m->idDest, m->mess);
}
