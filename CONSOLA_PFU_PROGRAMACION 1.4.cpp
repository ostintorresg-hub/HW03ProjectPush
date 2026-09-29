#include <iostream>
#include <string>
#include <cctype>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <limits>
#include <conio.h>
#include <windows.h>

using namespace std;
// ==========================================
// PROTOTIPOS DE FUNCIONES (ANTES DEL MAIN)
// ==========================================

// ==========================================
// DECLARACION DE FUNCIONES DE LOS JUEGOS
// ==========================================
void jugarAhorcado(string nombreJugador);
void jugarBingo(string nombreJugador);
void jugarTetris(string nombreJugador);
void jugarSnake(string nombreJugador);

// ==========================================
// PROTOTIPOS: FUNCIONES DE VALIDACION Y UTILIDAD
// ==========================================
bool validarNombre(string nombre);
string convertirMayusculas(string nombre);
int calcularEdad(int diaN, int mesN, int anioN);
int obtenerAnioActual();

// ==========================================
// PROTOTIPOS: SOPORTE DE SECUENCIAS ANSI/VT PARA CONSOLA
// ==========================================
void habilitarModoVirtualTerminal();
void ajustarTamanoConsola();
void moverCursorInicio();

// ==========================================
// PROTOTIPOS: FUNCION AUXILIAR DE LECTURA SEGURA
// ==========================================
int leerEntero(const string &mensaje);
char leerOpcionMenu(const string &mensaje);
bool leerConfirmacionSN(const string &mensaje);

// ==========================================
// PROTOTIPOS: FUNCIONES AUXILIARES PARA EL AHORCADO
// ==========================================
void mostrarAhorcado(int errores);

// ==========================================
// PROTOTIPOS: FUNCIONES AUXILIARES PARA BINGO
// ==========================================
bool verificarGanadorBingo(bool marcados[5][5], string &tipoVictoria);
string generarTextoCarton(string nombre, int carton[5][5], bool marcados[5][5]);

// ==========================================
// FUNCIONES AUXILIARES PARA TETRIS
// ==========================================
// Diseno tomado del archivo "consola" (piezas de tamano dinamico 2x2/3x3/4x4
// en vez de una matriz fija 4x4, y con el cursor del sistema oculto durante
// la partida). Se le agregaron los colores por tipo de pieza que ya tenia
// esta version.
const int FILAS_TETRIS = 20;
const int COLS_TETRIS = 10;

struct PiezaTetris {
    int x, y;
    int tipo;
    int matriz[4][4];
    int size;
};

// Un color fijo por cada tipo de pieza, igual que en el Tetris clasico.
const string COLORES_PIEZAS[7] = {
    "\x1b[1;36m", // I - Cian
    "\x1b[1;33m", // O - Amarillo
    "\x1b[1;35m", // T - Magenta
    "\x1b[1;32m", // S - Verde
    "\x1b[1;31m", // Z - Rojo
    "\x1b[1;34m", // J - Azul
    "\x1b[38;5;208m" // L - Naranja
};

// ==========================================
// PROTOTIPOS: FUNCIONES AUXILIARES PARA TETRIS
// ==========================================
void mostrarOcultarCursorTetris(bool visible);
void asignarPieza(PiezaTetris &p);
void rotarPieza(PiezaTetris &p);
bool comprobarColision(PiezaTetris p, int nx, int ny, int tablero[FILAS_TETRIS][COLS_TETRIS]);
void fijarPieza(PiezaTetris p, int tablero[FILAS_TETRIS][COLS_TETRIS]);

// ==========================================
// FUNCIONES AUXILIARES PARA SNAKE
// ==========================================
// Diseno tomado del archivo "consola": la serpiente es un arreglo de
// posiciones (PuntoSnake), arranca en el centro del tablero moviendose
// hacia la derecha, se controla SOLO con las flechas de direccion (no con
// WASD), y el cursor del sistema se oculta durante la partida. Se agregaron
// los colores que ya tenia esta version (serpiente verde, comida roja).
const int FILAS_SNAKE = 20;
const int COLUMNAS_SNAKE = 20;
const int LONGITUD_MAX_SNAKE = FILAS_SNAKE * COLUMNAS_SNAKE;

struct PuntoSnake {
    int fila;
    int columna;
};

// ==========================================
// PROTOTIPOS: FUNCIONES AUXILIARES PARA SNAKE
// ==========================================
void mostrarOcultarCursorSnake(bool visible);
bool posicionOcupadaPorSerpiente(PuntoSnake serpiente[], int longitud, int f, int c);
void generarComidaSnake(PuntoSnake serpiente[], int longitud, PuntoSnake &comida);
void dibujarTableroSnake(PuntoSnake serpiente[], int longitud, PuntoSnake comida, const string &nombreJugador, int puntaje);
void leerTeclaSnake(char &direccionActual);
bool moverSerpiente(PuntoSnake serpiente[], int &longitud, char direccionActual, PuntoSnake &comida,
                     int &puntaje, int &alimentosConsumidos, int &siguienteBonusLongitud, bool &bonus15Otorgado);

// ==========================================
// COLORES ANSI PARA MEJORAR LA INTERFAZ
// ==========================================
const string COLOR_RESET     = "\x1b[0m";
const string COLOR_VERDE     = "\x1b[1;32m";
const string COLOR_ROJO      = "\x1b[1;31m";
const string COLOR_AMARILLO  = "\x1b[1;33m";
const string COLOR_CIAN      = "\x1b[1;36m";
const string COLOR_GRIS      = "\x1b[90m";

