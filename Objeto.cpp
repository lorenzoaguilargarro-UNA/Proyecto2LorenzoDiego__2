//
// Created by Diego Paesani on 03/06/2026.
//

#include "Objeto.h"

#include "Objeto.h"

Objeto::Objeto(const string& nombre, const string& descripcion, TipoObjeto tipo, int valor)
    : Entidad(nombre, descripcion), tipo(tipo), valor(valor) {}

TipoObjeto Objeto::getTipo() const { return tipo; }
int Objeto::getValor() const { return valor; }

bool Objeto::operator>(const Objeto& otro) const { return this->valor > otro.valor; }
bool Objeto::operator==(const Objeto& otro) const {
    return this->nombre == otro.nombre && this->tipo == otro.tipo;
}

string Objeto::toString() const {
    return "[" + tipoAString(tipo) + "] " + nombre +
           " (valor: " + to_string(valor) + ") - " + descripcion;
}

string Objeto::tipoAString(TipoObjeto t) {
    switch (t) {
        case TipoObjeto::ARMA:     return "ARMA";
        case TipoObjeto::POCION:   return "POCION";
        case TipoObjeto::LLAVE:    return "LLAVE";
        case TipoObjeto::TESORO:   return "TESORO";
        case TipoObjeto::ARMADURA: return "ARMADURA";
        default:                   return "DESCONOCIDO";
    }
}

TipoObjeto Objeto::stringATipo(const string& s) {
    if (s == "ARMA")     return TipoObjeto::ARMA;
    if (s == "POCION")   return TipoObjeto::POCION;
    if (s == "LLAVE")    return TipoObjeto::LLAVE;
    if (s == "TESORO")   return TipoObjeto::TESORO;
    if (s == "ARMADURA") return TipoObjeto::ARMADURA;
    throw invalid_argument("Tipo de objeto desconocido: " + s);
}