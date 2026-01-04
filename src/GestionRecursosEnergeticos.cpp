#include "GestionRecursosEnergeticos.h"
#include "ConexionADB.h"
#include <iostream>

// TEMP
void printSQLError(SQLSMALLINT handleType, SQLHANDLE handle)
{
    SQLCHAR sqlState[6];
    SQLCHAR message[512];
    SQLINTEGER native;
    SQLSMALLINT len;

    SQLGetDiagRec(
            handleType,
            handle,
            1,
            sqlState,
            &native,
            message,
            sizeof(message),
            &len
        );

        std::cerr << "ODBC ERROR [" << sqlState << "]: "
                  << message << std::endl;
    
}


GestionRecursosEnergeticos::GestionRecursosEnergeticos(ConexionADB& con) : conexion(con) {}

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
            sql_sentence += potencia_actual + "');";

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
            
            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
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
                    // Si el resultado de SUM no es NULL (indicador != SQL_NULL_DATA)
                    if (indicador != SQL_NULL_DATA) {
                        ingresos = static_cast<double>(ingresos_sql);
                        retorno = true;
                    } else {
                        std::cout << "WARNING: No se han encontrado columnas con estos datos.\n";
                        ingresos = 0.0;
                        retorno = false;
                    }
                } else {
                    std::cerr << "ERROR: No se pudo consultar el ingreso histórico.\n";
                }
            } else {
                std::cerr << "ERROR: No se pudo ejecutar la consulta.\n";
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
                "WHERE (LOWER(Nombre_Fuente_Energetica) = LOWER('" + tipoEnergia + "'));";

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

bool GestionRecursosEnergeticos::consultarIngresosPorTipoEnergia(
    const std::string& tipoEnergia,
    double& ingresos) const 
{
    // Valor de retorno por defecto: fallo. Como ya he explicado, cuanto menos overhead tengamos, mejor.
    bool retorno = false;

    // Comprobamos que la conexión esté establecida.
    if (conexion.isConnected()) {

        // Variable donde almacenaremos el resultado SQL. Luego lo pasaremos a ingresos (pasado por referencia).
        SQLDOUBLE ingresos_sql = 0.0;
        SQLLEN indicador = 0;

        // Conexión
        SQLHDBC conex = conexion.getConnection();
        // Handler
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

            // Sentencia SQL: suma ingresos para ese tipo de energía. ¡Ojo, puede devolver null data!
            std::string sql_sentence =
                "SELECT SUM(Ingresos_Netos_Historicos) "
                "FROM Instalacion_Energetica "
                "WHERE (Nombre_Fuente_Energetica = '" + tipoEnergia + "');";

            ret = SQLExecDirect(handler,
                                (SQLCHAR*)sql_sentence.c_str(),
                                SQL_NTS);

            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

                // Asociamos la única columna resultante (solo seleccionamos un dato).
                SQLBindCol(handler,
                           1,
                           SQL_C_DOUBLE,
                           &ingresos_sql,
                           0,
                           &indicador);

                // Recuperamos el valor
                ret = SQLFetch(handler);
                // Si todo bien:
                if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
                    // Si ha habido suma, es decir, han habido datos:
                    if (indicador != SQL_NULL_DATA) {
                        ingresos = static_cast<double>(ingresos_sql);
                    } else {
                        // Si no hay datos, devolvemos 0 por coherencia semántica
                        ingresos = 0.0;
                    }

                    retorno = true;
                }
                else {
                    std::cerr << "ERROR: No se pudo recuperar la fila de ingresos por tipo de energía.\n";
                }

            } else {
                std::cerr << "ERROR: No se pudo ejecutar la consulta de ingresos por tipo de energía.\n";
            }

            // Liberamos handler
            SQLFreeHandle(SQL_HANDLE_STMT, handler);

        } else {
            std::cerr << "ERROR: No se pudo crear el handler de sentencia.\n";
        }

    } else {
        std::cerr << "ERROR: Conexión no establecida correctamente. Compruebe sus credenciales.\n";
    }

    return retorno;
}

