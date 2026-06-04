//
// Created by Diego Paesani on 03/06/2026.
//

#ifndef PROYECTO2LORENZODIEGO_ENEMIGO_H
#define PROYECTO2LORENZODIEGO_ENEMIGO_H


#pragma once
#include "Entidad.h"
using namespace std;

class Enemigo : public Entidad {
private:
    int salud;
    int saludMaxima;
    int poderAtaque;
    int defensa;
    int oroRecompensa;
    bool vivo;

public:
    Enemigo(const string& nombre, const string& descripcion,
            int salud, int poderAtaque, int defensa, int oroRecompensa = 0);

    int getSalud() const;
    int getSaludMaxima() const;
    int getPoderAtaque() const;
    int getDefensa() const;
    int getOroRecompensa() const;
    bool estaVivo() const;

    int recibirDanio(int danioBase);

    string toString() const override;
};


#endif //PROYECTO2LORENZODIEGO_ENEMIGO_H