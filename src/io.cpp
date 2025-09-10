#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstring>
#include "libs/domain.h"
#include "libs/utils.h"
using namespace std;

void insertionSortPorFecha(Medicion* a, int n){
    for(int i=1;i<n;i++){
        Medicion key = a[i];
        int j = i-1;
        while(j>=0 && fechaMayor(a[j].fechaHora, key.fechaHora)){
            a[j+1] = a[j];
            j--;
        }
        a[j+1] = key;
    }
}

bool leerConfig(const char* ruta, Configuracion& cfg){
    ifstream in(ruta);
    if(!in.is_open()) 
    return false;
    cfg.tMin=0; cfg.tMax=100; cfg.oMin=0; cfg.oMax=100;
    cfg.pSisMin=0; cfg.pSisMax=300; cfg.pDiaMin=0; cfg.pDiaMax=200;
    cfg.eMin=-10; cfg.eMax=10;
    string linea;
    while(getline(in, linea)){
        if(linea.empty() || linea[0]=='#') continue;
        istringstream ss(linea);
        string tipo, smin, smax;
        if(!getline(ss, tipo, ',')) continue;
        getline(ss, smin, ','); getline(ss, smax, ',');
        double mn=atof(smin.c_str()), mx=atof(smax.c_str());
        if(tipo=="T"){ cfg.tMin=mn; cfg.tMax=mx; }
        else if(tipo=="O"){ cfg.oMin=mn; cfg.oMax=mx; }
        else if(tipo=="P_SIS"){ cfg.pSisMin=mn; cfg.pSisMax=mx; }
        else if(tipo=="P_DIA"){ cfg.pDiaMin=mn; cfg.pDiaMax=mx; }
        else if(tipo=="E"){ cfg.eMin=mn; cfg.eMax=mx; }
    }
    return true;
}

bool leerPacientes(const char* ruta, Paciente*& arr, int& n){
    ifstream in(ruta);
    if(!in.is_open()) return false;
    n=0; string line;
    while(getline(in,line)){ if(line.empty()||line[0]=='#') continue; ++n; }
    if(n==0){ arr=nullptr; return true; }
    in.clear(); in.seekg(0);
    arr = new Paciente[n];
    int i=0;
    while(getline(in,line)){
        if(line.empty()||line[0]=='#') continue;
        istringstream ss(line);
        string campo;
        if(!getline(ss,campo,';')) continue;
        arr[i].id = atoi(campo.c_str());
        getline(ss,campo,';'); 
           strcpy(arr[i].tipoDocumento,campo.c_str());
        getline(ss,campo,';'); 
           strcpy(arr[i].documento,campo.c_str());
        getline(ss,campo,';'); 
           strcpy(arr[i].nombres,campo.c_str());
        getline(ss,campo,';'); 
           strcpy(arr[i].apellidos,campo.c_str());
        getline(ss,campo,';'); 
           strcpy(arr[i].fechaNacimiento,campo.c_str());
        getline(ss,campo,';');
           strcpy(arr[i].telefono,campo.c_str());
        getline(ss,campo,';'); 
          strcpy(arr[i].email,campo.c_str());
        getline(ss,campo,';');
           strcpy(arr[i].tipoSangre,campo.c_str());
        getline(ss,campo,';'); 
           strcpy(arr[i].entidadSalud,campo.c_str());
        getline(ss,campo,';');
           strcpy(arr[i].medicinaPrepagada,campo.c_str());
        ++i; if(i>=n) break;
    }
    n=i; return true;
}