// ==========================================
// FUNCION PRINCIPAL
// ==========================================
int main() {
    string nombreIngresado, nombreJugador;
    int dia, mes, anio, edad;
    bool accesoPermitido = false;
    int anioActual = obtenerAnioActual();

    habilitarModoVirtualTerminal();
    ajustarTamanoConsola();

    cout << "========================================" << endl;
    cout << "        CONSOLA DE VIDEOJUEGOS          " << endl;
    cout << "========================================" << endl;

    // 1. Ingreso y validacion del nombre
    while (true) {
        cout << "\nIngrese su nombre (solo letras y espacios): ";
        getline(cin, nombreIngresado);

        if (validarNombre(nombreIngresado)) {
            nombreJugador = convertirMayusculas(nombreIngresado);
            break;
        } else {
            cout << "Error: El nombre debe contener unicamente caracteres alfabeticos y espacios." << endl;
        }
    }

    // 2. Ingreso de fecha de nacimiento y validacion de edad
    while (!accesoPermitido) {
        cout << "\n--- Fecha de Nacimiento ---" << endl;

        dia = leerEntero("Dia (1-31): ");
        mes = leerEntero("Mes (1-12): ");
        anio = leerEntero("Anio (Ej. 2005): ");

        if (dia < 1 || dia > 31) {
            cout << "Dia invalido. Intente nuevamente.\n";
            continue;
        }

        if (mes < 1 || mes > 12) {
            cout << "Mes invalido. Intente nuevamente.\n";
            continue;
        }

        if (anio < 1900 || anio > anioActual) {
            cout << "Anio invalido. Intente nuevamente.\n";
            continue;
        }

        edad = calcularEdad(dia, mes, anio);

        if (edad < 18) {
            cout << "\nLo sentimos, tienes " << edad << " anios." << endl;
            cout << "No cumples con la edad minima (18 anios) para jugar. Reiniciando proceso..." << endl;
            // El ciclo se repite solicitando la fecha nuevamente
        } else {
            cout << "\nEdad verificada: " << edad << " anios. Acceso concedido." << endl;
            accesoPermitido = true;
        }
    }

    // 3. Menu principal interactivo
    bool salir = false;

    while (!salir) {
        cout << "\n========================================" << endl;
        cout << "             MENU PRINCIPAL             " << endl;
        cout << "         Jugador: " << nombreJugador << endl;
        cout << "========================================" << endl;
        cout << "A - Jugar \"El ahorcado\"" << endl;
        cout << "B - Jugar \"Bingo\"" << endl;
        cout << "C - Jugar \"Tetris\"" << endl;
        cout << "D - Jugar \"Snake\"" << endl;
        cout << "F - Salir de la consola" << endl;
        cout << "========================================" << endl;

        char opcion = leerOpcionMenu("Seleccione una opcion: ");

        switch (opcion) {
            case 'A':
                jugarAhorcado(nombreJugador);
                break;
            case 'B':
                jugarBingo(nombreJugador);
                break;
            case 'C':
                jugarTetris(nombreJugador);
                break;
            case 'D':
                jugarSnake(nombreJugador);
                break;
            case 'F':
                cout << "\nGracias por usar la consola de videojuegos. !Hasta pronto, " << nombreJugador << "!" << endl;
                salir = true;
                break;
            default:
                cout << "\nError: Opcion no valida. Por favor, intente de nuevo." << endl;
                break;
        }
    }

    return 0;
}

// ==========================================
// IMPLEMENTACION (DESPUES DEL MAIN)
// ==========================================

// ==========================================
// FUNCIONES DE VALIDACION Y UTILIDAD
// ==========================================

// Valida que el nombre contenga unicamente letras y espacios
bool validarNombre(string nombre) {
    if (nombre.empty()) {
        return false;
    }
    for (char c : nombre) {
        if (!isalpha((unsigned char)c) && c != ' ') {
            return false;
        }
    }
    return true;
}

// Transforma el nombre completamente a mayusculas
string convertirMayusculas(string nombre) {
    string nombreMayus = "";
    for (char c : nombre) {
        nombreMayus += toupper((unsigned char)c);
    }
    return nombreMayus;
}

// Calcula la edad exacta basandose en la fecha actual del sistema
int calcularEdad(int diaN, int mesN, int anioN) {
    time_t t = time(0);
    tm* now = localtime(&t);
    int anioActual = now->tm_year + 1900;
    int mesActual = now->tm_mon + 1;
    int diaActual = now->tm_mday;

    int edad = anioActual - anioN;
    if (mesActual < mesN || (mesActual == mesN && diaActual < diaN)) {
        edad--;
    }
    return edad;
}

// Devuelve el anio actual del sistema (para no dejar limites de fecha "quemados")
int obtenerAnioActual() {
    time_t t = time(0);
    tm* now = localtime(&t);
    return now->tm_year + 1900;
}

// ==========================================
// SOPORTE DE SECUENCIAS ANSI/VT PARA CONSOLA
// ==========================================
// SetConsoleCursorPosition (API clasica de windows.h) no se reposiciona bien
// en la Terminal moderna de Windows (conpty), lo que provoca ese efecto de
// "parpadeo/salto" como si se presionara Enter constantemente durante
// Tetris y Snake. La solucion es usar secuencias ANSI, que son compatibles
// tanto con la consola clasica como con la Terminal moderna.
void habilitarModoVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (GetConsoleMode(hOut, &modo)) {
        modo |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, modo);
    }
}

// Agranda la ventana y el buffer de la consola al iniciar el programa, para
// que Tetris y Snake (que necesitan ~30 lineas por fotograma) quepan sin
// necesidad de hacer scroll. Si la terminal no soporta este ajuste (algunas
// terminales lo ignoran), el programa simplemente sigue funcionando igual.
void ajustarTamanoConsola() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Primero se encoge la ventana visible para poder redimensionar el
    // buffer sin que Windows rechace el cambio por ser mas chico que la
    // ventana actual.
    SMALL_RECT rectMinimo = {0, 0, 1, 1};
    SetConsoleWindowInfo(hOut, TRUE, &rectMinimo);

    COORD tamanoBuffer = {100, 45};
    SetConsoleScreenBufferSize(hOut, tamanoBuffer);

    SMALL_RECT rectVentana = {0, 0, 89, 42};
    SetConsoleWindowInfo(hOut, TRUE, &rectVentana);
}

