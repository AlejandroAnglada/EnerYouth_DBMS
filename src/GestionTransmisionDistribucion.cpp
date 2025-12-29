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

// Función para mostrar errores SQL
void mostrarError(SQLHSTMT stmt) {
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

GestionTransmisionDistribucion::GestionTransmisionDistribucion(ConexionADB& con)
    : conexion(con) {
}

bool GestionTransmisionDistribucion::altaHogar(const std::string& direccion, const std::string& dni_cliente, 
                                               int id_contrato, const std::string& tipo_contrato, const std::string& zona_geografica) {
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
    std::string tipo_contrato_esc = escapeSQL(tipo_contrato);
    std::string zona_geografica_esc = escapeSQL(zona_geografica);
    sprintf(hogar, "INSERT INTO Hogar (Direccion, DNI_Cliente, ID_Contrato_H, Tipo_Contrato_H, Zona_Geografica) VALUES ('%s', '%s', %d, '%s', '%s');", direccion_esc.c_str(), dni_cliente.c_str(), id_contrato, tipo_contrato_esc.c_str(), zona_geografica_esc.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)hogar, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para dar de alta el hogar.\n";
        mostrarError(handler);
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
    sprintf(hogar, "DELETE FROM Hogar WHERE Direccion = '%s';", direccion_esc.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)hogar, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para dar de baja el hogar.\n";
        mostrarError(handler);
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

    std::string consulta = "SELECT Direccion, DNI_Cliente, Tipo_Contrato_H FROM Hogar";
    bool tiene_condicion = false;

    if (!zona_geografica.empty()) {
        if (tiene_condicion) {
            consulta += " AND";
        } else {
            consulta += " WHERE";
        }
        consulta += " Zona_Geografica = '" + escapeSQL(zona_geografica) + "'";
        tiene_condicion = true;
    }

    if (!dni_cliente.empty()) {
        if (tiene_condicion) {
            consulta += " AND";
        } else {
            consulta += " WHERE";
        }
        consulta += " DNI_Cliente = '" + escapeSQL(dni_cliente) + "'";
        tiene_condicion = true;
    }

    if (!tipo_contrato.empty()) {
        if (tiene_condicion) {
            consulta += " AND";
        } else {
            consulta += " WHERE";
        }
        consulta += " Tipo_Contrato_H = '" + escapeSQL(tipo_contrato) + "'";
    }

    consulta += ";";

    // Ejecutamos la consulta
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para listar los hogares.\n";
        mostrarError(handler);
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

bool GestionTransmisionDistribucion::modificarHogar(const std::string& direccion, const std::string& dni_cliente,
                                                    const std::string& nuevo_dni_cliente,const DatosContrato& datos_contrato) {
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

    std::string direccion_esc = escapeSQL(direccion);
    std::string nuevo_dni_cliente_esc = escapeSQL(nuevo_dni_cliente);
    std::string dni_cliente_esc = escapeSQL(dni_cliente);

    if (!direccion_esc.empty()) {
        // Modificar Cliente (supongamos que el Cliente quiere actualizar el DNI que aparece en el contrato asociado a su Hogar)
        char cliente[2048];
        std::string nuevo_dni_cliente_esc = escapeSQL(nuevo_dni_cliente);
        sprintf(cliente, "UPDATE Cliente SET DNI_CIF = '%s' WHERE DNI_CIF = '%s';", nuevo_dni_cliente_esc.c_str(), dni_cliente_esc.c_str());
        
        SQLRETURN retCliente = SQLExecDirectA(handler, (SQLCHAR*)cliente, SQL_NTS);
        if (retCliente != SQL_SUCCESS && retCliente != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al modificar el DNI del cliente asociado al Hogar.\n";
            mostrarError(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return false;
        }

        SQLCloseCursor(handler);

        // Modificar Contrato 
        std::string updateContrato = "UPDATE Contrato SET ";
        bool primero = true;

        if (!datos_contrato.cups.empty()) {
            updateContrato += "CUPS = '" + escapeSQL(datos_contrato.cups) + "'";
            primero = false;
        }

        if (!datos_contrato.tipo_contrato.empty()) {
            if (!primero) updateContrato += ", ";
            updateContrato += "Tipo_Contrato = '" + escapeSQL(datos_contrato.tipo_contrato) + "'";
            primero = false;
        }

        if (!datos_contrato.tarifa.empty()) {
            if (!primero) updateContrato += ", ";
            updateContrato += "Tarifa = '" + escapeSQL(datos_contrato.tarifa) + "'";
            primero = false;
        }

        if (datos_contrato.potencia_contratada >= 0) { 
            if (!primero) updateContrato += ", ";
            char buf[32];
            sprintf(buf, "%.2f", datos_contrato.potencia_contratada);
            updateContrato += "Potencia_Con = " + std::string(buf);
        }

        updateContrato += " WHERE ID_Contrato = (SELECT ID_Contrato_H FROM Hogar WHERE Direccion = '" + direccion_esc + "')";

        SQLRETURN retContrato = SQLExecDirectA(handler, (SQLCHAR*)updateContrato.c_str(), SQL_NTS);
        if (retContrato != SQL_SUCCESS && retContrato != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al modificar el contrato del hogar.\n";
            mostrarError(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return false;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, handler);
    return true;
}

bool GestionTransmisionDistribucion::registrarIncidencia(int id_incidencia, int id_contrato,
                                                         const std::string& descripcion, const std::string& tipo_incidencia,
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
    sprintf(incidencia, "INSERT INTO Incidencia (ID_Incidencia, ID_Contrato_I, Descripcion_Incidencia, Tipo_Incidencia, Fecha_Incidencia, Fecha_Resolucion, Estado_Incidencia) "
                        "VALUES (%d, %d, '%s', '%s', TO_DATE('%s','YYYY-MM-DD'), TO_DATE('%s','YYYY-MM-DD'), '%s');",
                        id_incidencia, id_contrato, descripcion_esc.c_str(), tipo_incidencia.c_str(),
                        fecha_incidencia.c_str(), fecha_resolucion.c_str(), estado_incidencia.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)incidencia, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error al ejecutar la sentencia SQL para registrar la incidencia.\n";
        mostrarError(handler);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    return true;
}
