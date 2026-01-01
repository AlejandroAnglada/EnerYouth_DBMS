#include "../include/GestionClientes.h"
#include "../include/ConexionADB.h"
#include <sqltypes.h>
#include <iostream>
#include <regex>        // Para validaciones básicas
#include <sstream>      // Para conversiones simples
#include <ctime>        // Para obtener la fecha actual
#include <iomanip>      // Para formatear fechas

// Función para escapar comillas simples en cadenas SQL
std::string escapeSQLC(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\'') result += "''"; // duplicamos la comilla simple
        else result += c;
    }
    return result;
}

// Función para mostrar errores SQL
void mostrarErrorC(SQLHSTMT stmt) {
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
    std::string dni_esc = escapeSQLC(dni_cif);
    std::string consulta = "SELECT COUNT(*) FROM Cliente WHERE DNI_CIF = '" + dni_esc + "';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar cliente.\n";
        mostrarErrorC(handler);
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
    std::string tarifa_esc = escapeSQLC(tarifa);
    std::string consulta = "SELECT COUNT(*) FROM Tarifa WHERE Nombre_Tarifa = '" + tarifa_esc + "';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar tarifa.\n";
        mostrarErrorC(handler);
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
    std::string cups_esc = escapeSQLC(cups);
    std::string consulta = "SELECT COUNT(*) FROM Contrato WHERE CUPS = '" + cups_esc + "' AND Estado = 'Activo';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar contrato por CUPS.\n";
        mostrarErrorC(handler);
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

bool GestionClientes::existeContrato(int id_contrato) const {
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
    std::string consulta = "SELECT COUNT(*) FROM Contrato WHERE ID_Contrato = " + std::to_string(id_contrato) + ";";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar contrato.\n";
        mostrarErrorC(handler);
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
        mostrarErrorC(handler);
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
        mostrarErrorC(handler);
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

/* ======= MÉTODOS PÚBLICOS ======== */

/* ========================= RF-3.1 ========================= */
bool GestionClientes::altaCliente(const std::string& dni_cif,
                                  const std::string& nombre,
                                  const std::string& apellidos,
                                  const std::string& direccion,
                                  const std::string& telefono,
                                  const std::string& email) {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        std::cout << "Error: La conexión no está establecida.\n";
        return false;
    }

    // Validaciones de formato
    if (!validarFormatoDNI(dni_cif)) {
        std::cout << "Error: DNI/CIF con formato incorrecto.\n";
        return false;
    }

    if (!validarFormatoEmail(email)) {
        std::cout << "Error: Email con formato incorrecto.\n";
        return false;
    }

    if (!validarFormatoTelefono(telefono)) {
        std::cout << "Error: Teléfono con formato incorrecto (debe ser 9 dígitos).\n";
        return false;
    }

    // Verificar que el cliente no existe ya
    if (existeCliente(dni_cif)) {
        std::cout << "Error: El cliente con DNI " << dni_cif << " ya existe.\n";
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        std::cout << "Error: No se pudo asignar handle ODBC.\n";
        return false;
    }

    // Generamos siguiente ID_Cliente
    int id_cliente = generarSiguienteIDCliente();

    // Obtenemos fecha actual (YYYY-MM-DD) para el registro
    time_t ahora = time(nullptr);
    struct tm* timeinfo = localtime(&ahora);
    char fecha_registro[11];
    strftime(fecha_registro, sizeof(fecha_registro), "%Y-%m-%d", timeinfo);

    // Escapar valores
    std::string dni_esc = escapeSQLC(dni_cif);
    std::string nombre_esc = escapeSQLC(nombre);
    std::string apellidos_esc = escapeSQLC(apellidos);
    std::string direccion_esc = escapeSQLC(direccion);
    std::string telefono_esc = escapeSQLC(telefono);
    std::string email_esc = escapeSQLC(email);

    // Construimos la sentencia INSERT
    std::string consulta = "INSERT INTO Cliente (ID_Cliente, DNI_CIF, Nombre, Apellidos, Direccion, Telefono, Correo_Electronico, Estado, Fecha_Registro) "
                          "VALUES (" + std::to_string(id_cliente) + ", '" + dni_esc + "', '" + nombre_esc + "', '" + apellidos_esc + "', '"
                          + direccion_esc + "', '" + telefono_esc + "', '" + email_esc + "', 'Activo', '" + fecha_registro + "');";

    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para dar de alta al cliente.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Cliente " << nombre << " " << apellidos << " dado de alta correctamente.\n";

    return true;
}

/* ========================= RF-3.2 ========================= */
bool GestionClientes::consultarCliente(const std::string& dni_cif,
                                       ClienteInfo& info) {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        std::cout << "Error: La conexión no está establecida.\n";
        return false;
    }

    // Verificar que el cliente existe
    if (!existeCliente(dni_cif)) {
        std::cout << "Error: El cliente con DNI " << dni_cif << " no existe.\n";
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        std::cout << "Error: No se pudo asignar handle ODBC.\n";
        return false;
    }

    // Ejecutamos la consulta
    std::string dni_esc = escapeSQLC(dni_cif);
    std::string consulta = "SELECT ID_Cliente, DNI_CIF, Nombre, Apellidos, Direccion, Telefono, Correo_Electronico, Estado, Fecha_Registro, Fecha_Baja, Motivo_Baja "
                          "FROM Cliente WHERE DNI_CIF = '" + dni_esc + "';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para consultar cliente.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Obtenemos los datos del cliente
    if (SQLFetch(handler) != SQL_SUCCESS) {
        std::cout << "Error al obtener los datos del cliente.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Buffers para obtener los datos
    char id_buf[10];
    char dni_buf[10];
    char nombre_buf[51];
    char apellidos_buf[51];
    char direccion_buf[101];
    char telefono_buf[10];
    char email_buf[51];
    char estado_buf[20];
    char fecha_registro_buf[11];
    char fecha_baja_buf[11];
    char motivo_baja_buf[201];

    // Obtenemos los datos
    SQLGetData(handler, 1, SQL_C_CHAR, id_buf, sizeof(id_buf), NULL);
    SQLGetData(handler, 2, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
    SQLGetData(handler, 3, SQL_C_CHAR, nombre_buf, sizeof(nombre_buf), NULL);
    SQLGetData(handler, 4, SQL_C_CHAR, apellidos_buf, sizeof(apellidos_buf), NULL);
    SQLGetData(handler, 5, SQL_C_CHAR, direccion_buf, sizeof(direccion_buf), NULL);
    SQLGetData(handler, 6, SQL_C_CHAR, telefono_buf, sizeof(telefono_buf), NULL);
    SQLGetData(handler, 7, SQL_C_CHAR, email_buf, sizeof(email_buf), NULL);
    SQLGetData(handler, 8, SQL_C_CHAR, estado_buf, sizeof(estado_buf), NULL);
    SQLGetData(handler, 9, SQL_C_CHAR, fecha_registro_buf, sizeof(fecha_registro_buf), NULL);

    // Controlamos los campos que pueden ser NULL
    SQLLEN len_fecha_baja = 0;
    SQLLEN len_motivo_baja = 0;
    SQLGetData(handler, 10, SQL_C_CHAR, fecha_baja_buf, sizeof(fecha_baja_buf), &len_fecha_baja);
    SQLGetData(handler, 11, SQL_C_CHAR, motivo_baja_buf, sizeof(motivo_baja_buf), &len_motivo_baja);

    // Llenamos la estructura
    info.id_cliente = std::stoi(id_buf);
    info.dni_cif = std::string(dni_buf);
    info.nombre = std::string(nombre_buf);
    info.apellidos = std::string(apellidos_buf);
    info.direccion = std::string(direccion_buf);
    info.telefono = std::string(telefono_buf);
    info.email = std::string(email_buf);
    info.estado = std::string(estado_buf);
    info.fecha_registro = std::string(fecha_registro_buf);

    // Controlamos los posibles campos NULL
    if (len_fecha_baja == SQL_NULL_DATA) {
        info.fecha_baja = "";
    } else {
        info.fecha_baja = std::string(fecha_baja_buf);
    }
    
    if (len_motivo_baja == SQL_NULL_DATA) {
        info.motivo_baja = "";
    } else {
        info.motivo_baja = std::string(motivo_baja_buf);
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    // Mostramos la información del cliente
    std::cout << "\n";
    std::cout << "------------------------------------------------------------------\n";
    std::cout << "|                        FICHA DEL CLIENTE                        |\n";
    std::cout << "------------------------------------------------------------------\n";
    std::cout << "\nDatos Personales:\n";
    std::cout << "  - ID Cliente:        " << info.id_cliente << "\n";
    std::cout << "  - DNI/CIF:           " << info.dni_cif << "\n";
    std::cout << "  - Nombre:            " << info.nombre << "\n";
    std::cout << "  - Apellidos:         " << info.apellidos << "\n";
    std::cout << "  - Dirección:         " << info.direccion << "\n";
    std::cout << "  - Teléfono:          " << info.telefono << "\n";
    std::cout << "  - Email:             " << info.email << "\n";
    std::cout << "\nEstado y Fechas:\n";
    std::cout << "  - Estado:            " << info.estado << "\n";
    std::cout << "  - Fecha Registro:    " << info.fecha_registro << "\n";

    if (info.fecha_baja.empty()) {
        std::cout << "  - Fecha Baja:        (No registrada)\n";
        std::cout << "  - Motivo Baja:       (No registrada)\n";
    } else {
        std::cout << "  - Fecha Baja:        " << info.fecha_baja << "\n";
        std::cout << "  - Motivo Baja:       " << info.motivo_baja << "\n";
    }
    std::cout << "\n";

    return true;
}

/* ========================= RF-3.3 ========================= */
bool GestionClientes::actualizarCliente(const std::string& dni_cif,
                                       const std::string& campo,
                                       const std::string& valor_nuevo) {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        std::cout << "Error: La conexión no está establecida.\n";
        return false;
    }

    // Verificar que el cliente existe
    if (!existeCliente(dni_cif)) {
        std::cout << "Error: El cliente con DNI " << dni_cif << " no existe.\n";
        return false;
    }

    // Mapear campos permitidos a columnas de BD
    std::string columna;
    if (campo == "nombre" || campo == "Nombre" || campo == "NOMBRE") {
        columna = "Nombre";
    } else if (campo == "apellidos" || campo == "Apellidos" || campo == "APELLIDOS") {
        columna = "Apellidos";
    } else if (campo == "direccion" || campo == "Direccion" || campo == "DIRECCION") {
        columna = "Direccion";
    } else if (campo == "telefono" || campo == "Telefono" || campo == "TELEFONO") {
        columna = "Telefono";
    } else if (campo == "email" || campo == "Email" || campo == "EMAIL") {
        columna = "Correo_Electronico";
    } else {
        std::cout << "Error: Campo '" << campo << "' no válido.\n";
        std::cout << "Campos permitidos: nombre, apellidos, direccion, telefono, email.\n";
        return false;
    }

    // Validar según el tipo de campo
    if (campo == "telefono" && !validarFormatoTelefono(valor_nuevo)) {
        std::cout << "Error: Teléfono con formato incorrecto (debe ser 9 dígitos).\n";
        return false;
    }

    if (campo == "email" && !validarFormatoEmail(valor_nuevo)) {
        std::cout << "Error: Email con formato incorrecto.\n";
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        std::cout << "Error: No se pudo asignar handle ODBC.\n";
        return false;
    }

    // Escapar valores
    std::string dni_esc = escapeSQLC(dni_cif);
    std::string valor_esc = escapeSQLC(valor_nuevo);

    // Construimos la sentencia UPDATE
    std::string consulta = "UPDATE Cliente SET " + columna + " = '" + valor_esc + "' WHERE DNI_CIF = '" + dni_esc + "';";

    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para actualizar cliente.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Campo '" << campo << "' actualizado correctamente a: " << valor_nuevo << "\n";
    return true;
}

/* ========================= RF-3.4 ========================= */
bool GestionClientes::crearContrato(const std::string& dni_cif,
                                    const std::string& cups,
                                    const std::string& tipo_contrato,
                                    double potencia_con,
                                    const std::string& tarifa,
                                    const std::string& iban,
                                    const std::string& fecha_inicio,
                                    const std::string& fecha_fin) {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        std::cout << "Error: La conexión no está establecida.\n";
        return false;
    }

    // Verificar que el cliente existe
    if (!existeCliente(dni_cif)) {
        std::cout << "Error: El cliente con DNI " << dni_cif << " no existe.\n";
        return false;
    }

    // Validar potencia
    if (!validarPotencia(potencia_con)) {
        std::cout << "Error: La potencia contratada debe ser mayor a 3 kW.\n";
        return false;
    }

    // Validar IBAN
    if (!validarFormatoIBAN(iban)) {
        std::cout << "Error: IBAN con formato incorrecto.\n";
        return false;
    }

    // Validar que la tarifa existe
    if (!existeTarifa(tarifa)) {
        std::cout << "Error: La tarifa '" << tarifa << "' no existe en el sistema.\n";
        return false;
    }

    // Validar que no existe contrato activo con ese CUPS
    if (existeContratoActivoPorCUPS(cups)) {
        std::cout << "Error: Ya existe un contrato activo con CUPS " << cups << ".\n";
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        std::cout << "Error: No se pudo asignar handle ODBC.\n";
        return false;
    }

    // Generamos el siguiente ID_Contrato
    int id_contrato = generarSiguienteIDContrato();

    // Escapar valores
    std::string dni_esc = escapeSQLC(dni_cif);
    std::string cups_esc = escapeSQLC(cups);
    std::string tipo_esc = escapeSQLC(tipo_contrato);
    std::string tarifa_esc = escapeSQLC(tarifa);
    std::string iban_esc = escapeSQLC(iban);

    // Construimos la sentencia INSERT
    std::string consulta = "INSERT INTO Contrato (ID_Contrato, DNI_CIF, CUPS, Tipo_Contrato, Potencia_Con, Tarifa, IBAN, Fecha_Inicio, Fecha_Fin, Estado) "
                          "VALUES (" + std::to_string(id_contrato) + ", '" + dni_esc + "', '" + cups_esc + "', '" + tipo_esc + "', "
                          + std::to_string(potencia_con) + ", '" + tarifa_esc + "', '" + iban_esc + "', '" + fecha_inicio + "', ";
    
    // Controlamos que fecha_fin pueda ser opcional
    if (fecha_fin.empty()) {
        consulta += "NULL, 'Activo');";
    } else {
        consulta += "'" + fecha_fin + "', 'Activo');";
    }

    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para crear contrato.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "\nContrato creado correctamente:\n";
    std::cout << "  - ID Contrato:       " << id_contrato << "\n";
    std::cout << "  - DNI/CIF Cliente:   " << dni_cif << "\n";
    std::cout << "  - CUPS:              " << cups << "\n";
    std::cout << "  - Tipo Contrato:     " << tipo_contrato << "\n";
    std::cout << "  - Potencia:          " << potencia_con << " kW\n";
    std::cout << "  - Tarifa:            " << tarifa << "\n";
    std::cout << "  - Estado:            Activo\n";
    std::cout << "  - Fecha Inicio:      " << fecha_inicio << "\n";

    return true;
}

bool GestionClientes::finalizarContrato(int id_contrato) {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        std::cout << "Error: La conexión no está establecida.\n";
        return false;
    }

    // Verificar que el contrato existe
    if (!existeContrato(id_contrato)) {
        std::cout << "Error: El contrato con ID " << id_contrato << " no existe.\n";
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        std::cout << "Error: No se pudo asignar handle ODBC.\n";
        return false;
    }

    // Obtener fecha actual (YYYY-MM-DD)
    time_t ahora = time(nullptr);
    struct tm* timeinfo = localtime(&ahora);
    char fecha_fin[11];
    strftime(fecha_fin, sizeof(fecha_fin), "%Y-%m-%d", timeinfo);

    // Construimos la sentencia UPDATE
    std::string consulta = "UPDATE Contrato SET Estado = 'Finalizado', Fecha_Fin = '" + std::string(fecha_fin) + "' WHERE ID_Contrato = " + std::to_string(id_contrato) + ";";

    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para finalizar contrato.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Contrato ID " << id_contrato << " finalizado correctamente.\n";
    std::cout << "Fecha de finalización: " << fecha_fin << "\n";

    return true;
}