// Mueve el cursor a la esquina superior izquierda usando ANSI. Ya no hace
// falta limpiar toda la pantalla en cada fotograma: como ajustarTamanoConsola()
// agranda la ventana/buffer al iniciar el programa, el tablero completo
// (cabecera + filas + pie) siempre entra sin que la consola tenga que hacer
// scroll. Al no scrollear, "Home" siempre apunta al mismo lugar, y como cada
// fotograma reescribe TODAS las celdas del tablero, no queda ningun resto de
// fotogramas anteriores. Resultado: se ve nitido y sin parpadeo.
void moverCursorInicio() {
    cout << "\x1b[H";
}

// ==========================================
// FUNCION AUXILIAR DE LECTURA SEGURA
// ==========================================
// Lee un entero validando que el usuario no haya escrito letras/simbolos.
// Si cin falla, limpia el estado de error y descarta el buffer para evitar
// el bucle infinito clasico de "cin >> variable" con entradas invalidas.
int leerEntero(const string &mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Debe ingresar un numero entero.\n";
            continue;
        }
        // Limpia cualquier resto de texto en la misma linea (ej: "12 abc")
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return valor;
    }
}

// Lee un solo caracter de opcion (menu) descartando basura sobrante del buffer
char leerOpcionMenu(const string &mensaje) {
    char opcion;
    cout << mensaje;
    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return toupper((unsigned char)opcion);
}

// Pregunta S/N y NO acepta ninguna otra letra: si el usuario escribe algo
// distinto de S o N (mayuscula o minuscula), se le indica el error y se
// vuelve a preguntar, en vez de asumir "No" por defecto.
bool leerConfirmacionSN(const string &mensaje) {
    char opcion;
    while (true) {
        cout << mensaje;
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        opcion = toupper((unsigned char)opcion);

        if (opcion == 'S') return true;
        if (opcion == 'N') return false;

        cout << "[!] Opcion invalida. Por favor ingresa S (Si) o N (No)." << endl;
    }
}

// ==========================================
// FUNCIONES AUXILIARES PARA EL AHORCADO
// ==========================================
void mostrarAhorcado(int errores) {
    // El dibujo se pinta en verde con pocos errores, amarillo a mitad de
    // camino y rojo cuando el jugador esta cerca de perder.
    string color = COLOR_VERDE;
    if (errores >= 5) color = COLOR_ROJO;
    else if (errores >= 3) color = COLOR_AMARILLO;

    cout << color;
    cout << "\n +---+" << endl;
    cout << " |   |" << endl;
    switch (errores) {
        case 0:
            cout << "     |" << endl << "     |" << endl << "     |" << endl << "     |" << endl;
            break;
        case 1:
            cout << " O   |" << endl << "     |" << endl << "     |" << endl << "     |" << endl;
            break;
        case 2:
            cout << " O   |" << endl << " |   |" << endl << "     |" << endl << "     |" << endl;
            break;
        case 3:
            cout << " O   |" << endl << "/|   |" << endl << "     |" << endl << "     |" << endl;
            break;
        case 4:
            cout << " O   |" << endl << "/|\\  |" << endl << "     |" << endl << "     |" << endl;
            break;
        case 5:
            cout << " O   |" << endl << "/|\\  |" << endl << "/    |" << endl << "     |" << endl;
            break;
        default:
            cout << " O   |" << endl << "/|\\  |" << endl << "/ \\  |" << endl << "     |" << endl;
            break;
    }
    cout << "=========" << COLOR_RESET << endl;
}

