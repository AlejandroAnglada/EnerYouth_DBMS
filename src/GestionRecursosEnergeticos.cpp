#include "GestionRecursosEnergeticos.h"
#include "ConexionADB.h"
#include <iostream>

bool GestionRecursosEnergeticos::altaFuenteEnergetica(
    const std::string& nombre, 
    const std::string& descripcion, 
    const std::string& direccion,
    const std::string& fechaYYYYMMDD,
    const std::string& ingresos_netos,
    const std::string& potencia_actual)
{ 
    // Valor de retorno por defecto: fallo. Se actualizará si todo va bien; y asumimos que no (hay más casos de
    // fallo (aunque menos probables) que de éxito; menos overhead (aunque sea muy pequeño, sigue existiendo y
    // hay que optimizar al máximo :P))
    bool retorno = false;

    // Comprobamos que la conexión esté establecida.
    if (conexion.isConnected()) {
        // Conexión
        SQLHDBC conex = conexion.getConnection();
        // Handler (nulo hasta inicializarlo)
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler:
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

            // Definimos la sentencia SQL:
            std::string sql_sentence = "INSERT INTO Instalacion_Energetica ";
            sql_sentence += "(Nombre_Fuente_Energetica, Direccion_Instalaciones, Descripcion, ";
            sql_sentence += "Ingresos_Netos_Historicos, Fecha_Fundacion, Potencia_Actual) ";
            sql_sentence += "VALUES ('" + nombre + "','" + direccion + "','" + descripcion + "','";
            sql_sentence += ingresos_netos + "', TO_DATE('" + fechaYYYYMMDD + "', 'YYYY-MM-DD'),'";
            sql_sentence += potencia_actual + "')";

            // Ejecutamos la sentencia.
            ret = SQLExecDirect(handler,
                                (SQLCHAR*)sql_sentence.c_str(),
                                SQL_NTS);

            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                // Todo ha ido bien: hacemos COMMIT, y devolvemos true.
                SQLEndTran(SQL_HANDLE_DBC, conex, SQL_COMMIT);
                retorno = true;
            } else {
                // Error en el INSERT: hacemos ROLLBACK   .
                std::cerr << "ERROR: No se pudo insertar la fuente energética. Compruebe datos.\nFinalizando...\n";
                SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
            }

            // Liberamos el handler de sentencia.
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
        } else {
            std::cerr << "ERROR: No se pudo crear el handler de sentencia.\n";
        }

    } else {
        std::cerr << "ERROR: Conexión no establecida correctamente. Compruebe sus credenciales.\nFinalizando...\n";
    }

    return retorno;
}

// La duplicidad de comentarios viene de copiar y pegar código para *seguir* cumpliendo el esquema ODBC de conexión.
bool GestionRecursosEnergeticos::bajaFuenteEnergetica(const std::string& nombre, 
                                                      const std::string& direccion){
    // Valor de retorno por defecto: fallo. Se actualizará si todo va bien; y asumimos que no (hay más casos de
    // fallo (aunque menos probables) que de éxito; menos overhead (aunque sea muy pequeño, sigue existiendo y
    // hay que optimizar al máximo :P))
    bool retorno = false;
    // Para ver que efectivamente se hayan borrado filas. Si no, no hay error, pero quizá no se han pasado correcta-
    // mente los parámetros.
    SQLLEN filas_afectadas = 0;

    // Comprobamos que la conexión esté establecida.
    if (conexion.isConnected()) {
        // Conexión
        SQLHDBC conex = conexion.getConnection();
        // Handler (nulo hasta inicializarlo)
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler:
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

            // Definimos la sentencia SQL:
            std::string sql_sentence = "DELETE FROM Instalacion_Energetica WHERE (";
            sql_sentence += "Nombre_Fuente_Energetica = '" + nombre + "' AND ";
            sql_sentence += "Direccion_Instalaciones = '" + direccion + "');";
            // Ejecutamos la sentencia.
            ret = SQLExecDirect(handler,
                                (SQLCHAR*)sql_sentence.c_str(),
                                SQL_NTS);
            // Si no ha habido fallo:
            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                // Contamos cuántas filas han sido afectadas
                SQLRowCount(handler, &filas_afectadas);
                // Si se ha borrado una fila (todo bien):
                if (filas_afectadas == 1) {
                    SQLEndTran(SQL_HANDLE_DBC, conex, SQL_COMMIT);
                    retorno = true;
                } 
                // Si no se ha borrado fila (no tira error, pero semánticamente está mal por lo que
                // devuelve false):
                else {
                    std::cout << "WARNING: No se ha borrado ninguna fila.\n";
                    SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
                }

            } else {
                std::cerr << "ERROR: No se pudo borrar la fuente energética.\n";
                SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
        }


            // Liberamos el handler de sentencia.
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
        } else {
            std::cerr << "ERROR: No se pudo crear el handler de sentencia.\n";
        }

    } else {
        std::cerr << "ERROR: Conexión no establecida correctamente. Compruebe sus credenciales.\nFinalizando...\n";
    }

    return retorno;
}
