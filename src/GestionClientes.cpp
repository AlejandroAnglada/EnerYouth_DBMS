#include "../include/GestionClientes.h"
#include "../include/ConexionADB.h"
#include <iostream>
#include <regex>        // Para validaciones básicas
#include <sstream>      // Para conversiones simples
#include <ctime>        // Para obtener la fecha actual
#include <iomanip>      // Para formatear fechas

// Función para escapar comillas simples en cadenas SQL
std::string escapeSQLE(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\'') result += "''"; // duplicamos la comilla simple
        else result += c;
    }
    return result;
}

// Función para mostrar errores SQL
void mostrarErrorE(SQLHSTMT stmt) {
    SQLCHAR sqlState[6], msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT msgLen;

    SQLGetDiagRecA(
        SQL_HANDLE_STMT,
        stmt,
        1,
        sqlState,
        &nativeError,
        msg,
        sizeof(msg),
        &msgLen
    );

    std::cerr << "SQLSTATE: " << sqlState << "\n";
    std::cerr << "Mensaje : " << msg << "\n";
}

// Constructor
GestionClientes::GestionClientes(ConexionADB& con) : conexion(con) {
    //La conexion se inicializa en la lista de inicialización
}

/* ======= MÉTODOS PRIVADOS ======== */

bool GestionClientes::validarFormatoDNI(const std::string& dni_cif) const {
    if (dni_cif.empty() || dni_cif.length() > 9)
        return false;

    // Expresiones regulares para validar DNI/CIF
    // Para DNI: 8 dígitos seguidos de una letra
    std::regex patron_dni("^[0-9]{8}[A-Za-z]$");
    // Para CIF: 1 letra, 8 dígitos
    std::regex patron_cif("^[A-Za-z][0-9]{8}$");

    return std::regex_match(dni_cif, patron_dni) || std::regex_match(dni_cif, patron_cif);
}

bool GestionClientes::validarFormatoEmail(const std::string& email) const {
    // Expresión regular para validar correo electrónico (blabla@blabla.blabla)
    std::regex patron_email("^[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return std::regex_match(email, patron_email);
}

bool GestionClientes::validarFormatoTelefono(const std::string& telefono) const {
    if (telefono.length() != 9)
        return false;

    // Expresión regular para validar que todo son números
    std::regex patron_telefono("^[0-9]{9}$");
    return std::regex_match(telefono, patron_telefono);
}

bool GestionClientes::validarFormatoIBAN(const std::string& iban) const {
    if (iban.empty() || iban.length() > 34)
        return false;
    // Expresión regular para validar IBAN
    std::regex patron_iban("^[A-Z]{2}[0-9]{2}[A-Z0-9]{1,30}$");
    return std::regex_match(iban, patron_iban);
}

bool GestionClientes::validarPotencia(double potencia) const {
    return potencia > 3.0;
}

bool GestionClientes::existeCliente(const std::string& dni_cif) const {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Ejecutamos la consulta
    std::string dni_esc = escapeSQLE(dni_cif);
    std::string consulta = "SELECT COUNT(*) FROM Cliente WHERE DNI_CIF = '" + dni_esc + "';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar cliente.\n";
        mostrarErrorE(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Comprobamos si existe el cliente
    if (SQLFetch(handler) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Obtenemos el resultado
    SQLINTEGER count = 0;
    SQLGetData(handler, 1, SQL_C_SLONG, &count, 0, NULL);

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return count > 0;
}

bool GestionClientes::existeTarifa(const std::string& tarifa) const {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Ejecutamos la consulta
    std::string tarifa_esc = escapeSQLE(tarifa);
    std::string consulta = "SELECT COUNT(*) FROM Tarifa WHERE Nombre_Tarifa = '" + tarifa_esc + "';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar tarifa.\n";
        mostrarErrorE(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Comprobamos si existe la tarifa
    if (SQLFetch(handler) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Obtenemos el resultado
    SQLINTEGER count = 0;
    SQLGetData(handler, 1, SQL_C_SLONG, &count, 0, NULL);

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return count > 0;
}

bool GestionClientes::existeContratoActivoPorCUPS(const std::string& cups) const {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Ejecutamos la consulta
    std::string cups_esc = escapeSQLE(cups);
    std::string consulta = "SELECT COUNT(*) FROM Contrato WHERE CUPS = '" + cups_esc + "' AND Estado = 'Activo';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar contrato por CUPS.\n";
        mostrarErrorE(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Comprobamos si existe el contrato
    if (SQLFetch(handler) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Obtenemos el resultado
    SQLINTEGER count = 0;
    SQLGetData(handler, 1, SQL_C_SLONG, &count, 0, NULL);

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return count > 0;
}

int GestionClientes::generarSiguienteIDCliente() const {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        return -1;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return -1;
    }

    // Ejecutamos la consulta
    std::string consulta = "SELECT MAX(ID_Cliente) FROM Cliente;";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para generar siguiente ID de cliente.\n";
        mostrarErrorE(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return -1;
    }

    // Comprobamos si hay resultados
    if (SQLFetch(handler) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return 1; // Si no hay clientes, empezamos desde 1
    }

    // Obtenemos el resultado
    SQLINTEGER max_id = 0;
    SQLGetData(handler, 1, SQL_C_SLONG, &max_id, 0, NULL);

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return max_id + 1;
}

int GestionClientes::generarSiguienteIDContrato() const {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        return -1;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return -1;
    }

    // Ejecutamos la consulta
    std::string consulta = "SELECT MAX(ID_Contrato) FROM Contrato;";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para generar siguiente ID de contrato.\n";
        mostrarErrorE(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return -1;
    }

    // Comprobamos si hay resultados
    if (SQLFetch(handler) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return 1; // Si no hay contratos, empezamos desde 1
    }

    // Obtenemos el resultado
    SQLINTEGER max_id = 0;
    SQLGetData(handler, 1, SQL_C_SLONG, &max_id, 0, NULL);

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return max_id + 1;
}