//
// Created by Diego Paesani on 03/06/2026.
//

#include "Juego.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <map>
#include <stdexcept>
#include <thread>
#include <chrono>
using namespace std;

// ============================================================
// Constructor e inicializacion
// ============================================================

Juego::Juego()
    : registro(Registro::getInstancia()),
      estado(EstadoJuego::EN_CURSO),
      rutaReporte("salida/reporte.txt") {}

void Juego::setNombreJugador(const string& nombre) {
    nombreJugador = nombre;
}

void Juego::setRutaReporte(const string& ruta) {
    rutaReporte = ruta;
}

// ============================================================
// Carga del mundo desde archivos
// ============================================================

void Juego::cargarMundo(const string& rutaSalas, const string& rutaObjetos,
                        const string& rutaEnemigos, const string& rutaJugador) {
    mundo.cargarSalas(rutaSalas);
    mundo.cargarObjetos(rutaObjetos);
    mundo.cargarEnemigos(rutaEnemigos);

    ifstream arch(rutaJugador);
    if (!arch.is_open())
        throw runtime_error("No se puede abrir el archivo de jugador: " + rutaJugador);

    string nombre, descripcion = "Aventurero sin historia conocida";
    int salud = 100, ataque = 10, defensa = 2;
    string linea;

    while (getline(arch, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        istringstream ss(linea);
        string clave; getline(ss, clave, ':');
        string valor; getline(ss, valor);
        if      (clave == "SALUD")       salud       = stoi(valor);
        else if (clave == "ATAQUE")      ataque      = stoi(valor);
        else if (clave == "DEFENSA")     defensa     = stoi(valor);
        else if (clave == "DESCRIPCION") descripcion = valor;
    }

    string nombreFinal = nombreJugador.empty() ? nombre : nombreJugador;
    jugador = make_unique<Jugador>(nombreFinal, descripcion, salud, ataque, defensa);
    jugador->setIdSalaActual(mundo.getIdSalaInicial());
    registro.registrar("[CARGA] Jugador '" + nombreFinal + "' creado. Sala inicial: " + mundo.getIdSalaInicial());
}

// ============================================================
// Modo interactivo
// ============================================================

void Juego::ejecutar() {
    registro.registrar("\n========================================");
    registro.registrar("   AVENTURA EN LA MAZMORRA - INICIO    ");
    registro.registrar("========================================\n");

    manejarMirar();
    manejarAyuda();

    while (estado == EstadoJuego::EN_CURSO) {
        cout << "\n" << jugador->toString() << "\n> ";
        string entrada;
        if (!getline(cin, entrada)) break;
        if (entrada.empty()) continue;

        auto [verbo, argumento] = parsearComando(entrada);

        try {
            if      (verbo == "mirar")                         manejarMirar();
            else if (verbo == "ir")                            manejarMover(argumento);
            else if (verbo == "recoger" || verbo == "tomar")   manejarRecoger(argumento);
            else if (verbo == "equipar")                       manejarEquipar(argumento);
            else if (verbo == "atacar")                        manejarAtacar(argumento);
            else if (verbo == "pocion" || verbo == "curar")    manejarUsarPocion();
            else if (verbo == "inventario" || verbo == "inv")  manejarInventario();
            else if (verbo == "estado")                        manejarEstado();
            else if (verbo == "ayuda")                         manejarAyuda();
            else if (verbo == "salir") {
                registro.registrar("[EVENTO] El jugador abandono la aventura.");
                estado = EstadoJuego::PERDIDO;
            } else {
                cout << "Comando desconocido. Escribe 'ayuda' para ver los comandos.\n";
            }
        } catch (const exception& ex) {
            cout << "[ERROR] " << ex.what() << "\n";
            registro.registrar("[ERROR] " + string(ex.what()));
        }

        if (!jugador->estaVivo()) {
            registro.registrar("[EVENTO] " + jugador->getNombre() + " ha caido en la mazmorra...");
            estado = EstadoJuego::PERDIDO;
        }
    }

    if (estado == EstadoJuego::GANADO)
        cout << "\n*** VICTORIA! Has escapado de la mazmorra! ***\n";
    else
        cout << "\n*** FIN DEL JUEGO - Tu aventura termina aqui. ***\n";

    registro.registrar(estado == EstadoJuego::GANADO
        ? "\n[RESULTADO] VICTORIA - " + jugador->getNombre() + " escapo de la mazmorra!"
        : "\n[RESULTADO] DERROTA - "  + jugador->getNombre() + " no sobrevivio.");

    generarReporte();
    registro.escribirArchivo();
    cout << "\nBitacora guardada en: salida/bitacora.txt\n";
    cout << "Reporte guardado en: " << rutaReporte << "\n";
}

// ============================================================
// Modo automatico
// ============================================================

void Juego::pausa(int milisegundos) {
    this_thread::sleep_for(chrono::milliseconds(milisegundos));
}

void Juego::ejecutarAutomatico() {
    registro.registrar("\n========================================");
    registro.registrar("   AVENTURA EN LA MAZMORRA - MODO AUTO  ");
    registro.registrar("========================================\n");

    jugador->setIdSalaActual(mundo.getIdSalaInicial());
    salasVisitadas.insert(mundo.getIdSalaInicial());

    cout << "\n[AUTO] Iniciando simulacion automatica...\n";
    pausa(1000);

    int turnosMaximos = 200;
    int turnoActual   = 0;

    while (estado == EstadoJuego::EN_CURSO) {

        if (++turnoActual > turnosMaximos) {
            cout << "[AUTO] Se alcanzo el limite de turnos.\n";
            estado = EstadoJuego::PERDIDO;
            break;
        }

        auto sala = mundo.getSala(jugador->getIdSalaActual());
        cout << "\n" << sala->toStringDetallado();
        pausa(800);

        // 1. Recoger todos los objetos de la sala
        while (!sala->getObjetos().empty()) {
            auto objeto = sala->getObjetos()[0];
            sala->tomarObjeto(objeto->getNombre());
            jugador->recogerObjeto(objeto);
            registro.registrarTurno("[AUTO] Recogio: " + objeto->getNombre());
            cout << "[AUTO] Recogio: " << objeto->getNombre() << "\n";
            if (objeto->getTipo() == TipoObjeto::ARMA || objeto->getTipo() == TipoObjeto::ARMADURA)
                if (jugador->equiparObjeto(objeto->getNombre()))
                    cout << "[AUTO] Equipo: " << objeto->getNombre() << "\n";
            if (objeto->getTipo() == TipoObjeto::TESORO)
                jugador->agregarOro(objeto->getValor());
            pausa(600);
        }

        // 2. Atacar todos los enemigos vivos
        for (const auto& enemigo : sala->getEnemigos()) {
            while (enemigo->estaVivo() && jugador->estaVivo()) {
                rondaCombate(enemigo);
                pausa(800);
                if (estado == EstadoJuego::GANADO) {
                    cout << "\n*** VICTORIA! Has escapado de la mazmorra! ***\n";
                    registro.registrar("\n[RESULTADO] VICTORIA - " + jugador->getNombre() + " escapo de la mazmorra!");
                    generarReporte();
                    registro.escribirArchivo();
                    cout << "\nBitacora guardada en: salida/bitacora.txt\n";
                    cout << "Reporte guardado en: " << rutaReporte << "\n";
                    return;
                }
            }
            if (!jugador->estaVivo()) {
                registro.registrar("[EVENTO] " + jugador->getNombre() + " ha caido en la mazmorra...");
                estado = EstadoJuego::PERDIDO;
                generarReporte();
                registro.escribirArchivo();
                cout << "\n*** FIN DEL JUEGO - Tu aventura termina aqui. ***\n";
                cout << "\nBitacora guardada en: salida/bitacora.txt\n";
                cout << "Reporte guardado en: " << rutaReporte << "\n";
                return;
            }
        }

        // 3. Usar pocion si salud baja del 50%
        if (jugador->getSalud() < jugador->getSaludMaxima() / 2) {
            for (const auto& o : jugador->getInventario()) {
                if (o->getTipo() == TipoObjeto::POCION) {
                    int antes  = jugador->getSalud();
                    jugador->curar(o->getValor());
                    int curado = jugador->getSalud() - antes;
                    registro.registrarTurno("[AUTO] Uso pocion, recupero " + to_string(curado) + " de salud.");
                    cout << "[AUTO] Uso pocion, recupero " << curado << " de salud.\n";
                    jugador->eliminarObjeto(o->getNombre());
                    pausa(600);
                    break;
                }
            }
        }

        // 4. Verificar condicion de victoria
        verificarCondicionVictoria();
        if (estado == EstadoJuego::GANADO) {
            cout << "\n*** VICTORIA! Has escapado de la mazmorra! ***\n";
            registro.registrar("\n[RESULTADO] VICTORIA - " + jugador->getNombre() + " escapo de la mazmorra!");
            generarReporte();
            registro.escribirArchivo();
            cout << "\nBitacora guardada en: salida/bitacora.txt\n";
            cout << "Reporte guardado en: " << rutaReporte << "\n";
            return;
        }

        // 5. Moverse a sala no visitada adyacente
        bool movido = false;
        for (const auto& [dir, idDestino] : sala->getSalidas()) {
            if (salasVisitadas.count(idDestino)) continue;
            auto destino = mundo.getSala(idDestino);
            if (!destino) continue;
            if (destino->estaBloqueada()) {
                const string& llave = destino->getNombreLlaveRequerida();
                if (!jugador->tieneObjeto(llave)) continue;
                destino->desbloquear();
                registro.registrarTurno("[AUTO] Desbloqueo '" + destino->getNombre() + "' con " + llave);
                cout << "[AUTO] Desbloqueo '" << destino->getNombre() << "' con " << llave << "\n";
            }
            jugador->setIdSalaActual(idDestino);
            salasVisitadas.insert(idDestino);
            registro.registrarTurno("[AUTO] Se movio hacia '" + destino->getNombre() + "'");
            cout << "[AUTO] Se movio hacia '" << destino->getNombre() << "'\n";
            pausa(800);
            movido = true;
            break;
        }

        // 6. Intentar desbloquear sala bloqueada adyacente con llave disponible
        if (!movido) {
            for (const auto& [dir, idDestino] : sala->getSalidas()) {
                auto destino = mundo.getSala(idDestino);
                if (!destino || !destino->estaBloqueada()) continue;
                const string& llave = destino->getNombreLlaveRequerida();
                if (jugador->tieneObjeto(llave)) {
                    destino->desbloquear();
                    jugador->setIdSalaActual(idDestino);
                    salasVisitadas.insert(idDestino);
                    registro.registrarTurno("[AUTO] Desbloqueo y entro a '" + destino->getNombre() + "'");
                    cout << "[AUTO] Desbloqueo y entro a '" << destino->getNombre() << "'\n";
                    pausa(800);
                    movido = true;
                    break;
                }
            }
        }

        // 7. BFS para encontrar camino a sala no visitada o sala meta
        if (!movido) {
            map<string, string> padre;
            queue<string> cola;
            string inicio = jugador->getIdSalaActual();
            cola.push(inicio);
            padre[inicio] = "";
            bool encontrado = false;
            string objetivo = "";

            while (!cola.empty() && !encontrado) {
                string actual = cola.front(); cola.pop();
                auto salaActual = mundo.getSala(actual);
                if (!salaActual) continue;
                for (const auto& [d, vecino] : salaActual->getSalidas()) {
                    if (padre.count(vecino)) continue;
                    auto salaVecino = mundo.getSala(vecino);
                    if (!salaVecino || salaVecino->estaBloqueada()) continue;
                    padre[vecino] = actual;
                    if (!salasVisitadas.count(vecino) || vecino == "corredor") {
                        objetivo   = vecino;
                        encontrado = true;
                        break;
                    }
                    cola.push(vecino);
                }
            }

            if (encontrado && !objetivo.empty()) {
                vector<string> camino;
                string cur = objetivo;
                while (cur != inicio) {
                    camino.push_back(cur);
                    cur = padre[cur];
                }
                reverse(camino.begin(), camino.end());
                if (!camino.empty()) {
                    string siguienteSala = camino[0];
                    jugador->setIdSalaActual(siguienteSala);
                    if (!salasVisitadas.count(siguienteSala))
                        salasVisitadas.insert(siguienteSala);
                    auto destino = mundo.getSala(siguienteSala);
                    registro.registrarTurno("[AUTO] Navegando hacia '" + destino->getNombre() + "'");
                    cout << "[AUTO] Navegando hacia '" << destino->getNombre() << "'\n";
                    pausa(800);
                    movido = true;
                }
            }
        }

        // 8. Retroceder a cualquier sala accesible como ultimo recurso
        if (!movido) {
            for (const auto& [dir, idDestino] : sala->getSalidas()) {
                auto destino = mundo.getSala(idDestino);
                if (!destino || destino->estaBloqueada()) continue;
                jugador->setIdSalaActual(idDestino);
                registro.registrarTurno("[AUTO] Retrocedio hacia '" + destino->getNombre() + "'");
                cout << "[AUTO] Retrocedio hacia '" << destino->getNombre() << "'\n";
                pausa(800);
                break;
            }
        }
    }
}

// ============================================================
// Handlers de comandos del modo interactivo
// ============================================================

void Juego::manejarMirar() {
    auto sala = mundo.getSala(jugador->getIdSalaActual());
    if (!sala) throw runtime_error("Sala actual no encontrada!");
    cout << "\n" << sala->toStringDetallado();
    registro.registrarTurno("Miro alrededor en '" + sala->getNombre() + "'");
}

void Juego::manejarMover(const string& direccion) {
    if (direccion.empty()) {
        auto sala = mundo.getSala(jugador->getIdSalaActual());
        cout << "Ir donde? Salidas disponibles:\n";
        for (const auto& [dir, id] : sala->getSalidas())
            cout << "  - " << dir << "\n";
        return;
    }

    auto sala = mundo.getSala(jugador->getIdSalaActual());
    auto it   = sala->getSalidas().find(direccion);
    if (it == sala->getSalidas().end()) {
        cout << "No hay salida al " << direccion << ".\n";
        return;
    }

    auto salaMeta = mundo.getSala(it->second);
    if (!salaMeta) throw runtime_error("La sala destino no existe: " + it->second);

    if (salaMeta->estaBloqueada()) {
        const string& llave = salaMeta->getNombreLlaveRequerida();
        if (!jugador->tieneObjeto(llave)) {
            cout << "La puerta al " << direccion << " esta bloqueada. Necesitas: " << llave << ".\n";
            registro.registrarTurno("Intento entrar a '" + salaMeta->getNombre() + "' pero estaba bloqueada.");
            return;
        }
        salaMeta->desbloquear();
        cout << "Usas la " << llave << " para desbloquear la puerta.\n";
        registro.registrarTurno("Desbloqueo '" + salaMeta->getNombre() + "' con " + llave);
    }

    if (sala->tieneEnemigosVivos()) {
        cout << "Hay enemigos bloqueando el paso! Debes derrotarlos primero.\n";
        cout << "Enemigos presentes:\n";
        for (const auto& e : sala->getEnemigos())
            if (e->estaVivo())
                cout << "  - " << e->getNombre() << "\n";
        registro.registrarTurno("Intento huir pero los enemigos bloquearon la salida.");
        return;
    }

    jugador->setIdSalaActual(salaMeta->getId());
    registro.registrarTurno("Se movio al " + direccion + " hacia '" + salaMeta->getNombre() + "'");
    cout << "\nTe mueves al " << direccion << ".\n";
    manejarMirar();
    verificarCondicionVictoria();
}

void Juego::manejarRecoger(const string& nombreObjeto) {
    if (nombreObjeto.empty()) {
        auto sala = mundo.getSala(jugador->getIdSalaActual());
        if (sala->getObjetos().empty()) {
            cout << "No hay objetos aqui.\n";
        } else {
            cout << "Recoger que? Objetos presentes:\n";
            for (const auto& o : sala->getObjetos())
                cout << "  - " << o->getNombre() << "\n";
        }
        return;
    }

    auto sala   = mundo.getSala(jugador->getIdSalaActual());
    auto objeto = sala->tomarObjeto(nombreObjeto);
    if (!objeto) {
        cout << "No hay '" << nombreObjeto << "' aqui.\n";
        return;
    }

    jugador->recogerObjeto(objeto);
    registro.registrarTurno("Recogio: " + objeto->toString());
    cout << "Recoges: " << objeto->getNombre() << ".\n";

    if (objeto->getTipo() == TipoObjeto::ARMA || objeto->getTipo() == TipoObjeto::ARMADURA)
        if (jugador->equiparObjeto(objeto->getNombre()))
            cout << "(Se equipo automaticamente " << objeto->getNombre() << ")\n";

    if (objeto->getTipo() == TipoObjeto::TESORO) {
        jugador->agregarOro(objeto->getValor());
        cout << "Encontraste " << objeto->getValor() << " monedas de oro!\n";
    }
}

void Juego::manejarEquipar(const string& nombreObjeto) {
    if (nombreObjeto.empty()) {
        cout << "Equipar que? Objetos equipables en inventario:\n";
        for (const auto& o : jugador->getInventario())
            if (o->getTipo() == TipoObjeto::ARMA || o->getTipo() == TipoObjeto::ARMADURA)
                cout << "  - " << o->getNombre() << "\n";
        return;
    }

    string busqueda = nombreObjeto;
    transform(busqueda.begin(), busqueda.end(), busqueda.begin(), ::tolower);

    shared_ptr<Objeto> objetivo = nullptr;
    for (const auto& o : jugador->getInventario()) {
        string nombreO = o->getNombre();
        transform(nombreO.begin(), nombreO.end(), nombreO.begin(), ::tolower);
        if (nombreO == busqueda || nombreO.find(busqueda) != string::npos) {
            objetivo = o;
            break;
        }
    }

    if (!objetivo) {
        cout << "No tienes ese objeto en tu inventario.\n";
        return;
    }

    if (jugador->equiparObjeto(objetivo->getNombre())) {
        registro.registrarTurno("Equipo: " + objetivo->getNombre());
        cout << "Equipas: " << objetivo->getNombre() << ".\n";
    } else {
        cout << "Ese objeto no se puede equipar.\n";
    }
}

void Juego::manejarAtacar(const string& nombreEnemigo) {
    auto sala = mundo.getSala(jugador->getIdSalaActual());

    vector<shared_ptr<Enemigo>> vivosActual;
    for (const auto& e : sala->getEnemigos())
        if (e->estaVivo()) vivosActual.push_back(e);

    if (vivosActual.empty()) {
        cout << "No hay enemigos aqui.\n";
        return;
    }

    if (nombreEnemigo.empty()) {
        if (vivosActual.size() == 1) {
            rondaCombate(vivosActual[0]);
        } else {
            cout << "Atacar a quien? Enemigos presentes:\n";
            for (const auto& e : vivosActual)
                cout << "  - " << e->getNombre() << "\n";
        }
        return;
    }

    string busqueda = nombreEnemigo;
    transform(busqueda.begin(), busqueda.end(), busqueda.begin(), ::tolower);

    shared_ptr<Enemigo> objetivo = nullptr;
    for (const auto& e : vivosActual) {
        string nombreE = e->getNombre();
        transform(nombreE.begin(), nombreE.end(), nombreE.begin(), ::tolower);
        if (nombreE == busqueda || nombreE.find(busqueda) != string::npos) {
            objetivo = e;
            break;
        }
    }

    if (!objetivo) {
        cout << "No se encontro ese enemigo. Enemigos presentes:\n";
        for (const auto& e : vivosActual)
            cout << "  - " << e->getNombre() << "\n";
        return;
    }

    rondaCombate(objetivo);
}

void Juego::manejarUsarPocion() {
    shared_ptr<Objeto> pocion = nullptr;
    for (const auto& o : jugador->getInventario())
        if (o->getTipo() == TipoObjeto::POCION) { pocion = o; break; }

    if (!pocion) {
        cout << "No tienes pociones.\n";
        return;
    }

    int antes  = jugador->getSalud();
    jugador->curar(pocion->getValor());
    int curado = jugador->getSalud() - antes;
    registro.registrarTurno("Uso " + pocion->getNombre() + " y recupero " + to_string(curado) + " de salud.");
    cout << "Bebes la " << pocion->getNombre() << " y recuperas " << curado << " de salud.\n";
    cout << "Salud: " << jugador->getSalud() << "/" << jugador->getSaludMaxima() << "\n";
    jugador->eliminarObjeto(pocion->getNombre());
}

void Juego::manejarInventario() {
    cout << "\n--- Inventario ---\n" << jugador->inventarioToString();
}

void Juego::manejarEstado() {
    cout << "\n--- Estado ---\n" << jugador->toString() << "\n";
}

void Juego::manejarAyuda() {
    cout << "\n--- Comandos ---\n"
         << "  mirar             - describir la sala actual\n"
         << "  ir <direccion>    - moverse (norte/sur/este/oeste)\n"
         << "  recoger <objeto>  - recoger un objeto del suelo\n"
         << "  equipar <objeto>  - equipar un arma o armadura\n"
         << "  atacar <enemigo>  - atacar a un enemigo\n"
         << "  curar / pocion    - usar una pocion de salud\n"
         << "  inv / inventario  - mostrar inventario\n"
         << "  estado            - mostrar estadisticas del jugador\n"
         << "  ayuda             - mostrar esta lista\n"
         << "  salir             - terminar la aventura\n";
}

// ============================================================
// Logica de simulacion
// ============================================================

void Juego::rondaCombate(shared_ptr<Enemigo> enemigo) {
    int danioJugador = enemigo->recibirDanio(jugador->getAtaqueTotal());
    registro.registrarTurno(jugador->getNombre() + " ataca a " + enemigo->getNombre() +
                             " causando " + to_string(danioJugador) + " de danio.");
    cout << "Golpeas a " << enemigo->getNombre() << " causando " << danioJugador << " de danio!\n";

    if (!enemigo->estaVivo()) {
        registro.registrarTurno(enemigo->getNombre() + " ha sido derrotado!");
        cout << enemigo->getNombre() << " ha sido derrotado!\n";
        jugador->agregarOro(enemigo->getOroRecompensa());
        if (enemigo->getOroRecompensa() > 0)
            cout << "Obtuviste " << enemigo->getOroRecompensa() << " monedas de oro!\n";
        verificarCondicionVictoria();
        return;
    }

    int danioEnemigo = jugador->recibirDanio(enemigo->getPoderAtaque());
    registro.registrarTurno(enemigo->getNombre() + " contraataca causando " + to_string(danioEnemigo) + " de danio.");
    cout << enemigo->getNombre() << " te golpea causando " << danioEnemigo << " de danio!\n";
    cout << "Tu salud: " << jugador->getSalud() << "/" << jugador->getSaludMaxima() << "\n";
}

void Juego::verificarCondicionVictoria() {
    if (jugador->getIdSalaActual() == mundo.getIdSalaMeta() &&
        !mundo.getSala(jugador->getIdSalaActual())->tieneEnemigosVivos())
        estado = EstadoJuego::GANADO;
}

void Juego::generarReporte() const {
    ofstream archivo(rutaReporte);
    if (!archivo.is_open())
        throw runtime_error("No se puede abrir el archivo de reporte: " + rutaReporte);

    archivo << "========================================\n";
    archivo << "   AVENTURA EN LA MAZMORRA - REPORTE FINAL\n";
    archivo << "========================================\n\n";
    archivo << "--- Estado del Jugador ---\n" << jugador->toString() << "\n\n";
    archivo << "--- Inventario ---\n"         << jugador->inventarioToString() << "\n";
    archivo << "--- Resultado ---\n";

    if (estado == EstadoJuego::GANADO)
        archivo << "RESULTADO: VICTORIA - " << jugador->getNombre() << " escapo de la mazmorra!\n";
    else
        archivo << "RESULTADO: DERROTA - "  << jugador->getNombre() << " no sobrevivio.\n";

    archivo << "\n--- Resumen de Bitacora ---\n";
    int cuenta = 0;
    for (const auto& entrada : registro.getEntradas()) {
        archivo << entrada << "\n";
        if (++cuenta > 200) { archivo << "... (truncado)\n"; break; }
    }
    archivo.close();
}

// ============================================================
// Utilidades
// ============================================================

pair<string, string> Juego::parsearComando(const string& linea) {
    istringstream ss(linea);
    string verbo, resto;
    ss >> verbo;
    getline(ss, resto);
    if (!resto.empty() && resto[0] == ' ') resto = resto.substr(1);
    resto.erase(resto.find_last_not_of(" \t") + 1);
    transform(verbo.begin(), verbo.end(), verbo.begin(), ::tolower);
    return {verbo, resto};
}