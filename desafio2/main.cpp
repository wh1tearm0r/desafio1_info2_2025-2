#include <iostream>
#include <fstream>

using namespace std;

char* leerArchivoEnArreglo(const char* nombreArchivo, int& tamano) {
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        cout << "Verifica que el archivo existe y la ruta es correcta." << endl;
        return nullptr;
    }

    // Obtener el tamaño del archivo
    archivo.seekg(0, ios::end);
    tamano = archivo.tellg();
    archivo.seekg(0, ios::beg);

    // Crear arreglo dinámico de caracteres
    char* contenido = new char[tamano + 1]; // +1 para el carácter nulo

    // Leer el contenido del archivo
    archivo.read(contenido, tamano);
    contenido[tamano] = '\0'; // Agregar carácter nulo al final

    archivo.close();

    return contenido;
}

unsigned char rotarDerecha(unsigned char byte, int posiciones) {
    posiciones &= 7;
    if (posiciones == 0) return byte;
    return (unsigned char)(((byte >> posiciones) | (byte << (8 - posiciones))) & 0xFF);
}

int main()
{
    int cantidadArchivos = 0;
    int tamano = 0;

    cout << "Ingrese la cantidad de archivos encriptados a procesar: ";
    cin >> cantidadArchivos;

    for (int i = 1; i <= cantidadArchivos; i++) { // En este ciclo la idea es realizar los procesos de desencriptación archivo por archivo
        char nombreArchivo[500];
        sprintf(nombreArchivo, "..\\..\\datasetDesarrollo\\Encriptado%d.txt", i);

        cout << "\nProcesando archivo: " << nombreArchivo << endl;

        // Escribir el contenido del archivo en un arreglo dinámico
        char* contenido = leerArchivoEnArreglo(nombreArchivo, tamano);

        if (contenido != nullptr) {
            cout << "Archivo leído exitosamente!" << endl;
            cout << "Tamaño del archivo: " << tamano << " caracteres" << endl; //Tamaño del arreglo dependiendo de la cantidad de caracteres

            cout << "Iniciando Proceso de desencriptacion..." << endl;

            // Proceso EXHAUSTIVO para desencriptar con XOR (mascara/clave) y rotacion de bits;
            // Son dos procesos: primero la rotacion y luego la mascara

            /* Primero se aplica una mascara XOR del bit 1 a 255, por cada intento de xor se aplica una rotacion de 1 a 7 bits a la derecha hasta
             encontrar el texto comprimido en RLE o LZ78, este proceso se hace caracter por caracter (despues se crean las funciones para verificar el tipo de texto) */

            //Memoria dinamica para el texto desencriptado
            char* dec = new char[tamano + 1]; // +1 para el carácter nulo

            bool encontrado = false;
            for (int mascara = 1; mascara <= 255 && !encontrado; mascara++) {
                for (int rotacion = 1; rotacion <= 7 && !encontrado; rotacion++) {
                    for (int j = 0; j < tamano; j++) {
                        unsigned char byteOriginal = static_cast<unsigned char>(contenido[j]);
                        unsigned char byteXOR = byteOriginal ^ static_cast<unsigned char>(mascara);
                        unsigned char byteRotado = rotarDerecha(byteXOR, rotacion);
                        dec[j] = static_cast<char>(byteRotado);

                    }
                    dec[tamano] = '\0'; // Agregar carácter nulo al final

                    // Aquí se debería verificar si 'dec' es un texto válido en RLE o LZ78

                    //Debugging para probar con una mascara y rotacion especifica
                    if (mascara == 0x5A && rotacion == 3) { // Cambiar estos valores para probar diferentes combinaciones
                        cout << "\nTexto desencriptado con mascara " << mascara << " y rotacion " << rotacion << ":\n";
                        cout << "----------------------------------------" << endl;
                        cout << dec << endl;
                        cout << "----------------------------------------" << endl;
                        encontrado = true; // Salir de los bucles
                    }
                }
            }

            // Liberar memoria del arreglo dinámico
            delete[] contenido;
            delete[] dec;
        } else {
            cout << "No se pudo leer el archivo." << endl;
        }
    }
    return 0;
}
