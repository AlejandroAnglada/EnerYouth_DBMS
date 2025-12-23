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

bool GestionRecursosEnergeticos::consultarIngresosPorInstalacion(const std::string& direccion, 
                                                                 double& ingresos) const {
    // Valor de retorno por defecto: fallo. Se actualizará si todo va bien; y asumimos que no (hay más casos de
    // fallo (aunque menos probables) que de éxito; menos overhead (aunque sea muy pequeño, sigue existiendo y
    // hay que optimizar al máximo :P))
    bool retorno = false;

    // Comprobamos que la conexión esté establecida.
    if (conexion.isConnected()) {
        // Contador para actualizar los ingresos
        SQLDOUBLE ingresos_sql = 0.0;
        // Francamente, no sé qué es esto, pero se necesita para el BindCol
        SQLLEN indicador = 0;
        // Conexión
        SQLHDBC conex = conexion.getConnection();
        // Handler (nulo hasta inicializarlo)
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler:
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

            // Definimos la sentencia SQL:
            std::string sql_sentence = "SELECT SUM(Ingresos_Netos_Historicos) FROM Instalacion_Energetica ";
            sql_sentence += "WHERE (Direccion_Instalaciones = '" + direccion + "');";
            ret = SQLExecDirect(handler,
                                (SQLCHAR*)sql_sentence.c_str(),
                                SQL_NTS);
            SQLBindCol(handler,
                       1,                  // Columna 1 (empieza en 1 porque sólo se recupera una columna)
                       SQL_C_DOUBLE,       // Tipo de dato en C
                       &ingresos_sql,      // Dónde guardar los ingresos
                       0,                  // No hay realmente buffer, luego buffer len = 0
                       &indicador);
            // Recuperamos el valor
            ret = SQLFetch(handler);

            // Si no ha habido fallo:
            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                // Si todo bien, guardamos en ingresos el valor del BindCol.
                ingresos = static_cast<double>(ingresos_sql);
                // Todo OK
                retorno = true;
            } 
            // Es posible que no se encuentren datos al hacer la consulta (No se usa NO_DATA porque usamos
            // SUM, que jamás devuelve NO_DATA):
            else if(ret == SQL_NULL_DATA){
                std::cout << "WARNING: No se han encontrado columnas con estos datos.\n";
                // De nuevo, error semántico y no sintáctico.
                retorno = false;
            } else {
                std::cerr << "ERROR: No se pudo consultar el ingreso histórico.\n";
                // No se necesita rollback; es un SELECT simple.
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


bool GestionRecursosEnergeticos::consultarInstalacionesPorEnergia(
    const std::string& tipoEnergia,
    std::vector<std::string>& instalaciones) const
{
    // Valor de retorno por defecto: fallo, asumimos esto por el tema del overhead ya visto.
    bool retorno = false;

    // Limpiamos el vector para cada consulta.
    instalaciones.clear();

    // Comprobamos que la conexión esté establecida.
    if (conexion.isConnected()) {

        // Conexión.
        SQLHDBC conex = conexion.getConnection();
        // Handler.
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler.
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

            // Sentencia SQL: puede devolver VARIAS filas.
            std::string sql_sentence =
                "SELECT Direccion_Instalaciones "
                "FROM Instalacion_Energetica "
                "WHERE (Nombre_Fuente_Energetica = '" + tipoEnergia + "')";

            ret = SQLExecDirect(handler,
                                (SQLCHAR*)sql_sentence.c_str(),
                                SQL_NTS);

            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

                // Buffer para la dirección (VARCHAR).
                char direccion_sql[256];
                // Indicador ODBC (NULL / longitud).
                SQLLEN indicador = 0;

                // Asociamos la columna.
                SQLBindCol(handler,
                           1,                 // Primera columna (sólo seleccionamos un ítem)
                           SQL_C_CHAR,        // Tipo en C (char*)
                           direccion_sql,     // Ahora SÍ que hay Buffer.
                           sizeof(direccion_sql),
                           &indicador);

                // Recorremos todas las filas.
                while ((ret = SQLFetch(handler)) == SQL_SUCCESS ||
                       ret == SQL_SUCCESS_WITH_INFO) {

                    if (indicador != SQL_NULL_DATA) {
                        instalaciones.emplace_back(direccion_sql);
                    }
                }

                // Si hemos recuperado al menos una instalación, ¡bien! Devolvemos true.
                if (!instalaciones.empty()) {
                    retorno = true;
                }

            } else {
                std::cerr << "ERROR: No se pudo ejecutar la consulta de instalaciones.\n";
            }

            // Liberamos el handler
            SQLFreeHandle(SQL_HANDLE_STMT, handler);

        } else {
            std::cerr << "ERROR: No se pudo crear el handler de sentencia.\n";
        }

    } else {
        std::cerr << "ERROR: Conexión no establecida correctamente. Compruebe sus credenciales.\n";
    }

    return retorno;
}
