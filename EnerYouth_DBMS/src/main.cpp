#include<windows.h>
#include<sqlext.h>
#include<sqltypes.h>
#include<sql.h>
#include<iostream>

void mostrarContenidoTablas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    // Se usa SQLFetch y SQLGetData para leer fila por fila los resultados
    // Funcionamiento: se ejecuta el fetch de la siguiente fila del resultado de la consulta que se ejecutó con SQLExecDirectA y 
    // devuelve SQL_SUCCESS si hay fila disponible, SQL_NO_DATA si ya no hay más filas.Con el while se recorren todas las filas 
    // hasta el final.
    // SQLGetData(handler, columna, tipo, buffer, tamaño, indicador) recupera el valor de una columna específica de la fila actual.
    // Parámetros:
    // - handler
    // - columna: número de columna. En el caso de Stock, 1 para Cproducto, 2 para Cantidad
    // - tipo: tipo de datos en el buffer (SQL_C_LONG para enteros, SQL_C_CHAR para strings, etc.)
    // - buffer: dirección de memoria donde se guarda el valor de esa columna
    // - tamaño: tamaño del buffer (importante para strings)
    // - indicador: puntero a una variable que indica si el valor es NULL (puede ser NULL si no se quiere comprobar)
    
    // Tabla Stock
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Stock;", SQL_NTS);
    std::cout << "--Tabla Stock:--\n";
    SQLINTEGER cproducto, cantidad;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &cproducto, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &cantidad, 0, NULL);
        std::cout << cproducto << "\t" << cantidad << "\n";
    }

    // Tabla Pedido
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Pedido;", SQL_NTS);
    std::cout << "\n--Tabla Pedido:--\n";
    SQLINTEGER cpedido, ccliente;
    char fecha[11];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &cpedido, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ccliente, 0, NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, &fecha, sizeof(fecha), NULL);
        std::cout << cpedido << "\t" << ccliente << "\t" << fecha << "\n";
    }

    // Tabla Detalle_Pedido
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Detalle_Pedido;", SQL_NTS);
    std::cout << "\n--Tabla Detalle_Pedido:--\n";
    SQLINTEGER dp_cpedido, dp_cproducto, dp_cantidad;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &dp_cpedido, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &dp_cproducto, 0, NULL);
        SQLGetData(handler, 3, SQL_C_LONG, &dp_cantidad, 0, NULL);
        std::cout << dp_cpedido << "\t" << dp_cproducto << "\t" << dp_cantidad << "\n";
    }
}

void borrarTablas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Detalle_Pedido;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Pedido;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Stock;", SQL_NTS);

    std::cout << "\nTablas borradas correctamente.\n";

    SQLTransact(entorno, conexion, SQL_COMMIT);
}

void crearTablas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Stock ("
        "Cproducto INT PRIMARY KEY NOT NULL,"
        "Cantidad INT"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Pedido ("
        "Cpedido INT PRIMARY KEY NOT NULL,"
        "Ccliente INT,"
        "Fecha_pedido DATE"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Detalle_Pedido ("
        "Cpedido INT,"
        "Cproducto INT,"
        "Cantidad INT,"
        "PRIMARY KEY (Cpedido, Cproducto),"
        "FOREIGN KEY (Cpedido) REFERENCES Pedido(Cpedido),"
        "FOREIGN KEY (Cproducto) REFERENCES Stock(Cproducto)"
        ");", SQL_NTS);

    std::cout << "\nTablas creadas correctamente.\n";

    SQLTransact(entorno, conexion, SQL_COMMIT);
}

void insertarTuplas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (1, 100);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (2, 150);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (3, 200);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (4, 250);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (5, 300);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (6, 350);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (7, 400);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (8, 450);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (9, 500);", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "INSERT INTO Stock (Cproducto, Cantidad) VALUES (10, 550);", SQL_NTS);

    std::cout << "\nDatos insertados correctamente.\n";

    SQLTransact(entorno, conexion, SQL_COMMIT);
}