bool leerBSF(const char* ruta, SalaUCI& s) {
    s.numMaquinas = 0;
    s.maquinas = nullptr;

    ifstream in(ruta, ios::binary);
    if (!in.is_open()) return false;

    if (!in.read(reinterpret_cast<char*>(&s.idSala), 1)) return false;
    if (!in.read(reinterpret_cast<char*>(&s.numMaquinas), 1)) return false;

    s.maquinas = (s.numMaquinas ? new (nothrow) Maquina[s.numMaquinas] : nullptr);
    if (s.numMaquinas && !s.maquinas) { return false; }

    for (uint8_t i = 0; i < s.numMaquinas; ++i) {
        Maquina& m = s.maquinas[i];

        if (!in.read(reinterpret_cast<char*>(&m.id), 1)) { liberarSala(s); return false; }
        if (!in.read(reinterpret_cast<char*>(&m.numMediciones), 4)) { liberarSala(s); return false; }

        m.mediciones = (m.numMediciones ? new (nothrow) Medicion[m.numMediciones] : nullptr);
        if (m.numMediciones && !m.mediciones) { liberarSala(s); return false; }

        for (uint32_t j = 0; j < m.numMediciones; ++j) {
            Medicion& me = m.mediciones[j];

            if (!in.read(me.idPaciente, ID_LEN)) { liberarSala(s); return false; }
            me.idPaciente[10] = '\0';

            if (!in.read(me.fechaHora, FECHA_LEN)) { liberarSala(s); return false; }
            me.fechaHora[FECHA_LEN] = '\0';

            if (!in.read(reinterpret_cast<char*>(&me.numLecturas), 4)) { liberarSala(s); return false; }

            me.lecturas = (me.numLecturas ? new (nothrow) LecturaSensor[me.numLecturas] : nullptr);
            if (me.numLecturas && !me.lecturas) { liberarSala(s); return false; }

            for (uint32_t k = 0; k < me.numLecturas; ++k) {
                LecturaSensor& L = me.lecturas[k];

                if (!in.read(reinterpret_cast<char*>(&L.tipo), 1)) { liberarSala(s); return false; }

                if (L.tipo == 'P') {
                    uint32_t sis = 0, dia = 0;
                    if (!in.read(reinterpret_cast<char*>(&sis), 4)) { liberarSala(s); return false; }
                    if (!in.read(reinterpret_cast<char*>(&dia), 4)) { liberarSala(s); return false; }
                    L.v1 = static_cast<double>(sis);
                    L.v2 = static_cast<double>(dia);
                } else {
                    double v = 0.0;
                    if (!in.read(reinterpret_cast<char*>(&v), 8)) { liberarSala(s); return false; }
                    L.v1 = v;
                    L.v2 = 0.0;
                }
            }
        }
    }

    return true;
}


void liberarSala(SalaUCI& s){
    if(!s.maquinas) return;
    for(uint8_t i=0;i<s.numMaquinas;++i){
        Maquina& m = s.maquinas[i];
        if(m.mediciones){
            for(uint32_t j=0;j<m.numMediciones;++j){
                if(m.mediciones[j].lecturas){
                    delete[] m.mediciones[j].lecturas;
                    m.mediciones[j].lecturas=nullptr;
                }
            }
            delete[] m.mediciones; m.mediciones=nullptr;
        }
    }
    delete[] s.maquinas; s.maquinas=nullptr; s.numMaquinas=0;
}

