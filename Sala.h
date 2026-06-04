//
// Created by Diego Paesani on 03/06/2026.
//

#ifndef PROYECTO2LORENZODIEGO_SALA_H
#define PROYECTO2LORENZODIEGO_SALA_H


#pragma once
#include "Entidad.h"
#include "Objeto.h"
#include "Enemigo.h"
#include <vector>
#include <map>
#include <memory>
using namespace std;

class Sala : public Entidad {
private:
    string id;
    vector<shared_ptr<Objeto>> objetos;
    vector<shared_ptr<Enemigo>> enemigos;
    map<string, string> salidas;
    bool bloqueada;
    string nombreLlaveRequerida;

public:
    Sala(const string& id, const string& nombre, const string& descripcion,
         bool bloqueada = false, const string& nombreLlaveRequerida = "");

    const string& getId() const;
    bool estaBloqueada() const;
    const string& getNombreLlaveRequerida() const;
    void desbloquear();

    void agregarObjeto(shared_ptr<Objeto> objeto);
    void agregarEnemigo(shared_ptr<Enemigo> enemigo);
    void agregarSalida(const string& direccion, const string& idSala);

    shared_ptr<Objeto> tomarObjeto(const string& nombreObjeto);

    const vector<shared_ptr<Objeto>>& getObjetos() const;
    const vector<shared_ptr<Enemigo>>& getEnemigos() const;
    const map<string, string>& getSalidas() const;

    bool tieneEnemigosVivos() const;

    string toString() const override;
    string toStringDetallado() const;
};


#endif //PROYECTO2LORENZODIEGO_SALA_H