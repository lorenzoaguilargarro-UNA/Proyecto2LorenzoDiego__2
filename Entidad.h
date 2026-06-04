//
// Created by Diego Paesani on 03/06/2026.
//

#ifndef PROYECTO2LORENZODIEGO_ENTIDAD_H
#define PROYECTO2LORENZODIEGO_ENTIDAD_H


#pragma once
#include <string>
using namespace std;

class Entidad {
protected:
    string nombre;
    string descripcion;

public:
    Entidad(const string& nombre, const string& descripcion);
    virtual ~Entidad() = default;

    const string& getNombre() const;
    const string& getDescripcion() const;

    virtual string toString() const = 0;
};

#endif //PROYECTO2LORENZODIEGO_ENTIDAD_H