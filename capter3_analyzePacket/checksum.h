u_int16_t checksum(u_char *,int);
u_int16_t checksum2(u_char *,int,u_char *,int);
int checkIPchecksum(struct iphdr *,u_char *,int );
int checkIPDATAchecksum(struct iphdr *,unsigned char *,int );