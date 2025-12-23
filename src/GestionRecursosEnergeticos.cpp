#include "GestionRecursosEnergeticos.h"
#include "ConexionADB.h"
#include <iostream>

bool GestionRecursosEnergeticos::altaFuenteEnergetica(
    const std::string& nombre, 
    const std::string& descripcion, 
    const std::string& direccion,
    const std::string& fechaYYYYMMDD)
{
    // Como internamente la clase tiene una referencia a instancia de conexión, no debemos conectarnos.
    // Comprobamos que nos hemos conectado correctamente:
    if(conexion.isConnected()){
        // Conexión
        SQLHDBC conex = this->conexion.getConnection();
        // "Handler"
        SQLHSTMT handler = SQL_NULL_HSTMT;
        // Inicializamos el handler intermedio:
        SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);
        // Vamos a definir como string (convertible a char* con c_str()) la sentencia:
        std::string sql_sentence = "INSERT INTO Instalacion_Energetica ";
        sql_sentence += "(Nombre_Fuente_Energetica, Direccion_Instalaciones, Descripcion, Fecha_Fundacion) ";
        sql_sentence += "VALUES ('" + nombre + "','" + direccion + "','" + descripcion + "','" + fechaYYYYMMDD + "');";
        SQLRETURN ret = SQLExecDirect(handler, (SQLCHAR*)sql_sentence.c_str(), SQL_NTS);
        // Si no se ha realizado por lo que sea:
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cerr << "¡ERROR! Algo ha salido mal en la sentencia. ¿Ha puesto bien los datos?\n";
        }
        // Liberamos el handler:
        SQLFreeHandle(SQL_HANDLE_STMT, handler);

    } else {
        std::cerr << "ERROR: ¡Conexión no establecida correctamente! Compruebe sus credenciales.\nFinalizando...\n";
        exit(-1);
    }
}