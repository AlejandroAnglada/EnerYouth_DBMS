#include "../include/GestionTransmisionDistribucion.h"
#include "../include/ConexionADB.h"
#include <sqltypes.h>
#include <iostream>
#include <algorithm>

// Función para escapar comillas simples en cadenas SQL
std::string escapeSQL(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\'') result += "''"; // duplicamos la comilla simple
        else result += c;
    }
    return result;
}

GestionTransmisionDistribucion::GestionTransmisionDistribucion(ConexionADB& con)
    : conexion(con) {
}

bool GestionTransmisionDistribucion::altaHogar(const std::string& direccion, const std::string& dni_cliente, int id_contrato) {
    // Comprobamos que la conexión está establecida 
    if (!conexion.isConnected()) {
        return false;
    }

    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    // Inicializamos el handler para ejecutar la sentencia SQL
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Damos de alta el hogar
    char hogar[2048];
    std::string direccion_esc = escapeSQL(direccion);
    sprintf(hogar, "INSERT INTO Hogar (direccion, dni_cliente, id_contrato) VALUES ('%s', '%s', %d);", direccion_esc.c_str(), dni_cliente.c_str(), id_contrato);
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)hogar, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para dar de alta el hogar.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return true;
}

bool GestionTransmisionDistribucion::bajaHogar(const std::string& direccion) {
    // Comprobamos que la conexión está establecida 
    if (!conexion.isConnected()) {
        return false;
    }

    // Declaramos el handler para ejecutar la sentencia SQL
    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;

    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Damos de baja el hogar
    char hogar[2048];
    std::string direccion_esc = escapeSQL(direccion);
    sprintf(hogar, "DELETE FROM Hogar WHERE direccion = '%s';", direccion_esc.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)hogar, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para dar de baja el hogar.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return true;
}

std::vector<HogarInfo> GestionTransmisionDistribucion::listarHogares(const std::string& zona_geografica,
                                                                     const std::string& dni_cliente,
                                                                     const std::string& tipo_contrato){
    std::vector<HogarInfo> hogares;

    // Comprobamos la conexión
    if (!conexion.isConnected()) return hogares;

    // Declaramos el handler para ejecutar la sentencia SQL
    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;
    
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return hogares;
    }

    std::string consulta = "SELECT direccion, dni_cliente, tipo_contrato FROM Hogar";
    bool tiene_condicion = false;

    if (!zona_geografica.empty()) {
        if (tiene_condicion) {
            consulta += " AND";
        } else {
            consulta += " WHERE";
        }
        consulta += " zona_geografica = '" + escapeSQL(zona_geografica) + "'";
        tiene_condicion = true;
    }

    if (!dni_cliente.empty()) {
        if (tiene_condicion) {
            consulta += " AND";
        } else {
            consulta += " WHERE";
        }
        consulta += " dni_cliente = '" + escapeSQL(dni_cliente) + "'";
        tiene_condicion = true;
    }

    if (!tipo_contrato.empty()) {
        if (tiene_condicion) {
            consulta += " AND";
        } else {
            consulta += " WHERE";
        }
        consulta += " tipo_contrato = '" + escapeSQL(tipo_contrato) + "'";
    }

    consulta += ";";

    // Ejecutamos la consulta
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para listar los hogares.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return hogares;
    }

    // Buffers para obtener los datos
    char direccion_buf[101];
    char dni_buf[10];
    char tipo_buf[21];

    // Procesamos los resultados
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, direccion_buf, sizeof(direccion_buf), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, tipo_buf, sizeof(tipo_buf), NULL);

        HogarInfo hogar;
        hogar.direccion = direccion_buf;
        hogar.dni_cliente = dni_buf;
        hogar.tipo_contrato = tipo_buf;

        hogares.push_back(hogar);
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return hogares;
}

bool GestionTransmisionDistribucion::modificarHogar(const std::string& direccion, 
                                                    const std::string& dni_cliente,
                                                    const DatosContrato& datos_contrato) {
    // Comprobamos que la conexión está establecida 
    if (!conexion.isConnected()) {
        return false;
    }

    // Declaramos el handler para ejecutar la sentencia SQL
    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;
    
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Modificamos los datos del hogar
    char hogar[2048];
    std::string direccion_esc = escapeSQL(direccion);
    sprintf(hogar, "UPDATE Hogar SET dni_cliente = '%s' WHERE direccion = '%s';", dni_cliente.c_str(), direccion_esc.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)hogar, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para modificar el hogar.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return true;
}

bool GestionTransmisionDistribucion::registrarIncidencia(int id_incidencia, const std::string& tipo_incidencia,
                                                         const std::string& descripcion, int id_contrato,
                                                         const std::string& fecha_incidencia, const std::string& fecha_resolucion,
                                                         const std::string& estado_incidencia) {

    // Comprobamos que la conexión está establecida 
    if (!conexion.isConnected()) {
        return false;
    }

    // Declaramos el handler para ejecutar la sentencia SQL
    SQLHDBC con = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;
    
    if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
        return false;
    }

    // Registramos la incidencia
    char incidencia[4096];
    std::string descripcion_esc = escapeSQL(descripcion);
    sprintf(incidencia, "INSERT INTO Incidencia (id_incidencia, id_contrato, descripcion, tipo_incidencia, fecha_incidencia, fecha_resolucion, estado_incidencia) "
                        "VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s');",
                        id_incidencia, id_contrato, descripcion_esc.c_str(), tipo_incidencia.c_str(),
                        fecha_incidencia.c_str(), fecha_resolucion.c_str(), estado_incidencia.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)incidencia, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para registrar la incidencia.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return true;
}