// ==========================================
// IMPLEMENTACION: EL AHORCADO
// ==========================================
void jugarAhorcado(string nombreJugador) {
	vector<string> bancoPalabras = {"pseudocodigo","algoritmo","programacion","variable",
    "compilador","desarrollo","matriz","funciones","consola","bucle","puntero","ejecucion",
    "libreria","sintaxis","memoria"
    };

    int indiceAleatorio = rand() % bancoPalabras.size();
    string palabraSecreta = bancoPalabras[indiceAleatorio];
    
    int maxErrores = 6;
    int errores = 0;
    int intentos = 0; 	

    vector<bool> adivinadas(palabraSecreta.length(), false);

    string letrasIncorrectas = "";
    string letrasAcertadas = "";
    bool juegoTerminado = false;
    bool victoria = false;

    while (!juegoTerminado) {
        system("cls");

        cout << COLOR_CIAN << "========================================" << COLOR_RESET << endl;
        cout << COLOR_AMARILLO << "              EL AHORCADO               " << COLOR_RESET << endl;
        cout << COLOR_CIAN << "========================================" << COLOR_RESET << endl;

        mostrarAhorcado(errores);

        // La palabra se muestra siempre en MAYUSCULA, aunque internamente
        // este guardada en minuscula.
        cout << "\nPalabra: " << COLOR_VERDE;
        victoria = true;
        for (size_t i = 0; i < palabraSecreta.length(); i++) {
            if (adivinadas[i]) {
                cout << (char)toupper((unsigned char)palabraSecreta[i]) << " ";
            } else {
                cout << "_ ";
                victoria = false;
            }
        }
        cout << COLOR_RESET << endl;

        cout << "\nIntentos restantes: " << COLOR_AMARILLO << (maxErrores - errores) << COLOR_RESET << endl;
        
        cout << "Letras correctas   : " << COLOR_VERDE;

		if (letrasAcertadas.empty()) {
    		cout << "Ninguna";
		} else {
    		cout << convertirMayusculas(letrasAcertadas);
			}
			
		cout << COLOR_RESET << endl;

        cout << "Letras incorrectas : " << COLOR_ROJO;
        
		if (letrasIncorrectas.empty()) {
    		cout << "Ninguna";
		} else {
    		cout << convertirMayusculas(letrasIncorrectas);
		}
		
		cout << COLOR_RESET << endl;
        
        cout << COLOR_CIAN << "========================================" << COLOR_RESET << endl;

        if (victoria || errores >= maxErrores) {
            juegoTerminado = true;
            break;
        }

        string entrada;
        cout << "\nIngresa una letra: ";
        cin >> entrada;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (entrada.length() != 1 || !isalpha((unsigned char)entrada[0])) {
            cout << COLOR_ROJO << "Entrada invalida. Por favor ingresa unicamente una letra." << COLOR_RESET << endl;
            cout << "Presiona Enter para continuar...";
            cin.get();
            continue;
        }

        char letra = tolower((unsigned char)entrada[0]);
        intentos++;

        if (letrasAcertadas.find(letra) != string::npos || letrasIncorrectas.find(letra) != string::npos) {
            cout << COLOR_GRIS << "Ya usaste esa letra. Intenta con otra." << COLOR_RESET << endl;
            cout << "Presiona Enter para continuar...";
            cin.get();
            continue;
        }

        bool acierto = false;
        for (size_t i = 0; i < palabraSecreta.length(); i++) {
            if (palabraSecreta[i] == letra) {
                adivinadas[i] = true;
                acierto = true;
            }
        }

        if (acierto) {
            letrasAcertadas += letra;
            letrasAcertadas += " ";
        } else {
            letrasIncorrectas += letra;
            letrasIncorrectas += " ";
            errores++;
        }
    }

    system("cls");
	
    cout << COLOR_CIAN << "========================================" << COLOR_RESET << endl;
    mostrarAhorcado(errores);

    if (victoria) {
        cout << "\n" << COLOR_VERDE << "!Felicidades " << nombreJugador << ", has GANADO!" << COLOR_RESET << endl;
    } else {
        cout << "\n" << COLOR_ROJO << "Has PERDIDO. Alcanzaste el limite de errores permitidos (6)." << COLOR_RESET << endl;
    }

    cout << "La palabra secreta era: " << COLOR_AMARILLO << convertirMayusculas(palabraSecreta) << COLOR_RESET << endl;
    cout << COLOR_CIAN << "========================================" << COLOR_RESET << endl;

    string nombreArchivo = nombreJugador + "_AHORCADO.txt";
    ofstream archivo(nombreArchivo);

    if (archivo.is_open()) {
        archivo << "Nombre del jugador: " << nombreJugador << endl;
        archivo << "Nombre del juego: EL AHORCADO" << endl;
        archivo << "Resultado de la partida: " << (victoria ? "Victoria" : "Derrota") << endl;
        archivo << "Palabra secreta: " << palabraSecreta << endl;
        archivo << "Numero de intentos realizados: " << intentos << endl;
        archivo << "Letras acertadas: " << (letrasAcertadas.empty() ? "Ninguna" : letrasAcertadas) << endl;
        archivo << "Letras incorrectas: " << (letrasIncorrectas.empty() ? "Ninguna" : letrasIncorrectas) << endl;
        archivo.close();
        cout << "\n[i] El archivo con los resultados (" << nombreArchivo << ") ha sido generado exitosamente." << endl;
    } else {
        cout << "\n[!] Error al intentar generar el archivo de texto." << endl;
    }

    cout << "\nPresiona Enter para regresar al menu principal...";
    cin.get();
    system("cls");
}

// ==========================================
// FUNCIONES AUXILIARES PARA BINGO
// ==========================================
bool verificarGanadorBingo(bool marcados[5][5], string &tipoVictoria) {
    for (int i = 0; i < 5; i++) {
        if (marcados[i][0] && marcados[i][1] && marcados[i][2] && marcados[i][3] && marcados[i][4]) {
            tipoVictoria = "Fila " + to_string(i + 1);
            return true;
        }
    }
    for (int j = 0; j < 5; j++) {
        if (marcados[0][j] && marcados[1][j] && marcados[2][j] && marcados[3][j] && marcados[4][j]) {
            tipoVictoria = "Columna " + to_string(j + 1);
            return true;
        }
    }
    if (marcados[0][0] && marcados[1][1] && marcados[2][2] && marcados[3][3] && marcados[4][4]) {
        tipoVictoria = "Diagonal Principal";
        return true;
    }
    if (marcados[0][4] && marcados[1][3] && marcados[2][2] && marcados[3][1] && marcados[4][0]) {
        tipoVictoria = "Diagonal Secundaria";
        return true;
    }
    return false;
}

// Genera el texto del carton (una sola implementacion; se usa tanto para
// consola como para archivo, evitando el parametro peligroso de la version original)
string generarTextoCarton(string nombre, int carton[5][5], bool marcados[5][5]) {
    string salida = "";
    string linea = "----------------------";
    string encabezado = "   B   I   N   G   O";

    salida += "Carton de " + nombre + ":\n";
    salida += encabezado + "\n" + linea + "\n";

    for (int i = 0; i < 5; i++) {
        string filaTexto = "";
        for (int j = 0; j < 5; j++) {
            if (marcados[i][j]) {
                filaTexto += "  * ";
            } else {
                if (carton[i][j] < 10) filaTexto += "  " + to_string(carton[i][j]) + " ";
                else filaTexto += " " + to_string(carton[i][j]) + " ";
            }
        }
        salida += filaTexto + "\n";
    }
    return salida;
}

