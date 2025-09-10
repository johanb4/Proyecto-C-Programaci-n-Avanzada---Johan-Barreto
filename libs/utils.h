#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <cstring>

inline void safe_copy(char* dst, int cap, const char* src){
    if (!dst || cap <= 0) return;
    if (!src) { dst[0] = '\0'; return; }
    int n=0; while (src[n] && n < cap-1) { dst[n]=src[n]; ++n; }
    dst[n]='\0';
}

// Asume formato EXACTO "DD/MM/AAAA HH:MM:ss.sss" (24 bytes en el .bsf, + '\0' en memoria)
// Compara por posiciones fijas: AAAA, MM, DD, HH, MM, ss, sss.
inline bool fechaMayor(const char* f1, const char* f2){
    auto A = atoi(f1+6), A2 = atoi(f2+6);
    if(A!=A2) return A>A2;
    char buf[5];
    buf[0]=f1[3]; buf[1]=f1[4]; buf[2]='\0'; int M=atoi(buf);
    buf[0]=f2[3]; buf[1]=f2[4]; buf[2]='\0'; int M2=atoi(buf);
    if(M!=M2) return M>M2;
    buf[0]=f1[0]; buf[1]=f1[1]; buf[2]='\0'; int D=atoi(buf);
    buf[0]=f2[0]; buf[1]=f2[1]; buf[2]='\0'; int D2=atoi(buf);
    if(D!=D2) return D>D2;
    buf[0]=f1[11]; buf[1]=f1[12]; buf[2]='\0'; int h=atoi(buf);
    buf[0]=f2[11]; buf[1]=f2[12]; buf[2]='\0'; int h2=atoi(buf);
    if(h!=h2) return h>h2;
    buf[0]=f1[14]; buf[1]=f1[15]; buf[2]='\0'; int m=atoi(buf);
    buf[0]=f2[14]; buf[1]=f2[15]; buf[2]='\0'; int m2=atoi(buf);
    if(m!=m2) return m>m2;
    buf[0]=f1[17]; buf[1]=f1[18]; buf[2]='\0'; int s=atoi(buf);
    buf[0]=f2[17]; buf[1]=f2[18]; buf[2]='\0'; int s2=atoi(buf);
    if(s!=s2) return s>s2;
    buf[0]=f1[20]; buf[1]=f1[21]; buf[2]=f1[22]; buf[3]='\0'; int ms=atoi(buf);
    buf[0]=f2[20]; buf[1]=f2[21]; buf[2]=f2[22]; buf[3]='\0'; int ms2=atoi(buf);
    return ms>ms2;
}

struct Stats { double min,max,sum; int n; Stats():min(1e300),max(-1e300),sum(0),n(0){} };
inline void actualizarStats(Stats& s, double v){ if(v<s.min) s.min=v; if(v>s.max) s.max=v; s.sum+=v; s.n++; }
inline double promedio(const Stats& s){ return (s.n>0? s.sum/s.n:0.0); }
inline bool fuera(double v,double lo,double hi){ return v<lo||v>hi; }

#endif
