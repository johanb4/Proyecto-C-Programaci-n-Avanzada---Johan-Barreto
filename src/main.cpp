#include <iostream>
#include <fstream>
#include <cstring>
#include "libs/domain.h"
#include "libs/utils.h"

bool leerConfig(const char*, Configuracion&);
bool leerPacientes(const char*, Paciente*&, int&);
bool leerBSF(const char*, SalaUCI&);
void liberarSala(SalaUCI&);
bool reportePaciente(const SalaUCI&, const Configuracion&, const char*);
bool generarAnomalias(const SalaUCI&, const Configuracion&);
bool exportarECG(const SalaUCI&, const Configuracion&, const char*);

using namespace std;

int main(){
    Configuracion cfg{};
    Paciente* pacientes=nullptr; int nPac=0;
    SalaUCI sala{}; sala.maquinas=nullptr; sala.numMaquinas=0;

    int op;
    do{
        cout<<"1) Cargar config/pacientes\n2) Leer .bsf\n3) Reporte paciente\n4) Anomalias\n5) Exportar ECG\n6) Salir\n> ";
        if(!(cin>>op)) return 0;
        cin.ignore(1024,'\n');
        if(op==1){
            cout<<(leerConfig("configuracion.txt", cfg)?"Config OK\n":"Config ERR\n");
            cout<<(leerPacientes("pacientes.csv", pacientes, nPac)?"Pacientes OK\n":"Pacientes ERR\n");
        }else if(op==2){
            if(sala.maquinas) liberarSala(sala);
            cout<<(leerBSF("ejemplo.bsf", sala)?"Sala cargada\n":"Error .bsf\n");
        }else if(op==3){
            if(!sala.maquinas){ cout<<"Primero .bsf\n"; continue; }
            char id10[11]; cout<<"ID paciente (10): "; cin.getline(id10,11);
            cout<<"Buscando: ["<<id10<<"]\n";
cout<<(reportePaciente(sala,cfg,id10)?"Reporte generado\n":"Sin datos\n");
        }else if(op==4){
            if(!sala.maquinas){ cout<<"Primero .bsf\n"; continue; }
            cout<<(generarAnomalias(sala,cfg)?"Anomalias generadas\n":"Error\n");
        }else if(op==5){
            if(!sala.maquinas){ cout<<"Primero .bsf\n"; continue; }
            cout<<(exportarECG(sala,cfg,"pacientes_ecg_anomalos.dat")?"Exportado\n":"Error\n");
        }
    }while(op!=6);
    if(sala.maquinas) liberarSala(sala);
    if(pacientes) delete[] pacientes;
    return 0;
}
