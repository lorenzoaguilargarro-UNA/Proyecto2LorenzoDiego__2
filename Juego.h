//
// Created by Diego Paesani on 03/06/2026.
//

#pragma once
#include "Mundo.h"
#include "Jugador.h"
#include "Registro.h"
#include <memory>
#include <set>
using namespace std;

enum class EstadoJuego { EN_CURSO, GANADO, PERDIDO };

class Juego {
private:
    Mundo mundo;
    unique_ptr<Jugador> jugador;
    Registro& registro;
    EstadoJuego estado;
    string rutaReporte;
    string nombreJugador;
    set<string> salasVisitadas;

    void manejarMirar();
    void manejarMover(const string& direccion);
    void manejarRecoger(const string& nombreObjeto);
    void manejarEquipar(const string& nombreObjeto);
    void manejarAtacar(const string& nombreEnemigo);
    void manejarUsarPocion();
    void manejarInventario();
    void manejarEstado();
    void manejarAyuda();

    void rondaCombate(shared_ptr<Enemigo> enemigo);
    void verificarCondicionVictoria();
    void generarReporte() const;
    void pausa(int milisegundos = 800);

    static pair<string, string> parsearComando(const string& linea);

public:
    Juego();
    void setNombreJugador(const string& nombre);
    void setRutaReporte(const string& ruta);
    void cargarMundo(const string& rutaSalas, const string& rutaObjetos,
                     const string& rutaEnemigos, const string& rutaJugador);
    void ejecutar();
    void ejecutarAutomatico();
};