void darDeAltaPedido(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {    
    int ccliente, cpedido, cproducto, opcion;
    std::string fecha;
    bool salida = false;

    SQLSetConnectAttr(conexion, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
    
    SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT", SQL_NTS);

    std::cout << "Ingrese el codigo del pedido: ";
    std::cin >> cpedido;
    std::cout << "Ingrese el codigo del cliente: ";
    std::cin >> ccliente;
    std::cout << "Ingrese la fecha: ";
    std::cin >> fecha;
    std::cout << "Ingrese el producto: ";
    std::cin >> cproducto;
    
    char cadena[256];
    sprintf(cadena, "INSERT INTO Pedido (Cpedido, Ccliente, Fecha_pedido) VALUES (%d, %d, TO_DATE('%s', 'YYYY-MM-DD'));", cpedido, ccliente, fecha.c_str());
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)cadena, SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlState[6], msg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;
        SQLGetDiagRecA(SQL_HANDLE_STMT, handler, 1, sqlState, &nativeError, msg, sizeof(msg), &msgLen);
        std::cout << "Error SQL: " << msg << " (SQLSTATE: " << sqlState << ")\n";
    }

    mostrarContenidoTablas(handler, entorno, conexion);

    do {
        std::cout << "\n===== MENU ALTA PEDIDO =====\n";
        std::cout << "1. Añadir detalle de producto\n";
        std::cout << "2. Eliminar todos los detalles de producto\n";
        std::cout << "3. Cancelar pedido\n";
        std::cout << "4. Finalizar pedido\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: {
                // Creamos handlers separados para SELECT, INSERT y UPDATE
                SQLHSTMT handler_select, handler_insert, handler_update;
                SQLAllocHandle(SQL_HANDLE_STMT, conexion, &handler_select);
                SQLAllocHandle(SQL_HANDLE_STMT, conexion, &handler_insert);
                SQLAllocHandle(SQL_HANDLE_STMT, conexion, &handler_update);

                SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT darDeAltaPedido_creado", SQL_NTS);

                // Comprobamos si hay stock disponible
                char cadena_select[256];
                sprintf(cadena_select, "SELECT Cantidad FROM Stock WHERE Cproducto = %d", cproducto);
                SQLRETURN retProducto = SQLExecDirectA(handler_select, (SQLCHAR*)cadena_select, SQL_NTS);

                if (retProducto != SQL_SUCCESS && retProducto != SQL_SUCCESS_WITH_INFO) {
                    std::cout << "Error al consultar el stock.\n";
                    SQLTransact(entorno, conexion, SQL_ROLLBACK);
                    break;
                }

                int cantidad_disponible = 0;

                if (SQLFetch(handler_select) == SQL_SUCCESS) {
                    SQLGetData(handler_select, 1, SQL_C_LONG, &cantidad_disponible, 0, NULL);

                    SQLCloseCursor(handler_select); 

                    if (cantidad_disponible > 0) {
                        int cantidad_solicitada;
                        std::cout << "Ingrese la cantidad solicitada: ";
                        std::cin >> cantidad_solicitada;

                        if (cantidad_solicitada <= cantidad_disponible) {
                            char cadena2[256];

                            sprintf(cadena2, "INSERT INTO Detalle_Pedido (Cpedido, Cproducto, Cantidad) VALUES (%d, %d, %d);", cpedido, cproducto, cantidad_solicitada);
                            SQLExecDirectA(handler_insert, (SQLCHAR*)cadena2, SQL_NTS);

                            // Actualizamos Stock restando la cantidad solicitada
                            char cadenaCant[256];
                            sprintf(cadenaCant, "UPDATE Stock SET Cantidad = Cantidad - %d WHERE Cproducto = %d", cantidad_solicitada, cproducto);
                            SQLExecDirectA(handler_update, (SQLCHAR*)cadenaCant, SQL_NTS);

                            std::cout << "Detalle de producto añadido correctamente.\n";

                        } else {
                            std::cout << "No hay suficiente stock disponible.\n";
                        }
                    } else {
                        std::cout << "No hay stock disponible para este producto.\n";
                    }
                } else {
                    std::cout << "No se ha podido obtener la cantidad disponible\n";
                }

                SQLFreeHandle(SQL_HANDLE_STMT, handler_select);
                SQLFreeHandle(SQL_HANDLE_STMT, handler_insert);
                SQLFreeHandle(SQL_HANDLE_STMT, handler_update);

                std::cout << "Contenido actual de las tablas:\n";
                mostrarContenidoTablas(handler, entorno, conexion);
            }
            break;
            case 2:
                SQLExecDirectA(handler, (SQLCHAR*) "ROLLBACK TO SAVEPOINT darDeAltaPedido_creado;", SQL_NTS);

                std::cout << "Contenido actual de las tablas:\n";
                mostrarContenidoTablas(handler, entorno, conexion);
            break;
            case 3:
                SQLTransact(entorno, conexion, SQL_ROLLBACK);
                std::cout << "Contenido actual de las tablas:\n";
                mostrarContenidoTablas(handler, entorno, conexion);
                salida = true;
            break;
            case 4: 
                std::cout << "Guardando cambios...\n";
                SQLTransact(entorno, conexion, SQL_COMMIT);
                salida = true;
            break; 
            default: std::cout << "Opcion no valida.\n"; break;
        }
    } while (!salida);
}

