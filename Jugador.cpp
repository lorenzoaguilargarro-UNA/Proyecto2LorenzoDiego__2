
//
// Created by Diego Paesani on 03/06/2026.
//



#include "Jugador.h"
#include <sstream>
#include <algorithm>
using namespace std;

Jugador::Jugador(const string& nombre, const string& descripcion, int salud, int ataque, int defensa)
    : Entidad(nombre, descripcion),
      salud(salud), saludMaxima(salud),
      ataqueBase(ataque), defensaBase(defensa), oro(0),
      armaEquipada(nullptr), armaduraEquipada(nullptr) {}

int Jugador::getSalud() const                  { return salud; }
int Jugador::getSaludMaxima() const            { return saludMaxima; }
int Jugador::getOro() const                    { return oro; }
const string& Jugador::getIdSalaActual() const { return idSalaActual; }
void Jugador::setIdSalaActual(const string& idSala) { idSalaActual = idSala; }

int Jugador::getAtaqueTotal() const {
    int bonus = armaEquipada ? armaEquipada->getValor() : 0;
    return ataqueBase + bonus;
}

int Jugador::getDefensaTotal() const {
    int bonus = armaduraEquipada ? armaduraEquipada->getValor() : 0;
    return defensaBase + bonus;
}

int Jugador::recibirDanio(int danioBase) {
    int real = max(1, danioBase - getDefensaTotal());
    salud -= real;
    if (salud < 0) salud = 0;
    return real;
}

void Jugador::curar(int cantidad) {
    salud = min(salud + cantidad, saludMaxima);
}

void Jugador::agregarOro(int cantidad) { oro += cantidad; }

void Jugador::recogerObjeto(shared_ptr<Objeto> objeto) {
    inventario.push_back(objeto);
}

bool Jugador::tieneObjeto(const string& nombreObjeto) const {
    for (const auto& o : inventario)
        if (o->getNombre() == nombreObjeto) return true;
    return false;
}

bool Jugador::tieneObjetoTipo(TipoObjeto tipo) const {
    for (const auto& o : inventario)
        if (o->getTipo() == tipo) return true;
    return false;
}

shared_ptr<Objeto> Jugador::getObjetoPorNombre(const string& nombreObjeto) const {
    for (const auto& o : inventario)
        if (o->getNombre() == nombreObjeto) return o;
    return nullptr;
}

const vector<shared_ptr<Objeto>>& Jugador::getInventario() const { return inventario; }

bool Jugador::equiparObjeto(const string& nombreObjeto) {
    auto obj = getObjetoPorNombre(nombreObjeto);
    if (!obj) return false;
    if (obj->getTipo() == TipoObjeto::ARMA)     { armaEquipada = obj;     return true; }
    if (obj->getTipo() == TipoObjeto::ARMADURA) { armaduraEquipada = obj; return true; }
    return false;
}

bool Jugador::estaVivo() const { return salud > 0; }

string Jugador::toString() const {
    ostringstream ss;
    ss << "[" << nombre << "]"
       << " Salud:" << salud << "/" << saludMaxima
       << " ATQ:" << getAtaqueTotal()
       << " DEF:" << getDefensaTotal()
       << " Oro:" << oro;
    if (armaEquipada)     ss << " Arma:[" << armaEquipada->getNombre() << "]";
    if (armaduraEquipada) ss << " Armadura:[" << armaduraEquipada->getNombre() << "]";
    return ss.str();
}

string Jugador::inventarioToString() const {
    if (inventario.empty()) return "  (vacio)";
    ostringstream ss;
    for (const auto& o : inventario)
        ss << "  - " << o->toString() << "\n";
    return ss.str();
}

void Jugador::eliminarObjeto(const string& nombreObjeto) {
    for (auto it = inventario.begin(); it != inventario.end(); ++it) {
        if ((*it)->getNombre() == nombreObjeto) {
            inventario.erase(it);
            return;
        }
    }
}