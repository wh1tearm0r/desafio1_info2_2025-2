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

int main()
{
    int cantidadArchivos = 0;
    int tamano = 0;

    cout << "Ingrese la cantidad de archivos encriptados a procesar: ";
    cin >> cantidadArchivos;

    for (int i = 1; i <= cantidadArchivos; i++) { // En este ciclo la idea es realizar los procesos de desencriptación archivo por archivo
        char nombreArchivo[500];
        sprintf(nombreArchivo, "C:\\Users\\eeval\\Desktop\\informatica ii\\test_desafio1\\datasetDesarrollo\\Encriptado%d.txt", i);

        cout << "\nProcesando archivo: " << nombreArchivo << endl;

        // Escribir el contenido del archivo en un arreglo dinámico
        char* contenido = leerArchivoEnArreglo(nombreArchivo, tamano);

        if (contenido != nullptr) {
            cout << "Archivo leído exitosamente!" << endl;
            cout << "Tamaño del archivo: " << tamano << " caracteres" << endl; //Tamaño del arreglo dependiendo de la cantidad de caracteres

            // Mostrar algunos caracteres para verificación (en hexadecimal, para debugging, borrar luego para implementacion y prueba de otros procesos)
            for (int j = 0; j < min(10, tamano); j++) {
                cout << hex << (int)(unsigned char)contenido[j] << " ";
            }
            cout << dec << endl;

            // Liberar memoria del arreglo dinámico
            delete[] contenido;
        } else {
            cout << "No se pudo leer el archivo." << endl;
        }
    }
}
