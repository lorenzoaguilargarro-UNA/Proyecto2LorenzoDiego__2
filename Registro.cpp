//
// Created by Diego Paesani on 03/06/2026.
//

#include "Registro.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
using namespace std;

Registro::Registro() : rutaArchivo("salida/bitacora.txt"), turno(0) {}

Registro& Registro::getInstancia() {
    static Registro instancia;
    return instancia;
}

void Registro::setRutaArchivo(const string& ruta) { rutaArchivo = ruta; }

void Registro::registrar(const string& mensaje) {
    entradas.push_back(mensaje);
    cout << mensaje << "\n";
}

void Registro::registrarTurno(const string& mensaje) {
    ++turno;
    string entrada = "[Turno " + to_string(turno) + "] " + mensaje;
    entradas.push_back(entrada);
    cout << entrada << "\n";
}

void Registro::escribirArchivo() const {
    // Crea la carpeta si no existe
    filesystem::create_directories(
        filesystem::path(rutaArchivo).parent_path()
    );

    ofstream archivo(rutaArchivo);
    if (!archivo.is_open())
        throw runtime_error("No se puede abrir el archivo de bitacora: " + rutaArchivo);

    for (const auto& entrada : entradas)
        archivo << entrada << "\n";

    archivo.close();
}

const vector<string>& Registro::getEntradas() const { return entradas; }

void Registro::limpiar() { entradas.clear(); turno = 0; }