int main(int argc, char ** argv){
    // PARA COMPILAR: "C:\mingw64\bin\x86_64-w64-mingw32-g++.exe" main.cpp -o app.exe -lodbc32 (luego se quita)

    std::string user, pwd;
    int opcion;

    // Pedimos las credenciales al usuario para conectarse a la db:
    std::string dsn = "practbd"; // Nombre del DSN, puesto por defecto tal y como en la explicación del S1
    std::cout << "Esperando credenciales...\n";
    std::cout << "Usuario: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pwd;

    // Inicializamos el entorno y la conexión:
    SQLHSTMT handler = SQL_NULL_HSTMT;
    SQLHENV entorno = SQL_NULL_HENV;
    SQLHDBC conexion = SQL_NULL_HDBC;

    // Le asignamos valores (dependientes del propio SO del usuario):

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &entorno);                  // Primero a null
    SQLSetEnvAttr(entorno, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0); // Establecemos los atributos de la conexión
    SQLAllocHandle(SQL_HANDLE_DBC, entorno, &conexion);                         // Reinicializamos la conexión, ya puestos dichos atributos

    // Intentamos conectar con las credenciales dadas:

    SQLRETURN validez = SQLConnectA( conexion, 
                                    (SQLCHAR*)dsn.c_str(), dsn.size(),  
                                    (SQLCHAR*)user.c_str(), user.size(),  
                                    (SQLCHAR*)pwd.c_str(), pwd.size());
             

    SQLAllocHandle(SQL_HANDLE_STMT, conexion, &handler);                        // Inicializamos el handler para las sentencias

    // Comprobamos si la conexión ha sido exitosa:

    if(!SQL_SUCCEEDED(validez)){

        std::cerr << "Conexion fallida. Puso bien las credenciales?\nFinalizando...\n";

        // Liberamos los recursos para una salida limpia del servidor:

        SQLFreeHandle(SQL_HANDLE_DBC, conexion);
        SQLFreeHandle(SQL_HANDLE_ENV, entorno);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        
        exit(-1);
    }

    // Borramos todas las tablas de la sesion anterior
    borrarTablas(handler, entorno, conexion);

    // Creamos la interfaz
    do {
        std::cout << "\n===== MENU PRINCIPAL =====\n";
        std::cout << "1. Borrar tablas\n";
        std::cout << "2. Crear tablas\n";
        std::cout << "3. Insertar tuplas\n";
        std::cout << "4. Dar de alta pedido\n";
        std::cout << "5. Mostrar contenido de tablas\n";
        std::cout << "6. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: borrarTablas(handler, entorno, conexion); break;
            case 2: crearTablas(handler, entorno, conexion); break;
            case 3: insertarTuplas(handler, entorno, conexion); break;
            case 4: darDeAltaPedido(handler, entorno, conexion); break;
            case 5: mostrarContenidoTablas(handler, entorno, conexion); break;
            case 6: std::cout << "Saliendo...\n"; break;
            default: std::cout << "Opcion no valida.\n"; break;
        }
    } while (opcion != 6);

    SQLExecDirectA(handler, (SQLCHAR*) "COMMIT;", SQL_NTS);

    // Liberamos los recursos para una salida limpia del servidor:
    SQLFreeHandle(SQL_HANDLE_DBC, conexion);
    SQLFreeHandle(SQL_HANDLE_ENV, entorno);
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Finalizando...\n";

    SQLDisconnect(conexion);

    return 0;
}
