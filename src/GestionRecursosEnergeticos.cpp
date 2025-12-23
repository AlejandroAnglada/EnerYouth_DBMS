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
