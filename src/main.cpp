// Porfa, portabilidad; recordad que no todos los usuarios usan Windows.
#ifdef _WIN32
    #include <windows.h>
#endif
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string>
#include "../include/ConexionADB.h"
#include "../include/GestionTransmisionDistribucion.h"


void crearTablas(ConexionADB &conexion, SQLHSTMT handler) {
    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Incidencia ("
            "ID_Incidencia INT(9) NOT NULL,"
            "ID_Contrato_I INT(9) NOT NULL"
            "Descripcion_Incidencia VARCHAR2(200),"
            "Tipo_Incidencia VARCHAR2(100),"
            "Fecha_Incidencia DATE,"
            "Fecha_Resolucion DATE,"
            "Estado_Incidencia VARCHAR2(50),"
            "CONSTRAINT PK_INCIDENCIA PRIMARY KEY (ID_Incidencia, ID_Contrato_I),"
            "FOREIGN KEY (ID_Contrato_I) REFERENCES Contrato(ID_Contrato)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Hogar ("
            "Direccion VARCHAR2(100) PRIMARY KEY NOT NULL,"
            "DNI_Cliente VARCHAR2(9) NOT NULL,"
            "Tipo_Contrato VARCHAR2(20) NOT NULL,"
            "Zona_Geografica VARCHAR2(100) NOT NULL,"
            "FOREIGN KEY (DNI_Cliente) REFERENCES Cliente(DNI_CIF)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Instalacion_Energetica ("
            "Nombre_Fuente_Energetica VARCHAR2(60),"
            "Direccion_Instalaciones VARCHAR2(180),"
            "Descripcion VARCHAR2(180),"
            "Ingresos_Netos_Historicos DOUBLE(20,2)"
            "Fecha_Fundacion DATE,"
            "Potencia_Actual NUMBER(3,0),"
            "CONSTRAINT Potencia_Valida CHECK (Potencia_Actual >= 0 AND Potencia_Actual <= 100),"
            "CONSTRAINT Clave_Primaria_IE PRIMARY KEY (Nombre_Fuente_Energetica, Direccion_Instalaciones)"
        ");", SQL_NTS);
    
    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Cesion_Potencia ("
            "FOREIGN KEY Nombre_Fuente_Energetica_C REFERENCES Instalacion_Energetica (Nombre_Fuente_Energetica)"
            "FOREIGN KEY Direccion_Instalaciones_C REFERENCES Instalacion_Energetica (Direccion_Instalaciones),"
            "FOREIGN KEY Nombre_Fuente_Energetica_R REFERENCES Instalacion_Energetica (Nombre_Fuente_Energetica)"
            "FOREIGN KEY Direccion_Instalaciones_R REFERENCES Instalacion_Energetica (Direccion_Instalaciones),"
            "CONSTRAINT Clave_Primaria_CesPot PRIMARY KEY (Nombre_Fuente_Energetica_C, Direccion_Instalaciones_C)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Empleado ("
            "DNI VARCHAR2(9) PRIMARY KEY NOT NULL,"
            "Nombre VARCHAR2(20) NOT NULL,"
            "Apellidos VARCHAR2(40) NOT NULL,"
            "Telefono VARCHAR2(20),"
            "Correo_Electronico VARCHAR2(30),"
            "Posicion_Empresa VARCHAR2(20)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Soluciona ("
            "DNI VARCHAR2(9) NOT NULL,"
            "ID_Incidencia INT(9) NOT NULL,"
            "ID_Contrato_I INT(9) NOT NULL,"
            "FOREIGN KEY (DNI) REFERENCES Empleado(DNI),"
            "FOREIGN KEY (ID_Incidencia) REFERENCES Incidencia(ID_Incidencia),"
            "FOREIGN KEY (ID_Contrato_I) REFERENCES Contrato(ID_Contrato),"
            "PRIMARY KEY (DNI, ID_Incidencia, ID_Contrato_I)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Contratado ("
            "DNI VARCHAR2(9) NOT NULL,"
            "Direccion VARCHAR2(100) NOT NULL,"
            "CONSTRAINT PK_Contratado PRIMARY KEY (DNI, Direccion),"
            "FOREIGN KEY (DNI) REFERENCES Cliente(DNI_CIF),"
            "FOREIGN KEY (Direccion) REFERENCES Hogar(Direccion)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Cliente ("
            "DNI_CIF VARCHAR2(9) PRIMARY KEY NOT NULL,"
            "Nombre VARCHAR2(20) NOT NULL,"
            "Apellidos VARCHAR2(80) NOT NULL,"
            "Direccion VARCHAR2(100) NOT NULL,"
            "Telefono VARCHAR2(9) NOT NULL,"
            "Email VARCHAR2(100) NOT NULL,"
            "CONSTRAINT UQ_CLIENTE_ID UNIQUE (ID_Cliente),"
            "CONSTRAINT UQ_CLIENTE_EMAIL UNIQUE (Email),"
            "CONSTRAINT UQ_CLIENTE_TLF UNIQUE (Telefono),"
            "CONSTRAINT CHK_CLIENTE_ESTADO CHECK (Estado IN ('Activo', 'Inactivo'))"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Registrada_en ("
            "ID_Incidencia INT(9) NOT NULL,"
            "Contrato_asociado INT(9) NOT NULL,"
            "FOREIGN KEY (ID_Incidencia) REFERENCES Incidencia(ID_Incidencia),"
            "FOREIGN KEY (Contrato_asociado) REFERENCES Contrato(ID_Contrato),"
            "PRIMARY KEY (ID_Incidencia, Contrato_asociado)"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Contrato ("
            "ID_Contrato INT(9) PRIMARY KEY NOT NULL,"
            "CUPS VARCHAR2(22) NOT NULL,"
            "Tipo_Contrato VARCHAR2(20) NOT NULL,"
            "Potencia_Con DOUBLE(6,2) NOT NULL,"
            "Tarifa VARCHAR2(20) NOT NULL,"
            "IBAN VARCHAR2(34) NOT NULL,"
            "Fecha_Inicio DATE DEFAULT SYSDATE NOT NULL,"
            "Fecha_Fin DATE,"
            "Estado VARCHAR2(15) DEFAULT 'Activo' NOT NULL,"
            "CONSTRAINT CHK_CONTRATO_ESTADO CHECK (Estado IN ('Activo', 'Baja')),"
            "CONSTRAINT CHK_POTENCIA CHECK (Potencia_Con > 3),"
            "CONSTRAINT FK_CONTRATO_CLIENTE FOREIGN KEY (DNI_Titular)"
                "REFERENCES Cliente (DNI_CIF)"
                "ON DELETE CASCADE"
        ");", SQL_NTS);

    SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Asociado ("
            "DNI_Asociado VARCHAR2(9) NOT NULL,"
            "ID_Contrato_A INT(9) NOT NULL,"
            "FOREIGN KEY (DNI_Asociado) REFERENCES Cliente(DNI_CIF),"
            "CONSTRAINT PK_ASOCIADO PRIMARY KEY (DNI_Asociado, ID_Contrato_A)"
        ");", SQL_NTS);
}

void mostrarContenidoTablas(ConexionADB &conexion, SQLHSTMT handler) {
    // Tabla Incidencia
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Incidencia;", SQL_NTS);
    std::cout << "--Tabla Incidencia:--\n";
    SQLINTEGER ID_Incidencia, ID_Contrato_I;
    SQLVARCHAR Descripcion_Incidencia[200], Tipo_Incidencia[100], Estado_Incidencia[50];
    SQLDATE Fecha_Incidencia, Fecha_Resolucion;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &ID_Incidencia, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ID_Contrato_I, 0, NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Descripcion_Incidencia, sizeof(Descripcion_Incidencia), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Tipo_Incidencia, sizeof(Tipo_Incidencia), NULL);
        SQLGetData(handler, 7, SQL_C_CHAR, Estado_Incidencia, sizeof(Estado_Incidencia), NULL);
        SQLGetData(handler, 5, SQL_C_DATE, &Fecha_Incidencia, 0, NULL);
        SQLGetData(handler, 6, SQL_C_DATE, &Fecha_Resolucion, 0, NULL);
        std::cout << ID_Incidencia << "\t" << ID_Contrato_I << "\n";
    }

    // Tabla Hogar
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Hogar;", SQL_NTS);
    std::cout << "\n--Tabla Hogar:--\n";
    SQLVARCHAR Direccion[100], DNI_Cliente[9], Tipo_Contrato[20], Zona_Geografica[100];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, Direccion, sizeof(Direccion), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, DNI_Cliente, sizeof(DNI_Cliente), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Tipo_Contrato, sizeof(Tipo_Contrato), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Zona_Geografica, sizeof(Zona_Geografica), NULL);
        std::cout << Direccion << "\t" << DNI_Cliente << "\n";
    }

    // Tabla Instalacion_Energetica
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Instalacion_Energetica;", SQL_NTS);
    std::cout << "\n--Tabla Instalacion_Energetica:--\n";
    SQLVARCHAR Nombre_Fuente_Energetica[60], Direccion_Instalaciones[180], Descripcion[180];
    SQLDOUBLE Ingresos_Netos_Historicos;
    SQLDATE Fecha_Fundacion;
    SQLINTEGER Potencia_Actual;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, Nombre_Fuente_Energetica, sizeof(Nombre_Fuente_Energetica), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Direccion_Instalaciones, sizeof(Direccion_Instalaciones), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Descripcion, sizeof(Descripcion), NULL);
        SQLGetData(handler, 4, SQL_C_DOUBLE, &Ingresos_Netos_Historicos, 0, NULL);
        SQLGetData(handler, 5, SQL_C_DATE, &Fecha_Fundacion, 0, NULL);
        SQLGetData(handler, 6, SQL_C_LONG, &Potencia_Actual, 0, NULL);
        std::cout << Nombre_Fuente_Energetica << "\t" << Direccion_Instalaciones << "\n";
    }

    // Tabla Cesion_Potencia
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Cesion_Potencia;", SQL_NTS);
    std::cout << "\n--Tabla Cesion_Potencia:--\n";
    SQLVARCHAR Nombre_Fuente_Energetica_C[60], Direccion_Instalaciones_C[180];
    SQLVARCHAR Nombre_Fuente_Energetica_R[60], Direccion_Instalaciones_R[180];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, Nombre_Fuente_Energetica_C, sizeof(Nombre_Fuente_Energetica_C), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Direccion_Instalaciones_C, sizeof(Direccion_Instalaciones_C), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Nombre_Fuente_Energetica_R, sizeof(Nombre_Fuente_Energetica_R), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Direccion_Instalaciones_R, sizeof(Direccion_Instalaciones_R), NULL);
        std::cout << Nombre_Fuente_Energetica_C << "\t" << Direccion_Instalaciones_C << "\n";
    }
    
    // Tabla Empleado
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Empleado;", SQL_NTS); 
    std::cout << "\n--Tabla Empleado:--\n";
    SQLVARCHAR DNI[9], Nombre[20], Apellidos[40], Telefono[20], Correo_Electronico[30], Posicion_Empresa[20];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI, sizeof(DNI), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Nombre, sizeof(Nombre), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Apellidos, sizeof(Apellidos), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Telefono, sizeof(Telefono), NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Correo_Electronico, sizeof(Correo_Electronico), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, Posicion_Empresa, sizeof(Posicion_Empresa), NULL);
        std::cout << DNI << "\t" << Nombre << "\n";
    }

    // Tabla Soluciona
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Soluciona;", SQL_NTS);
    std::cout << "\n--Tabla Soluciona:--\n";
    SQLVARCHAR DNI_S[9];
    SQLINTEGER ID_Incidencia_S, ID_Contrato_I_S;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_S, sizeof(DNI_S), NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ID_Incidencia_S, 0, NULL);
        SQLGetData(handler, 3, SQL_C_LONG, &ID_Contrato_I_S, 0, NULL);
        std::cout << DNI_S << "\n";
    }

    // Tabla Contratado
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Contratado;", SQL_NTS);
    std::cout << "\n--Tabla Contratado:--\n";
    SQLVARCHAR DNI_C[9], Direccion_C[100];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_C, sizeof(DNI_C), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Direccion_C, sizeof(Direccion_C), NULL);
        std::cout << DNI_C << "\t" << Direccion_C << "\n";
    }

    // Tabla Cliente
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Cliente;", SQL_NTS);
    std::cout << "\n--Tabla Cliente:--\n";
    SQLVARCHAR DNI_CIF[9], Nombre_Cl[20], Apellidos_Cl[80], Direccion_Cl[100], Telefono_Cl[9], Email_Cl[100];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_CIF, sizeof(DNI_CIF), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Nombre_Cl, sizeof(Nombre_Cl), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Apellidos_Cl, sizeof(Apellidos_Cl), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Direccion_Cl, sizeof(Direccion_Cl), NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Telefono_Cl, sizeof(Telefono_Cl), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, Email_Cl, sizeof(Email_Cl), NULL);
        std::cout << DNI_CIF << "\t" << Nombre_Cl << "\n";
    }

    // Tabla Registrada_en
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Registrada_en;", SQL_NTS);
    std::cout << "\n--Tabla Registrada_en:--\n";
    SQLINTEGER ID_Incidencia_R, Contrato_asociado;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &ID_Incidencia_R, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &Contrato_asociado, 0, NULL);
        std::cout << ID_Incidencia_R << "\t" << Contrato_asociado << "\n";
    }

    // Tabla Contrato
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Contrato;", SQL_NTS);
    std::cout << "\n--Tabla Contrato:--\n";
    SQLINTEGER ID_Contrato;
    SQLVARCHAR CUPS[22], Tipo_Contrato_Con[20], Tarifa[20], IBAN[34];
    SQLDOUBLE Potencia_Con;
    SQLDATE Fecha_Inicio, Fecha_Fin;
    SQLVARCHAR Estado[15];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &ID_Contrato, 0, NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, CUPS, sizeof(CUPS), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Tipo_Contrato_Con, sizeof(Tipo_Contrato_Con), NULL);
        SQLGetData(handler, 4, SQL_C_DOUBLE, &Potencia_Con, 0, NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Tarifa, sizeof(Tarifa), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, IBAN, sizeof(IBAN), NULL);
        SQLGetData(handler, 7, SQL_C_DATE, &Fecha_Inicio, 0, NULL);
        SQLGetData(handler, 8, SQL_C_DATE, &Fecha_Fin, 0, NULL);
        SQLGetData(handler, 9, SQL_C_CHAR, Estado, sizeof(Estado), NULL);
        std::cout << ID_Contrato << "\t" << CUPS << "\n";
    }

    // Tabla Asociado
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Asociado;", SQL_NTS);
    std::cout << "\n--Tabla Asociado:--\n";
    SQLVARCHAR DNI_Asociado[9];
    SQLINTEGER ID_Contrato_A;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_Asociado, sizeof(DNI_Asociado), NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ID_Contrato_A, 0, NULL);
        std::cout << DNI_Asociado << "\n";
    }

}

