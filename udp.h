#include <string.h>
struct UDPMessage{
	int idMes,idOrig,idDest;
	char mess[101];
};

char* UDPMessageToStr (struct UDPMessage m){
	int len = 3*sizeof(int)+101*sizeof(char);
	char *apstr = malloc(len);
	//snprintf (apstr, len+1, "%d-%d-%d-%d-%d-%s", m.id,m.fromPORT, m.fromIP, m.toPORT, m.toIP, m.mess);
	snprintf (apstr, len+1, "%d\n%d\n%d\n%s", m.idMes,m.idOrig, m.idDest, m.mess); printf(">>%s<<", apstr);
	return apstr;
}
void StrToUDPMessage (char *a, struct UDPMessage *m){
	m->idMes = atoi(strtok (a,"\n"));
	m->idOrig = atoi(strtok (a,"\n"));
	m->idDest = atoi(strtok (a,"\n"));
	snprintf(m->mess, 101, "%s", strtok (a,"\0"));
}
