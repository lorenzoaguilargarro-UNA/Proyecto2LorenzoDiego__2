//
// Created by Diego Paesani on 03/06/2026.
//

#include "Mundo.h"
#include "Registro.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

void Mundo::agregarSala(shared_ptr<Sala> sala) { salas[sala->getId()] = sala; }
void Mundo::setIdSalaInicial(const string& id) { idSalaInicial = id; }
void Mundo::setIdSalaMeta(const string& id)    { idSalaMeta = id; }

shared_ptr<Sala> Mundo::getSala(const string& id) const {
    auto it = salas.find(id);
    if (it == salas.end()) return nullptr;
    return it->second;
}

const string& Mundo::getIdSalaInicial() const { return idSalaInicial; }
const string& Mundo::getIdSalaMeta()    const { return idSalaMeta; }
bool Mundo::existeSala(const string& id) const { return salas.count(id) > 0; }

// Formato salas.txt.txt:
// INICIO:<id>  |  META:<id>
// SALA:<id>|<nombre>|<descripcion>|<bloqueada:0|1>|<llave_o_ninguna>
// SALIDA:<desde>|<direccion>|<hacia>
void Mundo::cargarSalas(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open())
        throw runtime_error("No se puede abrir el archivo de salas.txt: " + ruta);

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        istringstream ss(linea);
        string token; getline(ss, token, ':');

        if (token == "INICIO") {
            string id; getline(ss, id);
            idSalaInicial = id;
        } else if (token == "META") {
            string id; getline(ss, id);
            idSalaMeta = id;
        } else if (token == "SALA") {
            string resto; getline(ss, resto);
            istringstream rs(resto);
            string id, nombre, desc, bloqueadaStr, llave;
            getline(rs, id,          '|');
            getline(rs, nombre,      '|');
            getline(rs, desc,        '|');
            getline(rs, bloqueadaStr,'|');
            getline(rs, llave,       '|');
            bool bloqueada = (bloqueadaStr == "1");
            string llaveReal = (llave == "ninguna") ? "" : llave;
            agregarSala(make_shared<Sala>(id, nombre, desc, bloqueada, llaveReal));
        } else if (token == "SALIDA") {
            string resto; getline(ss, resto);
            istringstream rs(resto);
            string desde, direccion, hacia;
            getline(rs, desde,     '|');
            getline(rs, direccion, '|');
            getline(rs, hacia,     '|');
            auto sala = getSala(desde);
            if (!sala) throw runtime_error("SALIDA referencia sala desconocida: " + desde);
            sala->agregarSalida(direccion, hacia);
        } else {
            throw runtime_error("Token desconocido en archivo de salas.txt: " + token);
        }
    }
    Registro::getInstancia().registrar("[CARGA] Salas cargadas desde " + ruta);
}

// Formato objetos.txt:
// OBJETO:<id_sala>|<nombre>|<descripcion>|<tipo>|<valor>
void Mundo::cargarObjetos(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open())
        throw runtime_error("No se puede abrir el archivo de objetos: " + ruta);

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        istringstream ss(linea);
        string token; getline(ss, token, ':');
        if (token != "OBJETO") throw runtime_error("Se esperaba OBJETO, se obtuvo: " + token);

        string resto; getline(ss, resto);
        istringstream rs(resto);
        string idSala, nombre, desc, tipoStr, valorStr;
        getline(rs, idSala,   '|');
        getline(rs, nombre,   '|');
        getline(rs, desc,     '|');
        getline(rs, tipoStr,  '|');
        getline(rs, valorStr, '|');

        auto objeto = make_shared<Objeto>(nombre, desc, Objeto::stringATipo(tipoStr), stoi(valorStr));
        auto sala = getSala(idSala);
        if (!sala) throw runtime_error("Objeto referencia sala desconocida: " + idSala);
        sala->agregarObjeto(objeto);
    }
    Registro::getInstancia().registrar("[CARGA] Objetos cargados desde " + ruta);
}

// Formato enemigos.txt:
// ENEMIGO:<id_sala>|<nombre>|<descripcion>|<salud>|<ataque>|<defensa>|<oro>
void Mundo::cargarEnemigos(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open())
        throw runtime_error("No se puede abrir el archivo de enemigos: " + ruta);

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        istringstream ss(linea);
        string token; getline(ss, token, ':');
        if (token != "ENEMIGO") throw runtime_error("Se esperaba ENEMIGO, se obtuvo: " + token);

        string resto; getline(ss, resto);
        istringstream rs(resto);
        string idSala, nombre, desc, saludStr, ataqueStr, defensaStr, oroStr;
        getline(rs, idSala,     '|');
        getline(rs, nombre,     '|');
        getline(rs, desc,       '|');
        getline(rs, saludStr,   '|');
        getline(rs, ataqueStr,  '|');
        getline(rs, defensaStr, '|');
        getline(rs, oroStr,     '|');

        int oro = oroStr.empty() ? 0 : stoi(oroStr);

        auto enemigo = make_shared<Enemigo>(
            nombre, desc, stoi(saludStr), stoi(ataqueStr), stoi(defensaStr), oro);
        auto sala = getSala(idSala);
        if (!sala) throw runtime_error("Enemigo referencia sala desconocida: " + idSala);
        sala->agregarEnemigo(enemigo);
    }
    Registro::getInstancia().registrar("[CARGA] Enemigos cargados desde " + ruta);
}

string Mundo::listarSalas() const {
    ostringstream ss;
    for (const auto& [id, sala] : salas)
        ss << "  " << sala->toString() << "\n";
    return ss.str();
}