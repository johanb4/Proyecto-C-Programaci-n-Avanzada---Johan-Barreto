#ifndef DOMAIN_H
#define DOMAIN_H

#include <cstdint>

const int ID_LEN       = 11;   
const int FECHA_LEN    = 24;   
const int NOMBRE_LEN   = 30;
const int APELLIDO_LEN = 30;
const int EMAIL_LEN    = 40;

struct LecturaSensor {
    char   tipo;  
    double v1;  
    double v2;   
};

struct Medicion {
    char     idPaciente[ID_LEN];  
    char     fechaHora[FECHA_LEN]; 
    uint32_t numLecturas;
    LecturaSensor* lecturas;
    Medicion(): idPaciente{0}, fechaHora{0}, numLecturas(0), lecturas(nullptr) {}
};

struct Maquina {
    uint8_t   id;
    uint32_t  numMediciones;
    Medicion* mediciones;
    Maquina(): id(0), numMediciones(0), mediciones(nullptr) {}
};

struct SalaUCI {
    uint8_t  idSala;
    uint8_t  numMaquinas;
    Maquina* maquinas;
    SalaUCI(): idSala(0), numMaquinas(0), maquinas(nullptr) {}
};

struct Umbral { char tipo[8]; double min; double max; };
struct Configuracion {
    double tMin, tMax;
    double pSisMin, pSisMax;
    double pDiaMin, pDiaMax;
    double eMin, eMax;
    double oMin, oMax;
};

struct cosnfiguracion {
    Umbral temperatura;
    Umbral presionSistolica;
    Umbral presionDiastolica;
    Umbral oxigenacion;
    Umbral ecg;
};

struct Paciente {
    int  id;
    char tipoDocumento[3];
    char documento[ID_LEN];
    char nombres[NOMBRE_LEN];
    char apellidos[APELLIDO_LEN];
    char fechaNacimiento[11];
    char telefono[12];
    char email[EMAIL_LEN];
    char tipoSangre[4];
    char entidadSalud[20];
    char medicinaPrepagada[20];
};



#endif
