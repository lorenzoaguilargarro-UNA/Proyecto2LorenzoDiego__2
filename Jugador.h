//
// Created by Diego Paesani on 03/06/2026.
//


#pragma once
#include "Entidad.h"
#include "Objeto.h"
#include <vector>
#include <memory>
using namespace std;

class Jugador : public Entidad {
private:
    int salud;
    int saludMaxima;
    int ataqueBase;
    int defensaBase;
    int oro;
    vector<shared_ptr<Objeto>> inventario;
    shared_ptr<Objeto> armaEquipada;
    shared_ptr<Objeto> armaduraEquipada;
    string idSalaActual;

public:
    Jugador(const string& nombre, const string& descripcion, int salud, int ataque, int defensa);

    int getSalud() const;
    int getSaludMaxima() const;
    int getOro() const;
    const string& getIdSalaActual() const;
    void setIdSalaActual(const string& idSala);

    int getAtaqueTotal() const;
    int getDefensaTotal() const;

    int recibirDanio(int danioBase);
    void curar(int cantidad);
    void agregarOro(int cantidad);
    void eliminarObjeto(const string& nombreObjeto);
    void recogerObjeto(shared_ptr<Objeto> objeto);
    bool tieneObjeto(const string& nombreObjeto) const;
    bool tieneObjetoTipo(TipoObjeto tipo) const;
    shared_ptr<Objeto> getObjetoPorNombre(const string& nombreObjeto) const;
    const vector<shared_ptr<Objeto>>& getInventario() const;

    bool equiparObjeto(const string& nombreObjeto);
    bool estaVivo() const;

    string toString() const override;
    string inventarioToString() const;
};