// ==========================================
// IMPLEMENTACION: BINGO
// ==========================================
void jugarBingo(string nombreJugador) {
    bool jugarDeNuevo = true;
    srand((unsigned int)time(0));

    while (jugarDeNuevo) {
        system("cls");

        cout << "========================================" << endl;
        cout << "                 BINGO                  " << endl;
        cout << "========================================" << endl;
        cout << "Reglas basicas: Se sortearan numeros del 1 al 75." << endl;
        cout << "Si un numero sorteado esta en tu carton, se marcara con un '*'." << endl;
        cout << "Gana el primero en completar una Fila, Columna o Diagonal." << endl;
        cout << "========================================\n" << endl;

        int numJugadores = leerEntero("Ingrese el numero de jugadores (Minimo 3, Maximo 10): ");
        while (numJugadores < 3 || numJugadores > 10) {
            cout << "[!] Error: Deben participar entre 3 y 10 jugadores." << endl;
            numJugadores = leerEntero("Ingrese el numero de jugadores (Minimo 3, Maximo 10): ");
        }

        string nombres[10];
        nombres[0] = nombreJugador;
        cout << "\nJugador 1 (Tu): " << nombreJugador << endl;

        for (int i = 1; i < numJugadores; i++) {
            string nombreTemp;
            // Se usa getline (no "cin >>") para que un nombre con espacio,
            // por ejemplo "MATIAS PAUCAR", no se divida y se cuele en el
            // siguiente jugador.
            do {
                cout << "Ingrese el nombre del Jugador " << (i + 1) << ": ";
                getline(cin, nombreTemp);
                if (nombreTemp.empty()) {
                    cout << "[!] El nombre no puede estar vacio." << endl;
                }
            } while (nombreTemp.empty());

            for (char &c : nombreTemp) c = toupper((unsigned char)c);
            nombres[i] = nombreTemp;
        }

        int cartones[10][5][5];
        bool marcados[10][5][5];

        for (int p = 0; p < numJugadores; p++) {
            for (int col = 0; col < 5; col++) {
                int minVal = col * 15 + 1;

                int generados = 0;
                while (generados < 5) {
                    int randNum = minVal + rand() % 15;
                    bool repetido = false;
                    for (int k = 0; k < generados; k++) {
                        if (cartones[p][k][col] == randNum) repetido = true;
                    }
                    if (!repetido) {
                        cartones[p][generados][col] = randNum;
                        marcados[p][generados][col] = false;
                        generados++;
                    }
                }
            }
            cartones[p][2][2] = 0;
            marcados[p][2][2] = true;
        }

        cout << "\n[i] Cartones generados exitosamente. Presiona Enter para iniciar el sorteo...";
        cin.get();

        int bolas[75];
        for (int i = 0; i < 75; i++) bolas[i] = i + 1;
        for (int i = 0; i < 75; i++) {
            int r = rand() % 75;
            int temp = bolas[i];
            bolas[i] = bolas[r];
            bolas[r] = temp;
        }

        int bolasSorteadas = 0;
        bool hayGanador = false;
        int idGanador = -1;
        string tipoVictoria = "";

        while (bolasSorteadas < 75 && !hayGanador) {
            system("cls");

            int numeroActual = bolas[bolasSorteadas];
            bolasSorteadas++;

            cout << "========================================" << endl;
            cout << ">> BOLA SORTEADA: " << numeroActual << " <<" << endl;
            cout << "Bolas extraidas: " << bolasSorteadas << "/75" << endl;
            cout << "========================================" << endl;

            for (int p = 0; p < numJugadores; p++) {
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 5; j++) {
                        if (cartones[p][i][j] == numeroActual) {
                            marcados[p][i][j] = true;
                        }
                    }
                }
                cout << "\n" << generarTextoCarton(nombres[p], cartones[p], marcados[p]);
            }

            for (int p = 0; p < numJugadores; p++) {
                if (verificarGanadorBingo(marcados[p], tipoVictoria)) {
                    hayGanador = true;
                    idGanador = p;
                    break;
                }
            }

            if (!hayGanador) {
                cout << "\nPresiona Enter para sacar la siguiente bola...";
                cin.get();
            }
        }

        cout << "\n========================================" << endl;
        if (hayGanador) {
            cout << "!BINGO! Felicidades, has ganado " << nombres[idGanador] << "." << endl;
            cout << "Combinacion ganadora: " << tipoVictoria << endl;
        } else {
            cout << "La partida ha terminado sin ganador (Se sortearon los 75 numeros)." << endl;
        }
        cout << "========================================" << endl;

        if (hayGanador) {
            string nombreArchivo = nombres[idGanador] + "_BINGO.txt";
            ofstream archivo(nombreArchivo);

            if (archivo.is_open()) {
                archivo << "Nombre del ganador: " << nombres[idGanador] << endl;
                archivo << "Nombre del juego: BINGO" << endl;
                archivo << "Cantidad de jugadores: " << numJugadores << endl;
                archivo << "Numero total de bolas sorteadas: " << bolasSorteadas << endl;
                archivo << "Combinacion ganadora: " << tipoVictoria << endl;
                archivo << "\n" << generarTextoCarton(nombres[idGanador], cartones[idGanador], marcados[idGanador]);

                archivo.close();
                cout << "\n[i] El archivo de resultados (" << nombreArchivo << ") ha sido generado." << endl;
            } else {
                cout << "\n[!] Error al crear el archivo .txt" << endl;
            }
        }

        if (!leerConfirmacionSN("\nDeseas jugar nuevamente BINGO? (S/N): ")) {
            jugarDeNuevo = false;
        }
    }
}

// Oculta o muestra el cursor parpadeante del sistema durante la partida
// (como en "consola"), para que no se vea superpuesto sobre el tablero.
void mostrarOcultarCursorTetris(bool visible) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void asignarPieza(PiezaTetris &p) {
    p.tipo = rand() % 7;
    p.y = 0;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            p.matriz[i][j] = 0;

    switch (p.tipo) {
        case 0: // O
            p.size = 2;
            p.matriz[0][0] = 1; p.matriz[0][1] = 1;
            p.matriz[1][0] = 1; p.matriz[1][1] = 1;
            break;
        case 1: // I
            p.size = 4;
            p.matriz[1][0] = 1; p.matriz[1][1] = 1; p.matriz[1][2] = 1; p.matriz[1][3] = 1;
            break;
        case 2: // T
            p.size = 3;
            p.matriz[0][1] = 1;
            p.matriz[1][0] = 1; p.matriz[1][1] = 1; p.matriz[1][2] = 1;
            break;
        case 3: // L
            p.size = 3;
            p.matriz[0][2] = 1;
            p.matriz[1][0] = 1; p.matriz[1][1] = 1; p.matriz[1][2] = 1;
            break;
        case 4: // J
            p.size = 3;
            p.matriz[0][0] = 1;
            p.matriz[1][0] = 1; p.matriz[1][1] = 1; p.matriz[1][2] = 1;
            break;
        case 5: // S
            p.size = 3;
            p.matriz[0][1] = 1; p.matriz[0][2] = 1;
            p.matriz[1][0] = 1; p.matriz[1][1] = 1;
            break;
        case 6: // Z
            p.size = 3;
            p.matriz[0][0] = 1; p.matriz[0][1] = 1;
            p.matriz[1][1] = 1; p.matriz[1][2] = 1;
            break;
    }
    p.x = (COLS_TETRIS / 2) - (p.size / 2);
}