// Vale, este requisito funcional es con diferencia el más largo y complejo de mi subsistema; así que voy a hacerlo
// MUY poquito a poco y con mucho amor y cuidado, que va a ser un dolor de cabeza depurarlo :p
// Ah, además voy a abandonar temporalmente la (buena) metodología de tener un sólo flujo porque es más sencillo
// meter algunos "return false;" que tener POR LO MENOS 4 tabulaciones de indent por comprobar que el retorno sea
// true en cada paso. Más legibilidad, un poco menos de depurabilidad, meh :p
bool GestionRecursosEnergeticos::cederPotencia(
    const std::string& dirCedente,
    const std::string& tipoCedente,
    const std::string& dirReceptora,
    const std::string& tipoReceptora,
    int porcentaje)
{
    // ANTES DE NADA, debemos gestionar que una instalación NO se ceda potencia a sí misma. (Me acuerdo de la defensa
    // de la P2; ahora ya sí puedo ponerlo explícitamente, je)
    if(dirCedente == dirReceptora && tipoCedente == tipoReceptora){
        std::cerr << "ERROR: Una instalación no se puede ceder potencia a sí misma.\n";
        return false;
    }

    // Valor por defecto: fallo. Asumimos fallo por, de nuevo, el overhead (me repito demasiado)
    bool retorno = false;

    // Comprobamos conexión.
    if (!conexion.isConnected()) {
        std::cerr << "ERROR: Conexión no establecida correctamente.\n";
        return false;
    }

    // Declaramos conexión y handler.
    SQLHDBC conex = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;
    // Comprobamos que se ha declarado correctamente el handler.
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "ERROR: No se pudo crear el handler.\n";
        return false;
    }

    // Potencias de cada una de las instalaciones.
    double potCed = 0.0;
    double potRec = 0.0;
    SQLLEN ind = 0;

    /* ============================================================
          1. OBTENEMOS POTENCIA DE LA INSTALACIÓN DE LA CEDENTE
       ============================================================ */
    // "q1" de query 1; primera consulta para ver potencias.
    std::string q1 =
        "SELECT Potencia_Actual "
        "FROM Instalacion_Energetica "
        "WHERE (Direccion_Instalaciones = '" + dirCedente + "' "
        "AND Nombre_Fuente_Energetica = '" + tipoCedente + "'):";

    // Ejecutamos y extraemos los datos de la columna vista.
    ret = SQLExecDirect(handler, (SQLCHAR*)q1.c_str(), SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLBindCol(handler, 1, SQL_C_DOUBLE, &potCed, 0, &ind);
        if (SQLFetch(handler) != SQL_SUCCESS) {
            std::cerr << "ERROR: La instalación cedente no existe.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return false;
        }
    }
    // Liberamos el cursor para la siguiente consulta en el mismo handler.
    SQLFreeStmt(handler, SQL_CLOSE);

    /* ============================================================
            2. OBTENEMOS POTENCIA DE LA INSTALACIÓN RECEPTORA
       ============================================================ */
    // De nuevo, "q2" de query 2; segunda consulta.
    std::string q2 =
        "SELECT Potencia_Actual "
        "FROM Instalacion_Energetica "
        "WHERE (Direccion_Instalaciones = '" + dirReceptora + "' "
        "AND Nombre_Fuente_Energetica = '" + tipoReceptora + "');";

    // Ejecutamos sentencia y guardamos datos.
    ret = SQLExecDirect(handler, (SQLCHAR*)q2.c_str(), SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLBindCol(handler, 1, SQL_C_DOUBLE, &potRec, 0, &ind);
        if (SQLFetch(handler) != SQL_SUCCESS) {
            std::cerr << "ERROR: La instalación receptora no existe.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return false;
        }
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    /* ============================================================
                3. COMPROBAMOS RESTRICCIONES SEMÁNTICAS
       ============================================================ */
    if (potCed <= 65.0) {
        std::cerr << "ERROR: La instalación cedente no supera el 65%.\n(VIOLACIÓN RS-2.5.1)";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    if (potRec >= 35.0) {
        std::cerr << "ERROR: La instalación receptora ya supera el 35%.\n(VIOLACIÓN RS-2.5.2)\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    // Validación extra lógica
    if (porcentaje <= 0 || porcentaje > 100) {
        std::cerr << "ERROR: Porcentaje de cesión no válido.(Nota: Rango de cesión entre 0% y 100%.)\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }
    if(potCed - porcentaje < 0.0 || potRec + porcentaje > 100.0){
        std::cerr << "ERROR: Porcentaje de cesión no válido.\n(Nota: El cedente se queda en números rojos, o el receptor por encima del 100%).\n";
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    /* ============================================================
                    4. ACTUALIZAMOS POTENCIA CEDENTE
       ============================================================ */
    std::string upd1 =
        "UPDATE Instalacion_Energetica "
        "SET Potencia_Actual = Potencia_Actual - " + std::to_string(porcentaje) +
        " WHERE (Direccion_Instalaciones = '" + dirCedente + "' "
        "AND Nombre_Fuente_Energetica = '" + tipoCedente + "');";

    ret = SQLExecDirect(handler, (SQLCHAR*)upd1.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    /* ============================================================
       5. ACTUALIZAMOS POTENCIA RECEPTORA
       ============================================================ */
    std::string upd2 =
        "UPDATE Instalacion_Energetica "
        "SET Potencia_Actual = Potencia_Actual + " + std::to_string(porcentaje) +
        " WHERE (Direccion_Instalaciones = '" + dirReceptora + "' "
        "AND Nombre_Fuente_Energetica = '" + tipoReceptora + "');";

    ret = SQLExecDirect(handler, (SQLCHAR*)upd2.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return false;
    }

    /* ============================================================
       6. COMMIT FINAL
       ============================================================ */
    SQLEndTran(SQL_HANDLE_DBC, conex, SQL_COMMIT);
    retorno = true;
    SQLFreeHandle(SQL_HANDLE_STMT, handler);
    return retorno;
}


// RF-2.6, modificado levemente para que implementar un trigger sea algo más natural.
bool GestionRecursosEnergeticos::anadirIngreso(
    const std::string& direccion,
    const std::string& tipoEnergia,
    const double& cantidadAAñadir)
{
    // Valor por defecto: fallo (menos overhead)
    bool retorno = false;
    
    // Aquí iba una comprobación de que la cantidad fuese > 0, pero como los ingresos son NETOS pueden ser negativos.

    // Comprobamos conexión
    if (!conexion.isConnected()) {
        std::cerr << "ERROR: Conexión no establecida correctamente.\n";
        return false;
    }

    // Declaramos conexión y handler.
    SQLHDBC conex = conexion.getConnection();
    SQLHSTMT handler = SQL_NULL_HSTMT;
    // Comprobamos que se hayan gestionado bien los recursos del handler.
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conex, &handler);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "ERROR: No se pudo crear el handler de sentencia.\n";
        return false;
    }

    // UPDATE (activará el trigger, en el main)
    std::string sql_sentence =
        "UPDATE Instalacion_Energetica "
        "SET Ingresos_Netos_Historicos = Ingresos_Netos_Historicos + "
        + std::to_string(cantidadAAñadir) +
        " WHERE (Direccion_Instalaciones = '" + direccion + "' "
        "AND Nombre_Fuente_Energetica = '" + tipoEnergia + "');";

    ret = SQLExecDirect(handler,
                        (SQLCHAR*)sql_sentence.c_str(),
                        SQL_NTS);

    // Si ejecución OK
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        // Contamos las filas
        SQLLEN filas = 0;
        SQLRowCount(handler, &filas);
        // Si sólo hay una fila (todo OK, bien identificado)
        if (filas == 1) {
            SQLEndTran(SQL_HANDLE_DBC, conex, SQL_COMMIT);
            retorno = true;
        // Si hay 0 o más de una (JAMÁS habrá más de una si está bien definido, pero es un caso contemplado en el else):
        } else {
            std::cout << "WARNING: No se ha actualizado ninguna instalación.\n";
            SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
        }
    // Si ejecución no-OK
    } else {
        std::cerr << "ERROR: No se pudo actualizar los ingresos.\n";
        SQLEndTran(SQL_HANDLE_DBC, conex, SQL_ROLLBACK);
    }
    // Liberamos recursos
    SQLFreeHandle(SQL_HANDLE_STMT, handler);
    // Fin
    return retorno;
}
