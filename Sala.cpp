//
// Created by Diego Paesani on 03/06/2026.
//

#include "Sala.h"

#include "Sala.h"
#include <sstream>
#include <algorithm>

using namespace std;

Sala::Sala(const string& id, const string& nombre, const string& descripcion,
           bool bloqueada, const string& nombreLlaveRequerida)
    : Entidad(nombre, descripcion), id(id),
      bloqueada(bloqueada), nombreLlaveRequerida(nombreLlaveRequerida) {}

const string& Sala::getId() const                   { return id; }
bool Sala::estaBloqueada() const                    { return bloqueada; }
const string& Sala::getNombreLlaveRequerida() const { return nombreLlaveRequerida; }
void Sala::desbloquear()                            { bloqueada = false; }

void Sala::agregarObjeto(shared_ptr<Objeto> objeto)    { objetos.push_back(objeto); }
void Sala::agregarEnemigo(shared_ptr<Enemigo> enemigo) { enemigos.push_back(enemigo); }
void Sala::agregarSalida(const string& direccion, const string& idSala) {
    salidas[direccion] = idSala;
}

shared_ptr<Objeto> Sala::tomarObjeto(const string& nombreObjeto) {
    // Convertir búsqueda a minúsculas
    string busqueda = nombreObjeto;
    transform(busqueda.begin(), busqueda.end(), busqueda.begin(), ::tolower);

    for (auto it = objetos.begin(); it != objetos.end(); ++it) {
        string nombreObj = (*it)->getNombre();
        transform(nombreObj.begin(), nombreObj.end(), nombreObj.begin(), ::tolower);

        // Acepta nombre exacto o nombre parcial
        if (nombreObj == busqueda || nombreObj.find(busqueda) != string::npos) {
            auto obj = *it;
            objetos.erase(it);
            return obj;
        }
    }
    return nullptr;
}

const vector<shared_ptr<Objeto>>&  Sala::getObjetos()  const { return objetos; }
const vector<shared_ptr<Enemigo>>& Sala::getEnemigos() const { return enemigos; }
const map<string, string>&         Sala::getSalidas()  const { return salidas; }

bool Sala::tieneEnemigosVivos() const {
    for (const auto& e : enemigos)
        if (e->estaVivo()) return true;
    return false;
}

string Sala::toString() const {
    return id + " - " + nombre + (bloqueada ? " [BLOQUEADA]" : "");
}

string Sala::toStringDetallado() const {
    ostringstream ss;
    ss << "=== " << nombre << " ===\n";
    ss << descripcion << "\n";

    if (bloqueada)
        ss << "(Esta sala esta bloqueada. Llave requerida: " << nombreLlaveRequerida << ")\n";

    if (!objetos.empty()) {
        ss << "\nObjetos aqui:\n";
        for (const auto& o : objetos)
            ss << "  - " << o->toString() << "\n";
    }

    if (!enemigos.empty()) {
        ss << "\nEnemigos:\n";
        for (const auto& e : enemigos)
            ss << "  - " << e->toString() << "\n";
    }

    if (!salidas.empty()) {
        ss << "\nSalidas: ";
        for (const auto& [dir, id] : salidas)
            ss << dir << " ";
        ss << "\n";
    }

    return ss.str();
}