void rotarPieza(PiezaTetris &p) {
    PiezaTetris temporal = p;
    for (int i = 0; i < p.size; i++) {
        for (int j = 0; j < p.size; j++) {
            p.matriz[j][p.size - 1 - i] = temporal.matriz[i][j];
        }
    }
}

bool comprobarColision(PiezaTetris p, int nx, int ny, int tablero[FILAS_TETRIS][COLS_TETRIS]) {
    for (int i = 0; i < p.size; i++) {
        for (int j = 0; j < p.size; j++) {
            if (p.matriz[i][j] != 0) {
                int tx = nx + j;
                int ty = ny + i;

                if (tx < 0 || tx >= COLS_TETRIS || ty >= FILAS_TETRIS) return true;
                if (ty >= 0 && tablero[ty][tx] != 0) return true;
            }
        }
    }
    return false;
}

// El tablero guarda tipo+1 (1 a 7) en vez de solo "ocupado", para poder
// colorear cada bloque fijo con el color de su pieza original.
void fijarPieza(PiezaTetris p, int tablero[FILAS_TETRIS][COLS_TETRIS]) {
    for (int i = 0; i < p.size; i++) {
        for (int j = 0; j < p.size; j++) {
            if (p.matriz[i][j] != 0 && p.y + i >= 0) {
                tablero[p.y + i][p.x + j] = p.tipo + 1;
            }
        }
    }
}

// ==========================================
// IMPLEMENTACION: TETRIS
// ==========================================
void jugarTetris(string nombreJugador) {
    bool quiereJugarDeNuevo = true;
    srand((unsigned int)time(0));

    while (quiereJugarDeNuevo) {
        system("cls");
        cout << COLOR_CIAN << "=========================================================" << COLOR_RESET << endl;
        cout << COLOR_AMARILLO << "                    BIENVENIDO A TETRIS                  " << COLOR_RESET << endl;
        cout << COLOR_CIAN << "=========================================================" << COLOR_RESET << endl;
        cout << "Hola " << nombreJugador << ", preparate para jugar.\n\n";
        cout << "REGLAS BASICAS:\n";
        cout << "1. Alinea bloques para completar filas enteras y eliminarlas.\n";
        cout << "2. El juego termina si los bloques llegan al tope superior.\n\n";
        cout << "CONTROLES:\n";
        cout << "  W / w -> Rotar pieza\n";
        cout << "  A / a -> Mover a la izquierda\n";
        cout << "  D / d -> Mover a la derecha\n";
        cout << "  S / s -> Caida rapida (Bajar)\n";
        cout << "  F / f -> Forzar salida guardando progreso\n\n";
        cout << "Presiona cualquier tecla para comenzar...";
        _getch();

        system("cls");
        mostrarOcultarCursorTetris(false);

        int tableroTetris[FILAS_TETRIS][COLS_TETRIS] = {0};

        int puntaje = 0;
        int filasEliminadas = 0;
        int piezasUsadas = 0;
        bool juegoActivo = true;
        bool salirForzado = false;

        time_t tiempoInicio = time(0);

        PiezaTetris actual;
        asignarPieza(actual);
        piezasUsadas++;

        while (juegoActivo) {
            moverCursorInicio();

            int nivelActual = (filasEliminadas / 4) + 1;

            cout << COLOR_CIAN << "=========================================================" << COLOR_RESET << endl;
            cout << " JUGADOR: " << nombreJugador << " | NIVEL: " << nivelActual << "     " << endl;
            cout << " PUNTOS: " << puntaje << " | FILAS: " << filasEliminadas << " | F: Salir   " << endl;
            cout << "---------------------------------------------------------" << endl;

            for (int i = 0; i < FILAS_TETRIS; i++) {
                cout << "#";
                for (int j = 0; j < COLS_TETRIS; j++) {
                    bool partePieza = false;

                    if (i >= actual.y && i < actual.y + actual.size && j >= actual.x && j < actual.x + actual.size) {
                        if (actual.matriz[i - actual.y][j - actual.x] != 0) {
                            partePieza = true;
                        }
                    }

                    if (partePieza) {
                        cout << COLORES_PIEZAS[actual.tipo] << "[]" << COLOR_RESET;
                    } else if (tableroTetris[i][j] != 0) {
                        cout << COLORES_PIEZAS[tableroTetris[i][j] - 1] << "[]" << COLOR_RESET;
                    } else {
                        cout << COLOR_GRIS << "  " << COLOR_RESET;
                    }
                }
                cout << "#" << endl;
            }

            for (int j = 0; j < (COLS_TETRIS * 2) + 2; j++) cout << "#";
            cout << endl;

            if (_kbhit()) {
                char t = tolower(_getch());

                if (t == 'a' && !comprobarColision(actual, actual.x - 1, actual.y, tableroTetris)) actual.x--;
                if (t == 'd' && !comprobarColision(actual, actual.x + 1, actual.y, tableroTetris)) actual.x++;
                if (t == 's' && !comprobarColision(actual, actual.x, actual.y + 1, tableroTetris)) actual.y++;

                if (t == 'w') {
                    PiezaTetris rotada = actual;
                    rotarPieza(rotada);
                    if (!comprobarColision(rotada, rotada.x, rotada.y, tableroTetris)) {
                        actual = rotada;
                    }
                }

                if (t == 'f') {
                    juegoActivo = false;
                    salirForzado = true;
                    break;
                }
            }

            if (!comprobarColision(actual, actual.x, actual.y + 1, tableroTetris)) {
                actual.y++;
            } else {
                fijarPieza(actual, tableroTetris);

                int filasSimultaneas = 0;
                for (int i = FILAS_TETRIS - 1; i >= 0; i--) {
                    bool llena = true;
                    for (int j = 0; j < COLS_TETRIS; j++) {
                        if (tableroTetris[i][j] == 0) llena = false;
                    }

                    if (llena) {
                        filasSimultaneas++;
                        filasEliminadas++;

                        for (int k = i; k > 0; k--) {
                            for (int j = 0; j < COLS_TETRIS; j++) {
                                tableroTetris[k][j] = tableroTetris[k - 1][j];
                            }
                        }
                        for (int j = 0; j < COLS_TETRIS; j++) tableroTetris[0][j] = 0;
                        i++;
                    }
                }

                if (filasSimultaneas == 1) puntaje += 100;
                if (filasSimultaneas == 2) puntaje += 300;
                if (filasSimultaneas == 3) puntaje += 500;
                if (filasSimultaneas == 4) puntaje += 800;

                asignarPieza(actual);
                piezasUsadas++;

                if (comprobarColision(actual, actual.x, actual.y, tableroTetris)) {
                    juegoActivo = false;
                }
            }

            Sleep(160);
        }

        mostrarOcultarCursorTetris(true);

        time_t tiempoFin = time(0);
        int tiempoTotalSegundos = (int)difftime(tiempoFin, tiempoInicio);

        system("cls");
        cout << COLOR_CIAN << "==============================================" << COLOR_RESET << endl;
        if (salirForzado) cout << "Has salido del juego de forma manual." << endl;
        else cout << COLOR_ROJO << "FIN DEL JUEGO: GAME OVER" << COLOR_RESET << endl;
        cout << COLOR_CIAN << "==============================================" << COLOR_RESET << endl;
        cout << "Jugador: " << nombreJugador << endl;
        cout << "Puntaje Definitivo: " << puntaje << " puntos." << endl;
        cout << "Filas Eliminadas: " << filasEliminadas << endl;
        cout << "Nivel alcanzado: " << (filasEliminadas / 4) + 1 << endl;
        cout << "Piezas utilizadas: " << piezasUsadas << endl;
        cout << "Tiempo de juego: " << tiempoTotalSegundos << " segundos." << endl;

        string fileC = nombreJugador + "_TETRIS.txt";
        ofstream outC(fileC.c_str());
        if (outC.is_open()) {
            outC << "Nombre del jugador: " << nombreJugador << endl;
            outC << "Nombre del juego: Tetris" << endl;
            outC << "Puntaje obtenido: " << puntaje << endl;
            outC << "Cantidad de filas eliminadas: " << filasEliminadas << endl;
            outC << "Nivel alcanzado: " << (filasEliminadas / 4) + 1 << endl;
            outC << "Numero de piezas utilizadas: " << piezasUsadas << endl;
            outC << "Tiempo de juego: " << tiempoTotalSegundos << " segundos" << endl;
            outC.close();
            cout << "\n[i] Resultados guardados en \"" << fileC << "\"" << endl;
        } else {
            cout << "\n[!] No se pudo escribir el archivo de resultados." << endl;
        }

        if (!leerConfirmacionSN("\nDeseas jugar otra partida de TETRIS? (S/N): ")) {
            quiereJugarDeNuevo = false;
        }
    }
}

