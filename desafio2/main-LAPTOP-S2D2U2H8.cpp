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
    posiciones = posiciones % 8;
    return (byte >> posiciones) | (byte << (8 - posiciones));
}


// Función mejorada para filtrar caracteres nulos con múltiples opciones
char* filtrarCaracteresNulos(const char* decodificado, int tamano) {
    bool mantenerNulosConsecutivos = false;

    // Validar parámetros de entrada
    if (decodificado == nullptr || tamano <= 0) {
        return nullptr;
    }

    // Contar caracteres válidos para optimizar la memoria
    int caracteresValidos = 0;
    for (int i = 0; i < tamano; i++) {
        if (decodificado[i] != '\0') {
            caracteresValidos++;
        } else if (mantenerNulosConsecutivos && i + 1 < tamano && decodificado[i + 1] == '\0') {
            caracteresValidos++; // Solo contar el primer nulo de una secuencia
        }
    }

    // Crear arreglo filtrado con el tamaño exacto necesario
    char* filtrado = new char[caracteresValidos + 1]; // +1 para el carácter nulo final

    int j = 0;
    for (int i = 0; i < tamano; i++) {
        if (decodificado[i] != '\0') {
            filtrado[j++] = decodificado[i];
        }
        else if (mantenerNulosConsecutivos) {
            if (j == 0 || filtrado[j - 1] != '\0') { // Evitar duplicados
                filtrado[j++] = '0';
            }
        }
    }

    filtrado[j] = '\0'; // Asegurar que termine con carácter nulo

    return filtrado;
}

char* descompresionRLE(const char* comprimido, char* descomprimido) {
    /*
    Funcion que permitira aplicar descompresion por el metodo RLE
    */
    int i = 0, j = 0;

    while (comprimido[i] != '\0'){
        int num = 0;

        // Mientras que el caracter que se este leyendo sea un numero
        while(comprimido[i] >= '1' && comprimido[i] <= '9'){
            num =  num * 10 + (comprimido[i] - '0');
            i++;
        }

        char simbolo = comprimido[i];
        i++;

        for(int k = 0; k < num; k++){
            descomprimido[j] = simbolo;
            j++;
        }
    }
    descomprimido[j] = '\0'; //que termine en caracter nulo para cortar la cadena

    return descomprimido;
}

char* descompresionLZ78(const char* comprimido, char* descomprimido) {

    const int espacios = 512;
    const int tamañomax = 1024;
    char dic[espacios][tamañomax];
    int tamañodic = 0;

    int i = 0, j = 0;

    while (comprimido[i] != '\0') {
        int indice = 0;
        while (comprimido[i] >= '0' && comprimido[i] <= '9') {
            indice = indice * 10 + (comprimido[i] - '0');
            i++;
        }

        char simbolo = comprimido[i];
        i++;

        char nueva[tamañomax];
        int k = 0;

        if (indice > 0) {
            for (int l = 0; dic[indice][l] != '\0'; l++ ) {
                nueva[k++] = dic[indice][l];
            }
        }

        nueva[k++] = simbolo;
        nueva[k] = '\0';

        for (int m = 0; nueva[m] != '\0'; m++) {
            descomprimido[j++] = nueva[m];
        }
        descomprimido[j] = '\0';

        tamañodic++;
        int m = 0;
        for (; nueva[m] != '\0'; m++) {
            dic[tamañodic][m] = nueva[m];
        }
        dic[tamañodic][m] = '\0';
    }

    return descomprimido;
}


bool esRLE (const char* texto) {
    /*La logica de la funcion es la siguiente:
     * Se recorre el arreglo filtrado caracter por caracter, tomando dos caracteres, uno verificando el numero y otro la letra*/
    int i = 0;
    while (texto[i] != '\0') {
        // Verificar que el primer caracter sea un dígito
        if (texto[i] < '0' || texto[i] > '9') {
            return false; // No es un dígito
        }
        i++;
        // Verificar que el siguiente caracter sea una letra (mayúscula o minúscula)
        if (!((texto[i] >= 'A' && texto[i] <= 'Z') || (texto[i] >= 'a' && texto[i] <= 'z'))) {
            return false; // No es una letra
        }
        i++;
    }
    return true;
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
                        if (byteRotado == 0) {
                            dec[j] = '\0'; // Mantener el carácter nulo
                        }
                        else if (byteRotado < 48) {
                            byteRotado += 48; // Ajuste para que el numero esté en el rango imprimible
                            dec[j] = static_cast<char>(byteRotado);
                        }
                        else {
                            dec[j] = static_cast<char>(byteRotado);
                        }
                    }
                    dec[tamano] = '\0'; // Agregar carácter nulo al final

                    // for (int k = 0; k <= 2 && !encontrado; k++)
                    char* filtrado = filtrarCaracteresNulos(dec, tamano);
                    if (esRLE(filtrado)){
                        char* descomprimido = new char[tamano*4];
                        char* txtfinal = descompresionRLE(filtrado, descomprimido);
                        char formato[4] = "RLE";
                        cout << "Mascara usada: " << mascara << endl << "Rotacion de bits aplicada: " << rotacion << endl  << "Formato de compresion usado: " << formato << endl << "Texto Original: " << txtfinal << endl;
                        encontrado = true; // Salir de ambos bucles

                        delete[] descomprimido;
                    }
                    else if() {
                        char* descomprimido = new char[tamano*4];
                        char* txtfinal = descompresionLz78(filtrado, descomprimido);
                        char formato[5] = "LZ78";
                        cout << "Mascara usada: " << mascara << endl << "Rotacion de bits aplicada: " << rotacion << endl  << "Formato de compresion usado: " << formato << endl << "Texto Original: " << txtfinal << endl;
                        encontrado = true;

                        delete[] descomprimido;
                    }

                    delete[] filtrado;

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