/* ========================= RF-3.5 ========================= */
bool GestionClientes::bajaCliente(const std::string& dni_cif,
                                  const std::string& fecha_baja,
                                  const std::string& motivo_baja) {
    // Comprobar que la conexión esté establecida
    if (!conexion.isConnected()) {
        std::cout << "Error: La conexión no está establecida.\n";
        return false;
    }

    // Verificar que el cliente existe
    if (!existeCliente(dni_cif)) {
        std::cout << "Error: El cliente con DNI " << dni_cif << " no existe.\n";
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        std::cout << "Error: No se pudo asignar handle ODBC.\n";
        return false;
    }

    // Verificar que el cliente no tiene contratos activos
    std::string dni_esc = escapeSQLC(dni_cif);
    std::string consulta_contratos = "SELECT COUNT(*) FROM Contrato WHERE DNI_CIF = '" + dni_esc + "' AND Estado = 'Activo';";
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta_contratos.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para verificar contratos activos.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Obtenemos resultado de contratos activos
    if (SQLFetch(handler) != SQL_SUCCESS) {
        std::cout << "Error al obtener el número de contratos activos.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    SQLINTEGER num_contratos_activos = 0;
    SQLGetData(handler, 1, SQL_C_SLONG, &num_contratos_activos, 0, NULL);

    // Si tiene contratos activos no se puede dar de baja
    if (num_contratos_activos > 0) {
        std::cout << "Error: No se puede dar de baja al cliente. Tiene " << num_contratos_activos << " contrato(s) activo(s).\n";
        std::cout << "Por favor, finalice los contratos antes de dar de baja al cliente.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Limpiar el statement para la siguiente consulta
    SQLFreeStmt(handler, SQL_CLOSE);

    // Escapar valores
    std::string motivo_esc = escapeSQLC(motivo_baja);

    // Sentencia UPDATE para dar de baja
    std::string consulta_baja = "UPDATE Cliente SET Estado = 'Baja', Fecha_Baja = '" + fecha_baja + "', Motivo_Baja = '" + motivo_esc + "' WHERE DNI_CIF = '" + dni_esc + "';";

    ret = SQLExecDirectA(handler, (SQLCHAR*)consulta_baja.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para dar de baja al cliente.\n";
        mostrarErrorC(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Cliente con DNI " << dni_cif << " dado de baja correctamente.\n";
    std::cout << "Motivo de baja: " << motivo_baja << "\n";
    std::cout << "Fecha de baja: " << fecha_baja << "\n";

    return true;
}