void mostrarOcultarCursorSnake(bool visible) {
    HANDLE hConsola = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor;
    cursor.dwSize = 1;
    cursor.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(hConsola, &cursor);
}

bool posicionOcupadaPorSerpiente(PuntoSnake serpiente[], int longitud, int f, int c) {
    for (int i = 0; i < longitud; i++) {
        if (serpiente[i].fila == f && serpiente[i].columna == c) return true;
    }
    return false;
}

void generarComidaSnake(PuntoSnake serpiente[], int longitud, PuntoSnake &comida) {
    int f, c;
    do {
        f = rand() % FILAS_SNAKE;
        c = rand() % COLUMNAS_SNAKE;
    } while (posicionOcupadaPorSerpiente(serpiente, longitud, f, c));
    comida.fila = f;
    comida.columna = c;
}

void dibujarTableroSnake(PuntoSnake serpiente[], int longitud, PuntoSnake comida, const string &nombreJugador, int puntaje) {
    moverCursorInicio();

    cout << COLOR_CIAN << "==================== SNAKE ====================" << COLOR_RESET << endl;
    cout << "Jugador: " << nombreJugador << "   Puntaje: " << puntaje << "     " << endl;
    cout << "Longitud actual: " << longitud << "      " << endl << endl;

    for (int f = 0; f < FILAS_SNAKE; f++) {
        for (int c = 0; c < COLUMNAS_SNAKE; c++) {
            bool esSerpiente = posicionOcupadaPorSerpiente(serpiente, longitud, f, c);

            if (f == comida.fila && c == comida.columna) {
                cout << COLOR_ROJO << "* " << COLOR_RESET; // Comida en rojo
            } else if (esSerpiente) {
                cout << COLOR_VERDE << "O " << COLOR_RESET; // Serpiente en verde
            } else {
                cout << COLOR_GRIS << ". " << COLOR_RESET;
            }
        }
        cout << endl;
    }
    cout << "\nControles: Flechas = mover   |   F = salir          " << endl;
}

void leerTeclaSnake(char &direccionActual) {
    if (_kbhit()) {
        int tecla = _getch();
        char nuevaDireccion = direccionActual;

        if (tecla == 224) {
            int flecha = _getch();
            switch (flecha) {
                case 72: nuevaDireccion = 'W'; break; // Arriba
                case 80: nuevaDireccion = 'S'; break; // Abajo
                case 75: nuevaDireccion = 'A'; break; // Izquierda
                case 77: nuevaDireccion = 'D'; break; // Derecha
            }
        } else {
            char t = toupper((unsigned char)tecla);
            if (t == 'F') nuevaDireccion = 'F';
        }

        bool opuestas = (direccionActual == 'W' && nuevaDireccion == 'S') ||
                         (direccionActual == 'S' && nuevaDireccion == 'W') ||
                         (direccionActual == 'A' && nuevaDireccion == 'D') ||
                         (direccionActual == 'D' && nuevaDireccion == 'A');

        if (!opuestas) direccionActual = nuevaDireccion;
    }
}

