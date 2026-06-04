//
// Created by Diego Paesani on 03/06/2026.
//

#ifndef PROYECTO2LORENZODIEGO_MUNDO_H
#define PROYECTO2LORENZODIEGO_MUNDO_H


#pragma once
#include "Sala.h"
#include <map>
#include <memory>
using namespace std;

class Mundo {
private:
    map<string, shared_ptr<Sala>> salas;
    string idSalaInicial;
    string idSalaMeta;

public:
    Mundo() = default;

    void agregarSala(shared_ptr<Sala> sala);
    void setIdSalaInicial(const string& id);
    void setIdSalaMeta(const string& id);

    shared_ptr<Sala> getSala(const string& id) const;
    const string& getIdSalaInicial() const;
    const string& getIdSalaMeta() const;

    bool existeSala(const string& id) const;

    void cargarSalas(const string& ruta);
    void cargarObjetos(const string& ruta);
    void cargarEnemigos(const string& ruta);

    string listarSalas() const;
};


#endif //PROYECTO2LORENZODIEGO_MUNDO_H