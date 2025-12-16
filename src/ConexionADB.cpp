#include "ConexionADB.h"

// Empezamos con el constructor; lo inicializamos todo a NULL para controlar estados de forma sencilla.

ConexionADB::ConexionADB(){
    env = SQL_NULL_HENV;
    hdb = SQL_NULL_HDBC;
    conexion_establecida = false;
}

// Conectamos con la DB:

bool ConexionADB::connect(const std::string& dsn,
                          const std::string& usr,
                          const std::string& pwd)
{
    // Variable de control: si pasa a false, algo ha fallado y no seguimos.
    bool bien = true;

    // Variable para comprobar el estado de las llamadas ODBC.
    SQLRETURN ret;

    // Creamos el entorno ODBC (ENV).
    if (bien) {
        ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            bien = false;
        }
    }

    // Indicamos que vamos a trabajar con ODBC versión 3.
    if (bien) {
        ret = SQLSetEnvAttr(env,
                            SQL_ATTR_ODBC_VERSION,
                            (SQLPOINTER)SQL_OV_ODBC3,
                            0);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            bien = false;
        }
    }

    // Creamos el handle de conexión (DBC), dependiente del entorno.
    if (bien) {
        ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            bien = false;
        }
    }

    // Intentamos establecer la conexión usando el DSN y las credenciales.
    if (bien) {
        ret = SQLConnect(dbc,
                         (SQLCHAR*)dsn.c_str(), SQL_NTS,
                         (SQLCHAR*)usr.c_str(), SQL_NTS,
                         (SQLCHAR*)pwd.c_str(), SQL_NTS);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            bien = false;
        }
    }

    // Desactivamos el autocommit.
    if (bien) {
        ret = SQLSetConnectAttr(dbc,
                                SQL_ATTR_AUTOCOMMIT,
                                (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                                0);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            bien = false;
        }
    }

    // Actualizamos el estado interno de la conexión.
    conexion_establecida = bien;

    // Si algo ha fallado, liberamos los recursos reservados.
    if (!bien) {
        if (dbc != SQL_NULL_HDBC) {
            SQLFreeHandle(SQL_HANDLE_DBC, dbc);
            dbc = SQL_NULL_HDBC;
        }
        if (env != SQL_NULL_HENV) {
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            env = SQL_NULL_HENV;
        }
    }

    // Devolvemos si la conexión se ha establecido correctamente o no.
    return bien;
}

// Para obtener la conexión y poder hacer cositas:
SQLHDBC ConexionADB::getConnection(){
    // Si hay conexión la devolvemos; si no, devolvemos NULL (por seguridad, pero se supone que siempre está o bien o en NULL).
    if(isConnected())
        return dbc;
    else
        return SQL_NULL_HDBC;
}

// Para ver si se está o no conectado:
bool ConexionADB::isConnected(){
    return conexion_establecida;
}

// Ahora vamos a gestionar la desconexión:
void ConexionADB::disconnect()
{
    // Si no hay conexión establecida, no hay nada que hacer.
    if (isConnected()) {
        // Antes de cerrar, hacemos rollback de cualquier transacción pendiente
        // para evitar dejar cambios a medias en la base de datos (CONGRUENCIA).
        SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_ROLLBACK);

        // Cerramos la conexión con la base de datos.
        SQLDisconnect(dbc);

        // Liberamos el handle de conexión.
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        dbc = SQL_NULL_HDBC;

        // Liberamos el entorno ODBC.
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        env = SQL_NULL_HENV;

        // Actualizamos el estado interno.
        conexion_establecida = false;
    }
}

// Destructor; SIEMPRE DEBE SER REDUNDANTE! Lo pido por favor,
// DESCONECTAR SIEMPRE! Nunca depender del destructor por si acaso.
ConexionADB::~ConexionADB()
{
    // Si existe una conexión activa, la cerramos de forma segura.
    // disconnect() se encarga de comprobar el estado y liberar recursos.
    disconnect();
}

