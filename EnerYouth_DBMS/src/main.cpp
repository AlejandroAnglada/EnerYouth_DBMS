#include<windows.h>
#include<sqlext.h>
#include<sqltypes.h>
#include<sql.h>
#include<iostream>

void mostrarContenidoTablas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*) "SELECT * FROM Stock;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "SELECT * FROM Pedido;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "SELECT * FROM Detalle_Pedido;", SQL_NTS);
    std::cout << "Contenido de las tablas mostrado correctamente.\n";
}

void borrarTablas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT borrarTablas", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Detalle_Pedido;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Pedido;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Stock;", SQL_NTS);

    std::cout << "Tablas borradas correctamente.\n";
}

void crearTablas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT crearTablas", SQL_NTS);

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
        "Cpedido FOREIGN KEY REFERENCES Pedido(Cpedido),"
        "Cproducto FOREIGN KEY REFERENCES Stock(Cproducto),"
        "Cantidad INT,"
        "PRIMARY KEY (Cpedido, Cproducto)"
        ");", SQL_NTS);
}

void insertarTuplas(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {
    SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT insertarTuplas", SQL_NTS);

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

    std::cout << "Datos insertados correctamente.\n";
}

void darDeAltaPedido(SQLHSTMT handler, SQLHENV entorno, SQLHDBC conexion) {    
    int ccliente, cpedido, cproducto, opcion;
    std::string fecha;
    bool salida = false;

    std::cout << "Ingrese el codigo del pedido: ";
    std::cin >> cpedido;
    std::cout << "Ingrese el codigo del cliente: ";
    std::cin >> ccliente;
    std::cout << "Ingrese la fecha: ";
    std::cin >> fecha;
    std::cout << "Ingrese el producto: ";
    std::cin >> cproducto;

    SQLExecDirectA(handler, (SQLCHAR*)
    "INSERT INTO Pedido (Cpedido, Ccliente, Fecha_pedido) VALUES (cpedido, ccliente, fecha);", SQL_NTS);

    do {
        std::cout << "\n===== MENU ALTA PEDIDO =====\n";
        std::cout << "1. Añadir detalle de producto\n";
        std::cout << "2. Eliminar todos los detalles de producto\n";
        std::cout << "3. Cancelar pedido\n";
        std::cout << "4. Finalizar pedido\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT darDeAltaPedido_añadirDetalle", SQL_NTS);

                // Comprobamos si hay o no stock
                SQLExecDirectA(handler, (SQLCHAR*)
                "SELECT Cantidad FROM Stock WHERE Cproducto = cproducto;", SQL_NTS);

                // Usamos SQLFetch para obtener la cantidad y comprobar si hay stock
                SQLFetch(handler);
                int cantidad_disponible;
                SQLGetData(handler, 1, SQL_C_LONG, &cantidad_disponible, 0, NULL);

                if (cantidad_disponible > 0) {
                    int cantidad_solicitada;
                    std::cout << "Ingrese la cantidad solicitada: ";
                    std::cin >> cantidad_solicitada;

                    if (cantidad_solicitada <= cantidad_disponible) {
                        SQLExecDirectA(handler, (SQLCHAR*)
                        "INSERT INTO Detalle_Pedido (Cpedido, Cproducto, Cantidad) VALUES (cpedido, cproducto, cantidad_solicitada);", SQL_NTS);

                        // Actualizamos el stock
                        SQLExecDirectA(handler, (SQLCHAR*)
                        "UPDATE Stock SET Cantidad = Cantidad - cantidad_solicitada WHERE Cproducto = cproducto;", SQL_NTS);

                        std::cout << "Detalle de producto añadido correctamente.\n";
                    } else {
                        std::cout << "No hay suficiente stock disponible.\n";
                    }
                } else {
                    std::cout << "No hay stock disponible para este producto.\n";
                }
                
                std::cout << "Contenido actual de las tablas:\n";
                mostrarContenidoTablas(handler, entorno, conexion);
            break;
            case 2:
                SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT darDeAltaPedido_eliminarDetalle", SQL_NTS);

                SQLExecDirectA(handler, (SQLCHAR*)
                "SELECT Cpedido FROM Detalle_Pedido WHERE Cproducto = cproducto;", SQL_NTS);

                SQLExecDirectA(handler, (SQLCHAR*)
                "DELETE FROM Detalle_Pedido WHERE Cpedido = cpedido;", SQL_NTS);

                std::cout << "Contenido actual de las tablas:\n";
                mostrarContenidoTablas(handler, entorno, conexion);
            break;
            case 3:
                SQLExecDirectA(handler, (SQLCHAR*) "SAVEPOINT darDeAltaPedido_cancelarPedido", SQL_NTS);

                SQLExecDirectA(handler, (SQLCHAR*)
                "DELETE FROM Detalle_Pedido WHERE Cpedido = cpedido;", SQL_NTS);

                SQLExecDirectA(handler, (SQLCHAR*)
                "DELETE FROM Pedido WHERE Cpedido = cpedido;", SQL_NTS);

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

    // Aquí se debería agregar la lógica para insertar el pedido y actualizar el stock
    std::cout << "Pedido dado de alta correctamente.\n";
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
    std::cout << "Contraseña: ";
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

    // Comprobamos si la conexión ha sido exitosa:

    if(!SQL_SUCCEEDED(validez)){

        std::cerr << "Conexion fallida. Puso bien las credenciales?\nFinalizando...\n";

        // Liberamos los recursos para una salida limpia del servidor:

        SQLFreeHandle(SQL_HANDLE_DBC, conexion);
        SQLFreeHandle(SQL_HANDLE_ENV, entorno);
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        
        exit(-1);
    }

    // Aqui va el codigo
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