bool reportePaciente(const SalaUCI& s, const Configuracion& cfg, const char* id10){
    int total=0;
    for(uint8_t im=0; im<s.numMaquinas; ++im){
        const Maquina& M = s.maquinas[im];
        for(uint32_t j=0;j<M.numMediciones;++j)
            if(strncmp(M.mediciones[j].idPaciente,id10,10)==0) total++;
    }
    if(total==0) return false;
    Medicion* vec = new Medicion[total];
    int t=0;
    for(uint8_t im=0; im<s.numMaquinas; ++im){
        const Maquina& M = s.maquinas[im];
        for(uint32_t j=0;j<M.numMediciones;++j){
            const Medicion& me = M.mediciones[j];
            if(strncmp(me.idPaciente,id10,10)==0){ vec[t]=me; t++; }
        }
    }
    insertionSortPorFecha(vec, t);
    char nombre[64]; snprintf(nombre,sizeof(nombre),"mediciones_paciente_%.10s.txt", id10);
    ofstream out(nombre); if(!out.is_open()){ delete[] vec; return false; }
    out<<"Paciente: "<<string(id10, id10+10)<<"\n";
    Stats stT, stO, stSis, stDia;
    for(int i=0;i<t;++i){
        const Medicion& me = vec[i];
        for(uint32_t k=0;k<me.numLecturas;++k){
            const LecturaSensor& L = me.lecturas[k];
            bool anom=false;
            if(L.tipo=='T'){ anom=fuera(L.v1,cfg.tMin,cfg.tMax); actualizarStats(stT,L.v1); out<<me.fechaHora<<" T "<<L.v1; }
            else if(L.tipo=='O'){ anom=fuera(L.v1,cfg.oMin,cfg.oMax); actualizarStats(stO,L.v1); out<<me.fechaHora<<" O "<<L.v1; }
            else if(L.tipo=='P'){
                bool a1=fuera(L.v1,cfg.pSisMin,cfg.pSisMax);
                bool a2=fuera(L.v2,cfg.pDiaMin,cfg.pDiaMax);
                anom=(a1||a2);
                actualizarStats(stSis,L.v1); actualizarStats(stDia,L.v2);
                out<<me.fechaHora<<" P "<<(int)L.v1<<"/"<<(int)L.v2;
            }else{
                out<<me.fechaHora<<" "<<L.tipo<<" "<<L.v1;
            }
            out<<" "<<(anom?"ANOM":"OK")<<"\n";
        }
    }
    out<<"T: min="<<stT.min<<" max="<<stT.max<<" prom="<<promedio(stT)<<"\n";
    out<<"O: min="<<stO.min<<" max="<<stO.max<<" prom="<<promedio(stO)<<"\n";
    out<<"P: sis[min="<<stSis.min<<", max="<<stSis.max<<", prom="<<promedio(stSis)
       <<"] dia[min="<<stDia.min<<", max="<<stDia.max<<", prom="<<promedio(stDia)<<"]\n";
    delete[] vec; return true;
}


bool generarAnomalias(const SalaUCI& s, const Configuracion& cfg){
    struct Anomalia { char id[ID_LEN]; char fecha[25]; char tipo; double v1; double v2; };
   
    int total=0;
    for(uint8_t im=0; im<s.numMaquinas; ++im){
        const Maquina& M = s.maquinas[im];
        for(uint32_t j=0;j<M.numMediciones;++j){
            const Medicion& me = M.mediciones[j];
            for(uint32_t k=0;k<me.numLecturas;++k){
                const LecturaSensor& L = me.lecturas[k];
                bool anom=false;
                if(L.tipo=='T') anom = fuera(L.v1,cfg.tMin,cfg.tMax);
                else if(L.tipo=='O') anom = fuera(L.v1,cfg.oMin,cfg.oMax);
                else if(L.tipo=='P'){
                    bool a1=fuera(L.v1,cfg.pSisMin,cfg.pSisMax);
                    bool a2=fuera(L.v2,cfg.pDiaMin,cfg.pDiaMax);
                    anom = (a1||a2);
                } else if(L.tipo=='E') {
                    anom = fuera(L.v1, cfg.eMin, cfg.eMax);
                }
                if(anom) total++;
            }
        }
    }
    if(total==0){ std::ofstream out("anomalias.txt"); return (bool)out.is_open(); }

    Anomalia* A = new Anomalia[total];
    int t=0;
    for(uint8_t im=0; im<s.numMaquinas; ++im){
        const Maquina& M = s.maquinas[im];
        for(uint32_t j=0;j<M.numMediciones;++j){
            const Medicion& me = M.mediciones[j];
            for(uint32_t k=0;k<me.numLecturas;++k){
                const LecturaSensor& L = me.lecturas[k];
                bool anom=false;
                if(L.tipo=='T') anom = fuera(L.v1,cfg.tMin,cfg.tMax);
                else if(L.tipo=='O') anom = fuera(L.v1,cfg.oMin,cfg.oMax);
                else if(L.tipo=='P'){
                    bool a1=fuera(L.v1,cfg.pSisMin,cfg.pSisMax);
                    bool a2=fuera(L.v2,cfg.pDiaMin,cfg.pDiaMax);
                    anom = (a1||a2);
                } else if(L.tipo=='E'){
                    anom = fuera(L.v1, cfg.eMin, cfg.eMax);
                }
                if(anom){
                    strcpy(A[t].id, me.idPaciente);
                    strcpy(A[t].fecha, me.fechaHora);
                    A[t].tipo = L.tipo;
                    A[t].v1 = L.v1; A[t].v2 = L.v2;
                    t++;
                }
            }
        }
    }

    auto mayorQue = [&](const Anomalia& a, const Anomalia& b)->bool{
        int cmp = strncmp(a.id, b.id, 10);
        if(cmp==0){
            return fechaMayor(a.fecha, b.fecha);
        }
        return (cmp>0);
    };
    for(int i=1;i<t;i++){
        Anomalia key=A[i]; int j=i-1;
        while(j>=0 && mayorQue(A[j], key)){ A[j+1]=A[j]; j--; }
        A[j+1]=key;
    }

    std::ofstream out("anomalias.txt");
    if(!out.is_open()){ delete[] A; return false; }
    char current[ID_LEN]; current[0]='\0';
    for(int i=0;i<t;i++){
        if(strncmp(A[i].id, current, 10)!=0){
            strcpy(current, A[i].id);
            out<<"Paciente: "<<std::string(current, current+10)<<"\n";
        }
        out<<A[i].fecha<<" "<<A[i].tipo<<" ";
        if(A[i].tipo=='P') out<<(int)A[i].v1<<"/"<<(int)A[i].v2<<"\n";
        else out<<A[i].v1<<"\n";
    }
    delete[] A;
    return true;
}

