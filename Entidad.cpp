//
// Created by Diego Paesani on 03/06/2026.
//

#include "Entidad.h"

#include "Entidad.h"

Entidad::Entidad(const string& nombre, const string& descripcion)
    : nombre(nombre), descripcion(descripcion) {}

const string& Entidad::getNombre() const { return nombre; }
const string& Entidad::getDescripcion() const { return descripcion; }