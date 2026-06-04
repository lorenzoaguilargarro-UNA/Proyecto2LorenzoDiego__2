//
// Created by Diego Paesani on 03/06/2026.
//

#ifndef PROYECTO2LORENZODIEGO_OBJETO_H
#define PROYECTO2LORENZODIEGO_OBJETO_H


#pragma once
#include "Entidad.h"
#include <stdexcept>
using namespace std;

enum class TipoObjeto { ARMA, POCION, LLAVE, TESORO, ARMADURA };

class Objeto : public Entidad {
private:
    TipoObjeto tipo;
    int valor;

public:
    Objeto(const string& nombre, const string& descripcion, TipoObjeto tipo, int valor);

    TipoObjeto getTipo() const;
    int getValor() const;

    bool operator>(const Objeto& otro) const;
    bool operator==(const Objeto& otro) const;

    string toString() const override;
    static string tipoAString(TipoObjeto t);
    static TipoObjeto stringATipo(const string& s);
};

#endif //PROYECTO2LORENZODIEGO_OBJETO_H