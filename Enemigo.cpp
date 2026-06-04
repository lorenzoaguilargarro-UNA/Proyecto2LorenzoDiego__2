//
// Created by Diego Paesani on 03/06/2026.
//

#include "Enemigo.h"
#include <algorithm>
using namespace std;

Enemigo::Enemigo(const string& nombre, const string& descripcion,
                 int salud, int poderAtaque, int defensa, int oroRecompensa)
    : Entidad(nombre, descripcion),
      salud(salud), saludMaxima(salud),
      poderAtaque(poderAtaque), defensa(defensa),
      oroRecompensa(oroRecompensa), vivo(true) {}

int Enemigo::getSalud() const           { return salud; }
int Enemigo::getSaludMaxima() const     { return saludMaxima; }
int Enemigo::getPoderAtaque() const     { return poderAtaque; }
int Enemigo::getDefensa() const         { return defensa; }
int Enemigo::getOroRecompensa() const   { return oroRecompensa; }
bool Enemigo::estaVivo() const          { return vivo; }

int Enemigo::recibirDanio(int danioBase) {
    int real = max(1, danioBase - defensa);
    salud -= real;
    if (salud <= 0) { salud = 0; vivo = false; }
    return real;
}

string Enemigo::toString() const {
    string estado = vivo ? "VIVO" : "DERROTADO";
    return nombre + " [" + estado + "] Salud:" + to_string(salud) +
           "/" + to_string(saludMaxima) +
           " ATQ:" + to_string(poderAtaque) +
           " DEF:" + to_string(defensa) +
           " Oro:" + to_string(oroRecompensa);
}