bool moverSerpiente(PuntoSnake serpiente[], int &longitud, char direccionActual, PuntoSnake &comida,
                     int &puntaje, int &alimentosConsumidos, int &siguienteBonusLongitud, bool &bonus15Otorgado) {
    if (direccionActual == 'F') return false;

    PuntoSnake nuevaCabeza = serpiente[0];
    switch (direccionActual) {
        case 'W': nuevaCabeza.fila--;    break;
        case 'S': nuevaCabeza.fila++;    break;
        case 'A': nuevaCabeza.columna--; break;
        case 'D': nuevaCabeza.columna++; break;
    }

    if (nuevaCabeza.fila < 0 || nuevaCabeza.fila >= FILAS_SNAKE ||
        nuevaCabeza.columna < 0 || nuevaCabeza.columna >= COLUMNAS_SNAKE) {
        return false;
    }

    if (posicionOcupadaPorSerpiente(serpiente, longitud, nuevaCabeza.fila, nuevaCabeza.columna)) {
        return false;
    }

    bool comioAlimento = (nuevaCabeza.fila == comida.fila && nuevaCabeza.columna == comida.columna);
    int limite = comioAlimento ? longitud : longitud - 1;
    for (int i = limite; i > 0; i--) {
        serpiente[i] = serpiente[i - 1];
    }
    serpiente[0] = nuevaCabeza;

    if (comioAlimento) {
        longitud++;
        alimentosConsumidos++;
        puntaje += 10;

        if (longitud >= siguienteBonusLongitud) {
            puntaje += 25;
            siguienteBonusLongitud += 5;
        }
        if (longitud >= 15 && !bonus15Otorgado) {
            puntaje += 50;
            bonus15Otorgado = true;
        }

        generarComidaSnake(serpiente, longitud, comida);
    }

    return true;
}

// ==========================================
// IMPLEMENTACION: SNAKE
// ==========================================
void jugarSnake(string nombreJugador) {
    bool jugarDeNuevo = true;
    srand((unsigned int)time(0));

    while (jugarDeNuevo) {
        system("cls");
        cout << COLOR_CIAN << "=========================================================" << COLOR_RESET << endl;
        cout << COLOR_AMARILLO << "                    BIENVENIDO A SNAKE                   " << COLOR_RESET << endl;
        cout << COLOR_CIAN << "=========================================================" << COLOR_RESET << endl;
        cout << "Hola " << nombreJugador << ", esquiva los bordes y come la fruta.\n\n";
        cout << "REGLAS BASICAS:\n";
        cout << "1. Usa las flechas para moverte y comer la comida (*).\n";
        cout << "2. Cada fruta aumenta tu longitud y tu puntaje.\n";
        cout << "3. No choques con los bordes del mapa ni contigo mismo.\n\n";
        cout << "CONTROLES:\n";
        cout << "  Flechas Direccionales -> Mover serpiente\n";
        cout << "  F / f                 -> Salir del juego\n\n";
        cout << "Presiona cualquier tecla para comenzar...";
        _getch();

        system("cls");
        mostrarOcultarCursorSnake(false);

        PuntoSnake serpiente[LONGITUD_MAX_SNAKE];
        int longitud = 3;
        int filaInicial = FILAS_SNAKE / 2;
        int colInicial = COLUMNAS_SNAKE / 2;

        serpiente[0].fila = filaInicial; serpiente[0].columna = colInicial;
        serpiente[1].fila = filaInicial; serpiente[1].columna = colInicial - 1;
        serpiente[2].fila = filaInicial; serpiente[2].columna = colInicial - 2;

        char direccionActual = 'D'; // Arranca moviendose hacia la derecha
        int puntaje = 0;
        int alimentosConsumidos = 0;
        int siguienteBonusLongitud = 5;
        bool bonus15Otorgado = false;

        PuntoSnake comida;
        generarComidaSnake(serpiente, longitud, comida);

        time_t inicio = time(0);

        bool continuar = true;
        while (continuar) {
            dibujarTableroSnake(serpiente, longitud, comida, nombreJugador, puntaje);
            leerTeclaSnake(direccionActual);
            continuar = moverSerpiente(serpiente, longitud, direccionActual, comida,
                                        puntaje, alimentosConsumidos, siguienteBonusLongitud, bonus15Otorgado);
            Sleep(150);
        }

        mostrarOcultarCursorSnake(true);

        time_t fin = time(0);
        int segundos = (int)difftime(fin, inicio);

        system("cls");
        cout << COLOR_CIAN << "================ FIN DEL JUEGO ================" << COLOR_RESET << endl;
        cout << "Jugador: " << nombreJugador << endl;
        cout << "Puntaje final: " << puntaje << endl;
        cout << "Longitud final de la serpiente: " << longitud << endl;
        cout << "Alimentos consumidos: " << alimentosConsumidos << endl;
        cout << "Tiempo jugado: " << segundos << " segundos" << endl;

        string nombreArchivo = nombreJugador + "_SNAKE.txt";
        ofstream archivo(nombreArchivo);

        if (archivo.is_open()) {
            archivo << "Nombre del jugador: " << nombreJugador << endl;
            archivo << "Nombre del juego: SNAKE" << endl;
            archivo << "Puntaje final: " << puntaje << endl;
            archivo << "Longitud maxima alcanzada: " << longitud << endl;
            archivo << "Alimentos consumidos: " << alimentosConsumidos << endl;
            archivo << "Tiempo de juego (segundos): " << segundos << endl;
            archivo.close();
            cout << "\n[i] Resultados guardados en \"" << nombreArchivo << "\"" << endl;
        } else {
            cout << "\n[!] No se pudo guardar el archivo de resultados." << endl;
        }

        if (!leerConfirmacionSN("\nDeseas jugar otra partida de SNAKE? (S/N): ")) {
            jugarDeNuevo = false;
        }
    }
}