void borrarTablas(ConexionADB &conexion, SQLHSTMT handler) {
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Soluciona;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Registrada_en;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Cesion_Potencia;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Contratado;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Asociado;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Incidencia;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Hogar;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Empleado;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Contrato;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Instalacion_Energetica;", SQL_NTS);
    SQLExecDirectA(handler, (SQLCHAR*) "DROP TABLE Cliente;", SQL_NTS);

    std::cout << "\nTablas borradas correctamente.\n";

    SQLEndTran(SQL_HANDLE_DBC, conexion.getConnection(), SQL_COMMIT);
}

void gestionTransmisionDistribucion(GestionTransmisionDistribucion &hogares) {
    int opcion_hogar;

    do {
        std::cout << "Gestion de Transmision/Distribucion de energia seleccionada.\n";
        std::cout << "--Bienvenido al subsistema de gestion de transmision y distribucion de energia---\n";
        std::cout << "Aqui podra gestionar los hogares, contratos e incidencias relacionadas con la transmision y distribucion de energia.\n"; 
        std::cout << "\n===== MENU GESTION DE TRANSMISION/DISTRIBUCION DE ENERGIA =====\n";
        std::cout << "1. Dar de alta hogar\n";
        std::cout << "2. Dar de baja hogar\n";
        std::cout << "3. Listar hogares\n";
        std::cout << "4. Modificar hogar\n";
        std::cout << "5. Registrar incidencia\n";
        std::cout << "6. Volver al menu principal\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion_hogar;

        switch (opcion_hogar) {
            case 1: {
                std::string direccion, dni_cliente;
                int id_contrato;

                std::cin.ignore();
                std::cout << "Introduzca la direccion del hogar a dar de alta: ";
                std::getline(std::cin, direccion);
                std::cout << "Introduzca el DNI del cliente asociado al hogar: ";
                std::getline(std::cin, dni_cliente);
                std::cout << "Introduzca el ID del contrato asociado al hogar: ";
                std::cin >> id_contrato;

                if (hogares.altaHogar(direccion, dni_cliente, id_contrato)) {
                    std::cout << "Hogar dado de alta correctamente.\n";
                } else {
                    std::cout << "Error al dar de alta el hogar.\n";
                }
                break;
            }
            case 2:
                {
                    std::string direccion;
                    std::cin.ignore();
                    std::cout << "Introduzca la direccion del hogar a dar de baja: ";
                    std::getline(std::cin, direccion);

                    if (hogares.bajaHogar(direccion)) {
                        std::cout << "Hogar dado de baja correctamente.\n";
                    } else {
                        std::cout << "Error al dar de baja el hogar.\n";
                    }
                }
                break;
            case 3:
                {
                    std::string zona_geografica, dni_cliente, tipo_contrato;
                    std::cin.ignore();
                    std::cout << "Introduzca la zona geografica para filtrar (deje vacio para no filtrar): ";
                    std::getline(std::cin, zona_geografica);
                    std::cout << "Introduzca el DNI del cliente para filtrar (deje vacio para no filtrar): ";
                    std::getline(std::cin, dni_cliente);
                    std::cout << "Introduzca el tipo de contrato para filtrar (deje vacio para no filtrar): ";
                    std::getline(std::cin, tipo_contrato);

                    std::vector<HogarInfo> hogares_listados = hogares.listarHogares(zona_geografica, dni_cliente, tipo_contrato);
                    std::cout << "Hogares encontrados:\n";
                    for (const auto& hogar : hogares_listados) {
                        std::cout << "Direccion: " << hogar.direccion << "\n";
                    }
                }
                break;
            case 4: {
                std::string direccion, dni_cliente;
                DatosContrato datos_contrato;
                std::cin.ignore();
                std::cout << "Introduzca la direccion del hogar a modificar (deje vacio para no modificar): ";
                std::getline(std::cin, direccion);
                std::cout << "Introduzca el nuevo DNI del cliente asociado al hogar (deje vacio para no modificar): ";
                std::getline(std::cin, dni_cliente);
                std::cout << "Introduzca el nuevo CUPS del contrato asociado al hogar (deje vacio para no modificar): ";
                std::getline(std::cin, datos_contrato.cups);
                std::cout << "Introduzca el nuevo tipo de contrato (deje vacio para no modificar): ";
                std::getline(std::cin, datos_contrato.tipo_contrato);
                std::cout << "Introduzca la nueva tarifa (deje vacio para no modificar): ";
                std::getline(std::cin, datos_contrato.tarifa);
                std::cout << "Introduzca la nueva potencia contratada (deje vacio para no modificar): ";
                std::cin >> datos_contrato.potencia_contratada;
                if (hogares.modificarHogar(direccion, dni_cliente, datos_contrato)) {
                    std::cout << "Hogar modificado correctamente.\n";
                } else {
                    std::cout << "Error al modificar el hogar.\n";
                }
                break;
            }
            case 5: {
                int id_incidencia, id_contrato_i;
                std::string descripcion_incidencia, tipo_incidencia, estado_incidencia, fecha_resolucion, fecha_incidencia;
                std::cin.ignore();
                std::cout << "Introduzca el ID de la incidencia: ";
                std::cin >> id_incidencia;
                std::cin.ignore();
                std::cout << "Introduzca el tipo de incidencia: ";
                std::getline(std::cin, tipo_incidencia);
                std::cout << "Introduzca la descripcion de la incidencia: ";
                std::getline(std::cin, descripcion_incidencia);
                std::cout << "Introduzca el ID del contrato asociado a la incidencia: ";
                std::cin >> id_contrato_i;
                std::cin.ignore();
                std::cout << "Introduzca la fecha de la incidencia (YYYY-MM-DD): ";
                std::getline(std::cin, fecha_incidencia);
                std::cout << "Introduzca la fecha de resolucion (YYYY-MM-DD): ";
                std::getline(std::cin, fecha_resolucion);
                std::cout << "Introduzca el estado de la incidencia: ";
                std::getline(std::cin, estado_incidencia);
                if (hogares.registrarIncidencia(id_incidencia, tipo_incidencia, descripcion_incidencia,
                                               id_contrato_i, fecha_incidencia, fecha_resolucion,
                                               estado_incidencia)) {
                    std::cout << "Incidencia registrada correctamente.\n";
                } else {
                    std::cout << "Error al registrar la incidencia.\n";
                }
                break;
            }
            case 6:
                std::cout << "Volviendo al menu principal...\n";
                break;
            default:
                std::cout << "Opcion no valida. Intente de nuevo.\n";
                break;
        }
    } while (opcion_hogar != 6);
}
int main(int argc, char ** argv){
    std::string user, pwd;
    int opcion;

    // Inicializamos el entorno y la conexión:
    SQLHSTMT handler = SQL_NULL_HSTMT;
    ConexionADB conexion;

    // Pedimos las credenciales al usuario para conectarse a la db:
    std::string dsn = "practbd"; // Nombre del DSN, puesto por defecto tal y como en la explicación del S1
    std::cout << "Esperando credenciales...\n";
    std::cout << "Usuario: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pwd;

    // Intentamos conectar con las credenciales dadas:
    SQLRETURN validez = SQLConnectA( conexion.getConnection(), 
                                    (SQLCHAR*)dsn.c_str(), dsn.size(),  
                                    (SQLCHAR*)user.c_str(), user.size(),  
                                    (SQLCHAR*)pwd.c_str(), pwd.size());
             

    SQLAllocHandle(SQL_HANDLE_STMT, conexion.getConnection(), &handler);                        // Inicializamos el handler para las sentencias

    // Creamos la conexión
    if (!conexion.connect(dsn, user, pwd)) {
        std::cerr << "Error al conectar con la BD\n";
        return 1;
    }

    GestionTransmisionDistribucion hogares(conexion);

    // Borramos todas las tablas de la sesion anterior
    borrarTablas(conexion, handler);

    // Creamos la interfaz
    do {
        std::cout << "\n===== MENU PRINCIPAL =====\n";
        std::cout << "1. Gestion de Transmision/Distribucion de energia\n";
        std::cout << "2. Gestion de Recursos Energeticos\n";
        std::cout << "3. Gestion de Empleados\n";
        std::cout << "4. Gestion de Clientes\n";
        std::cout << "5. Mostrar contenido de tablas\n";
        std::cout << "6. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:gestionTransmisionDistribucion(hogares); break;
            case 2: 

            case 3: 

            case 4: 

            case 5: 

            case 6: std::cout << "Saliendo...\n"; break;
            default: std::cout << "Opcion no valida.\n"; break;
        }
    } while (opcion != 6);

    SQLExecDirectA(handler, (SQLCHAR*) "COMMIT;", SQL_NTS);
    
    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Finalizando...\n";

    conexion.disconnect();

    return 0;
}
