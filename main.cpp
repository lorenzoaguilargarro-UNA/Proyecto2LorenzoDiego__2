#include "Juego.h"
#include "Registro.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
using namespace std;

int main() {
    try {
        filesystem::create_directories("salida");
        Registro::getInstancia().setRutaArchivo("salida/bitacora.txt");

        cout << "========================================\n";
        cout << "   AVENTURA EN LA MAZMORRA\n";
        cout << "========================================\n";

        cout << "\nSelecciona modo de juego:\n";
        cout << "  1 - Automatico (la simulacion corre sola)\n";
        cout << "  2 - Interactivo (tu controlas al personaje)\n";
        cout << "> ";
        string opcion;
        getline(cin, opcion);
        opcion.erase(0, opcion.find_first_not_of(" \t"));
        opcion.erase(opcion.find_last_not_of(" \t") + 1);

        string nombreJugador;
        cout << "\nIngresa el nombre de tu personaje: ";
        getline(cin, nombreJugador);
        nombreJugador.erase(0, nombreJugador.find_first_not_of(" \t"));
        nombreJugador.erase(nombreJugador.find_last_not_of(" \t") + 1);
        if (nombreJugador.empty()) nombreJugador = "Aventurero";

        Juego juego;
        juego.setRutaReporte("salida/reporte.txt");
        juego.setNombreJugador(nombreJugador);
        juego.cargarMundo(
            "datos/salas.txt",
            "datos/objetos.txt",
            "datos/enemigos.txt",
            "datos/jugador.txt"
        );

        if (opcion == "1")
            juego.ejecutarAutomatico();
        else
            juego.ejecutar();

    } catch (const exception& ex) {
        cerr << "[ERROR FATAL] " << ex.what() << "\n";
        return 1;
    }
    return 0;
}