bool exportarECG(const SalaUCI& s, const Configuracion&, const char* outBin){
   
    ofstream out(outBin, ios::binary); if(!out.is_open()) return false;
    
    const int MAXIDS=2000; char ids[MAXIDS][ID_LEN]; int nids=0;
    
    auto existe=[&](const char* id){ for(int i=0;i<nids;++i) if(strncmp(ids[i],id,10)==0) return true; return false; };
    
    for(uint8_t im=0; im<s.numMaquinas; ++im){
        const Maquina& M = s.maquinas[im];
        for(uint32_t j=0;j<M.numMediciones;++j){
            const Medicion& me = M.mediciones[j];
            bool anyE=false; for(uint32_t k=0;k<me.numLecturas;++k) if(me.lecturas[k].tipo=='E'){ anyE=true; break; }
            if(anyE && !existe(me.idPaciente) && nids<MAXIDS){ strcpy(ids[nids], me.idPaciente); nids++; }
        }
    }
    for(int u=0; u<nids; ++u){
        uint32_t cnt=0;
        for(uint8_t im=0; im<s.numMaquinas; ++im){
            const Maquina& M = s.maquinas[im];
            for(uint32_t j=0;j<M.numMediciones;++j){
                const Medicion& me = M.mediciones[j];
                if(strncmp(me.idPaciente,ids[u],10)==0){
                    for(uint32_t k=0;k<me.numLecturas;++k)
                        if(me.lecturas[k].tipo=='E') cnt++;
                }
            }
        }
        out.write(ids[u], 11);
        out.write(reinterpret_cast<const char*>(&cnt), 4);
        for(uint8_t im=0; im<s.numMaquinas; ++im){
            const Maquina& M = s.maquinas[im];
            for(uint32_t j=0;j<M.numMediciones;++j){
                const Medicion& me = M.mediciones[j];
                if(strncmp(me.idPaciente,ids[u],10)==0){
                    for(uint32_t k=0;k<me.numLecturas;++k){
                        const LecturaSensor& L = me.lecturas[k];
                        if(L.tipo=='E'){
                            out.write(me.fechaHora, 24);
                            double v=L.v1; out.write(reinterpret_cast<const char*>(&v), sizeof(double));
                        }
                    }
                }
            }
        }
    }
    return true;
}