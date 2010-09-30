#include <stdio.h>

FILE *in, *out;
 int adr,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15,b16,
        b17,b18,b19,b20,b21,b22,b23,b24,b25,b26,b27,b28,b29,b30,b31,b32,cs;
 int b[32];

main(int argc,char **argv)
{
    printf("Txt2Bin 1.0\n");
    if (argc!=3) { printf("Usage: txt2bin file.txt outfile\n"); exit(1);}
    in=fopen(argv[1],"rb"); out=fopen(argv[2],"wb");
    if (in==NULL || out==NULL) { printf("Unable to open file\n"); exit(1);}

    printf("GO\n");
    while(fscanf(in,"%04x:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x:%02x",
               &adr,
               &b[0],&b[1],&b[2],&b[3],&b[4],&b[5],&b[6],&b[7],&b[8],&b[9],&b[10],&b[11],&b[12],&b[13],&b[14],&b[15],
               &b[16],&b[17],&b[18],&b[19],&b[20],&b[21],&b[22],&b[23],&b[24],&b[25],&b[26],&b[27],&b[28],&b[29],&b[30],&b[31],
//               &b1,&b2,&b3,&b4,&b5,&b6,&b7,&b8,&b9,&b10,&b11,&b12,&b13,&b14,&b15,&b16,
//               &b17,&b18,&b19,&b20,&b21,&b22,&b23,&b24,&b25,&b26,&b27,&b28,&b29,&b30,&b31,&b32,
               &cs) != EOF)
    {
        int i;
        int s = 0;
        for ( i=0; i<32 ; i++) {
            putc(b[i],out);
            s += b[i];
        }
        int c = s%255;
        printf("%04x:%02x - %02x - %s\n",adr,cs,c,(c!=cs?"***ERROR***":""));


    }
    close(in); close(out);
    exit(0);
}

