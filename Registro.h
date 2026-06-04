//
// Created by Diego Paesani on 03/06/2026.
//

#ifndef PROYECTO2LORENZODIEGO_REGISTRO_H
#define PROYECTO2LORENZODIEGO_REGISTRO_H


#pragma once
#include <string>
#include <vector>
#include <fstream>
using namespace std;

// Singleton: registra todos los eventos de la simulacion
class Registro {
private:
    vector<string> entradas;
    string rutaArchivo;
    int turno;

    Registro();
    Registro(const Registro&) = delete;
    Registro& operator=(const Registro&) = delete;

public:
    static Registro& getInstancia();

    void setRutaArchivo(const string& ruta);
    void registrar(const string& mensaje);
    void registrarTurno(const string& mensaje);
    void escribirArchivo() const;
    const vector<string>& getEntradas() const;
    void limpiar();
};


#endif //PROYECTO2LORENZODIEGO_REGISTRO_H