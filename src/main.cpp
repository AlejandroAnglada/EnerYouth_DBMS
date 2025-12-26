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
#include "../include/GestionEmpleados.h"


void crearTablas(ConexionADB &conexion, SQLHSTMT handler) {
    SQLRETURN retCliente = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Cliente ("
            "DNI_CIF VARCHAR(9) PRIMARY KEY NOT NULL,"
            "Nombre VARCHAR(20) NOT NULL,"
            "Apellidos VARCHAR(80) NOT NULL,"
            "Direccion VARCHAR(100) NOT NULL,"
            "Telefono VARCHAR(9) NOT NULL,"
            "Email VARCHAR(100) NOT NULL,"
            "Estado VARCHAR(10) DEFAULT 'Activo' NOT NULL,"
            "CONSTRAINT UQ_CLIENTE_EMAIL UNIQUE (Email),"
            "CONSTRAINT UQ_CLIENTE_TLF UNIQUE (Telefono),"
            "CONSTRAINT CHK_CLIENTE_ESTADO CHECK (Estado IN ('Activo', 'Inactivo'))"
        ");", SQL_NTS);
    if (retCliente != SQL_SUCCESS && retCliente != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Cliente\n";
    }
    
    SQLRETURN retContrato = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Contrato ("
            "ID_Contrato NUMBER(9) PRIMARY KEY NOT NULL,"
            "CUPS VARCHAR(22) NOT NULL,"
            "Tipo_Contrato VARCHAR(20) NOT NULL,"
            "Potencia_Con NUMBER(6,2) NOT NULL,"
            "Tarifa VARCHAR(20) NOT NULL,"
            "IBAN VARCHAR(34) NOT NULL,"
            "Fecha_Inicio DATE DEFAULT SYSDATE NOT NULL,"
            "Fecha_Fin DATE,"
            "Estado VARCHAR(15) DEFAULT 'Activo' NOT NULL,"
            "CONSTRAINT UQ_TIPO_CONTRATO UNIQUE (Tipo_Contrato),"
            "CONSTRAINT CHK_CONTRATO_ESTADO CHECK (Estado IN ('Activo', 'Baja')),"
            "CONSTRAINT CHK_POTENCIA CHECK (Potencia_Con > 3)"
        ");", SQL_NTS);
    if (retContrato != SQL_SUCCESS && retContrato != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Contrato\n";
    }

    SQLRETURN retHogar = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Hogar ("
            "Direccion VARCHAR(100) PRIMARY KEY NOT NULL,"
            "DNI_Cliente VARCHAR(9) NOT NULL,"
            "ID_Contrato_H NUMBER(9) NOT NULL,"
            "Tipo_Contrato_H VARCHAR(20),"
            "Zona_Geografica VARCHAR(100),"
            "FOREIGN KEY (DNI_Cliente) REFERENCES Cliente(DNI_CIF),"
            "FOREIGN KEY (ID_Contrato_H) REFERENCES Contrato(ID_Contrato),"
            "FOREIGN KEY (Tipo_Contrato_H) REFERENCES Contrato(Tipo_Contrato)"
        ");", SQL_NTS);
    if (retHogar != SQL_SUCCESS && retHogar != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Hogar\n";
    }

    SQLRETURN retIncidencia = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Incidencia ("
            "ID_Incidencia NUMBER(9) NOT NULL,"
            "ID_Contrato_I NUMBER(9) NOT NULL,"
            "Descripcion_Incidencia VARCHAR(200),"
            "Tipo_Incidencia VARCHAR(100),"
            "Fecha_Incidencia DATE,"
            "Fecha_Resolucion DATE,"
            "Estado_Incidencia VARCHAR(50),"
            "CONSTRAINT PK_INCIDENCIA PRIMARY KEY (ID_Incidencia, ID_Contrato_I),"
            "FOREIGN KEY (ID_Contrato_I) REFERENCES Contrato(ID_Contrato)"
        ");", SQL_NTS);
    if (retIncidencia != SQL_SUCCESS && retIncidencia != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Incidencia\n";
    }

    SQLRETURN retInstalacionEnergetica = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Instalacion_Energetica ("
            "Nombre_Fuente_Energetica VARCHAR(60),"
            "Direccion_Instalaciones VARCHAR(180),"
            "Descripcion VARCHAR(180),"
            "Ingresos_Netos_Historicos NUMBER(20,2),"
            "Fecha_Fundacion DATE,"
            "Potencia_Actual NUMBER(3,0),"
            "CONSTRAINT Potencia_Valida CHECK (Potencia_Actual >= 0 AND Potencia_Actual <= 100),"
            "CONSTRAINT Clave_Primaria_IE PRIMARY KEY (Nombre_Fuente_Energetica, Direccion_Instalaciones)"
        ");", SQL_NTS);
    if (retInstalacionEnergetica != SQL_SUCCESS && retInstalacionEnergetica != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Instalacion_Energetica\n";
    }
    
    SQLRETURN retCesionPotencia = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Cesion_Potencia ("
            "Nombre_Fuente_Energetica_C VARCHAR(60) NOT NULL,"
            "Direccion_Instalaciones_C VARCHAR(180) NOT NULL,"
            "Nombre_Fuente_Energetica_R VARCHAR(60) NOT NULL,"
            "Direccion_Instalaciones_R VARCHAR(180) NOT NULL,"
            "FOREIGN KEY (Nombre_Fuente_Energetica_C, Direccion_Instalaciones_C) REFERENCES Instalacion_Energetica (Nombre_Fuente_Energetica, Direccion_Instalaciones),"
            "FOREIGN KEY (Nombre_Fuente_Energetica_R, Direccion_Instalaciones_R) REFERENCES Instalacion_Energetica (Nombre_Fuente_Energetica, Direccion_Instalaciones),"
            "CONSTRAINT Clave_Primaria_CesPot PRIMARY KEY (Nombre_Fuente_Energetica_C, Direccion_Instalaciones_C)"
        ");", SQL_NTS);
    if (retCesionPotencia != SQL_SUCCESS && retCesionPotencia != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Cesion_Potencia\n";
    }

    SQLRETURN retEmpleado = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Empleado ("
            "DNI VARCHAR(9) PRIMARY KEY NOT NULL,"
            "Nombre VARCHAR(20) NOT NULL,"
            "Apellidos VARCHAR(40) NOT NULL,"
            "Telefono VARCHAR(20),"
            "Correo_Electronico VARCHAR(30),"
            "Posicion_Empresa VARCHAR(20),"
            "Ventas NUMBER(9) DEFAULT 0,"
            "Incentivo NUMBER(10) DEFAULT 0"
        ");", SQL_NTS);
    if (retEmpleado != SQL_SUCCESS && retEmpleado != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Empleado\n";
    }

    SQLRETURN retSoluciona = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Soluciona ("
            "DNI VARCHAR(9) NOT NULL,"
            "ID_Incidencia NUMBER(9) NOT NULL,"
            "ID_Contrato_I NUMBER(9) NOT NULL,"
            "FOREIGN KEY (DNI) REFERENCES Empleado(DNI),"
            "FOREIGN KEY (ID_Incidencia, ID_Contrato_I) REFERENCES Incidencia (ID_Incidencia, ID_Contrato_I),"
            "CONSTRAINT PK_SOLUCIONA PRIMARY KEY (DNI, ID_Incidencia, ID_Contrato_I)"
        ");", SQL_NTS);
    if (retSoluciona != SQL_SUCCESS && retSoluciona != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Soluciona\n";
    }

    SQLRETURN retContratado = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Contratado ("
            "DNI VARCHAR(9) NOT NULL,"
            "Direccion VARCHAR(100) NOT NULL,"
            "CONSTRAINT PK_Contratado PRIMARY KEY (DNI, Direccion),"
            "FOREIGN KEY (DNI) REFERENCES Cliente(DNI_CIF),"
            "FOREIGN KEY (Direccion) REFERENCES Hogar(Direccion)"
        ");", SQL_NTS);
    if (retContratado != SQL_SUCCESS && retContratado != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Contratado\n";
    }

    SQLRETURN retRegistradaEn = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Registrada_en ("
            "ID_Incidencia NUMBER(9) NOT NULL,"
            "Contrato_asociado NUMBER(9) NOT NULL,"
            "FOREIGN KEY (ID_Incidencia, Contrato_asociado) REFERENCES Incidencia (ID_Incidencia, ID_Contrato_I),"           "FOREIGN KEY (Contrato_asociado) REFERENCES Contrato(ID_Contrato),"
            "PRIMARY KEY (ID_Incidencia, Contrato_asociado)"
        ");", SQL_NTS);
    if (retRegistradaEn != SQL_SUCCESS && retRegistradaEn != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Registrada_en\n";
    }

    SQLRETURN retAsociado = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE TABLE Asociado ("
            "DNI_Asociado VARCHAR(9) NOT NULL,"
            "ID_Contrato_A NUMBER(9) NOT NULL,"
            "FOREIGN KEY (DNI_Asociado) REFERENCES Cliente(DNI_CIF),"
            "CONSTRAINT PK_ASOCIADO PRIMARY KEY (DNI_Asociado, ID_Contrato_A)"
        ");", SQL_NTS);
    if (retAsociado != SQL_SUCCESS && retAsociado != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando tabla Asociado\n";
    }

    std::cout << "\nTablas creadas correctamente.\n";

    SQLEndTran(SQL_HANDLE_DBC, conexion.getConnection(), SQL_COMMIT);
}

void mostrarContenidoTablas(ConexionADB &conexion, SQLHSTMT handler) {
    // Tabla Incidencia
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Incidencia;", SQL_NTS);
    std::cout << "--Tabla Incidencia:--\n";
    SQLINTEGER ID_Incidencia, ID_Contrato_I;
    SQLVARCHAR Descripcion_Incidencia[201], Tipo_Incidencia[101], Estado_Incidencia[51];
    SQLCHAR Fecha_Incidencia[11], Fecha_Resolucion[11];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &ID_Incidencia, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ID_Contrato_I, 0, NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Descripcion_Incidencia, sizeof(Descripcion_Incidencia), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Tipo_Incidencia, sizeof(Tipo_Incidencia), NULL);
        SQLGetData(handler, 7, SQL_C_CHAR, Estado_Incidencia, sizeof(Estado_Incidencia), NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Fecha_Incidencia, sizeof(Fecha_Incidencia), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, Fecha_Resolucion, sizeof(Fecha_Resolucion), NULL);
        std::cout << "ID_Incidencia:" << ID_Incidencia << "\n";
        std::cout << "ID_Contrato_I:" << ID_Contrato_I << "\n";
        std::cout << "Descripcion_Incidencia:" << Descripcion_Incidencia << "\n";
        std::cout << "Tipo_Incidencia:" << Tipo_Incidencia << "\n";
        std::cout << "Estado_Incidencia:" << Estado_Incidencia << "\n";
        std::cout << "Fecha_Incidencia:" << Fecha_Incidencia << "\n";
        std::cout << "Fecha_Resolucion:" << Fecha_Resolucion << "\n\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Hogar
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Hogar;", SQL_NTS);
    std::cout << "\n--Tabla Hogar:--\n";
    SQLVARCHAR Direccion[101], DNI_Cliente[10], Tipo_Contrato[21], Zona_Geografica[101];
    SQLINTEGER ID_Contrato_H;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, Direccion, sizeof(Direccion), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, DNI_Cliente, sizeof(DNI_Cliente), NULL);
        SQLGetData(handler, 3, SQL_C_LONG, &ID_Contrato_H, 0, NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Tipo_Contrato, sizeof(Tipo_Contrato), NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Zona_Geografica, sizeof(Zona_Geografica), NULL);
        std::cout << "Direccion:"  << Direccion << "\n";
        std::cout << "DNI_Cliente:"  << DNI_Cliente << "\n";
        std::cout << "ID_Contrato_H:"  << ID_Contrato_H << "\n";
        std::cout << "Tipo_Contrato:"  << Tipo_Contrato << "\n";
        std::cout << "Zona_Geografica:"  << Zona_Geografica << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Instalacion_Energetica
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Instalacion_Energetica;", SQL_NTS);
    std::cout << "\n--Tabla Instalacion_Energetica:--\n";
    SQLVARCHAR Nombre_Fuente_Energetica[61], Direccion_Instalaciones[181], Descripcion[181];
    SQLDOUBLE Ingresos_Netos_Historicos;
    SQLCHAR Fecha_Fundacion[11];
    SQLINTEGER Potencia_Actual;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, Nombre_Fuente_Energetica, sizeof(Nombre_Fuente_Energetica), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Direccion_Instalaciones, sizeof(Direccion_Instalaciones), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Descripcion, sizeof(Descripcion), NULL);
        SQLGetData(handler, 4, SQL_C_DOUBLE, &Ingresos_Netos_Historicos, 0, NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Fecha_Fundacion, sizeof(Fecha_Fundacion), NULL);
        SQLGetData(handler, 6, SQL_C_LONG, &Potencia_Actual, 0, NULL);
        std::cout << "Nombre_Fuente_Energetica:" << Nombre_Fuente_Energetica << "\n";
        std::cout << "Direccion_Instalaciones:" << Direccion_Instalaciones << "\n";
        std::cout << "Descripcion:" << Descripcion << "\n";
        std::cout << "Ingresos_Netos_Historicos:" << Ingresos_Netos_Historicos << "\n";
        std::cout << "Fecha_Fundacion:" << Fecha_Fundacion << "\n";
        std::cout << "Potencia_Actual:" << Potencia_Actual << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Cesion_Potencia
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Cesion_Potencia;", SQL_NTS);
    std::cout << "\n--Tabla Cesion_Potencia:--\n";
    SQLVARCHAR Nombre_Fuente_Energetica_C[61], Direccion_Instalaciones_C[181];
    SQLVARCHAR Nombre_Fuente_Energetica_R[61], Direccion_Instalaciones_R[181];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, Nombre_Fuente_Energetica_C, sizeof(Nombre_Fuente_Energetica_C), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Direccion_Instalaciones_C, sizeof(Direccion_Instalaciones_C), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Nombre_Fuente_Energetica_R, sizeof(Nombre_Fuente_Energetica_R), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Direccion_Instalaciones_R, sizeof(Direccion_Instalaciones_R), NULL);
        std::cout << "Nombre_Fuente_Energetica_C:" << Nombre_Fuente_Energetica_C << "\n";
        std::cout << "Direccion_Instalaciones_C:" << Direccion_Instalaciones_C << "\n";
        std::cout << "Nombre_Fuente_Energetica_R:" << Nombre_Fuente_Energetica_R << "\n";
        std::cout << "Direccion_Instalaciones_R:" << Direccion_Instalaciones_R << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);
    
    // Tabla Empleado
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Empleado;", SQL_NTS); 
    std::cout << "\n--Tabla Empleado:--\n";
    SQLVARCHAR DNI[10], Nombre[21], Apellidos[41], Telefono[21], Correo_Electronico[31], Posicion_Empresa[21];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI, sizeof(DNI), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Nombre, sizeof(Nombre), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Apellidos, sizeof(Apellidos), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Telefono, sizeof(Telefono), NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Correo_Electronico, sizeof(Correo_Electronico), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, Posicion_Empresa, sizeof(Posicion_Empresa), NULL);
        std::cout << "DNI:" << DNI << "\n";
        std::cout << "Nombre:" << Nombre << "\n";
        std::cout << "Apellidos:" << Apellidos << "\n";
        std::cout << "Telefono:" << Telefono << "\n";
        std::cout << "Correo_Electronico:" << Correo_Electronico << "\n";
        std::cout << "Posicion_Empresa:" << Posicion_Empresa << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Soluciona
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Soluciona;", SQL_NTS);
    std::cout << "\n--Tabla Soluciona:--\n";
    SQLVARCHAR DNI_S[10];
    SQLINTEGER ID_Incidencia_S, ID_Contrato_I_S;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_S, sizeof(DNI_S), NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ID_Incidencia_S, 0, NULL);
        SQLGetData(handler, 3, SQL_C_LONG, &ID_Contrato_I_S, 0, NULL);
        std::cout << "DNI_S:" << DNI_S << "\n";
        std::cout << "ID_Incidencia_S:" << ID_Incidencia_S << "\n";
        std::cout << "ID_Contrato_I_S:" << ID_Contrato_I_S << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Contratado
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Contratado;", SQL_NTS);
    std::cout << "\n--Tabla Contratado:--\n";
    SQLVARCHAR DNI_C[10], Direccion_C[101];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_C, sizeof(DNI_C), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Direccion_C, sizeof(Direccion_C), NULL);
        std::cout << "DNI_C:" << DNI_C << "\n";
        std::cout << "Direccion_C:" << Direccion_C << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Cliente
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Cliente;", SQL_NTS);
    std::cout << "\n--Tabla Cliente:--\n";
    SQLVARCHAR DNI_CIF[10], Nombre_Cl[21], Apellidos_Cl[81], Direccion_Cl[101], Telefono_Cl[10], Email_Cl[101], Estado[11];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_CIF, sizeof(DNI_CIF), NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, Nombre_Cl, sizeof(Nombre_Cl), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Apellidos_Cl, sizeof(Apellidos_Cl), NULL);
        SQLGetData(handler, 4, SQL_C_CHAR, Direccion_Cl, sizeof(Direccion_Cl), NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Telefono_Cl, sizeof(Telefono_Cl), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, Email_Cl, sizeof(Email_Cl), NULL);
        SQLGetData(handler, 7, SQL_C_CHAR, Estado, sizeof(Estado), NULL);
        std::cout << "DNI_CIF:" << DNI_CIF << "\n";
        std::cout << "Nombre_Cl:" << Nombre_Cl << "\n";
        std::cout << "Apellidos_Cl:" << Apellidos_Cl << "\n";
        std::cout << "Direccion_Cl:" << Direccion_Cl << "\n";
        std::cout << "Telefono_Cl:" << Telefono_Cl << "\n";
        std::cout << "Email_Cl:" << Email_Cl << "\n";
        std::cout << "Estado:" << Estado << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Registrada_en
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Registrada_en;", SQL_NTS);
    std::cout << "\n--Tabla Registrada_en:--\n";
    SQLINTEGER ID_Incidencia_R, Contrato_asociado;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &ID_Incidencia_R, 0, NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &Contrato_asociado, 0, NULL);
        std::cout << "ID_Incidencia_R:" << ID_Incidencia_R << "\n";
        std::cout << "Contrato_asociado:" << Contrato_asociado << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Contrato
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Contrato;", SQL_NTS);
    std::cout << "\n--Tabla Contrato:--\n";
    SQLINTEGER ID_Contrato;
    SQLVARCHAR CUPS[23], Tipo_Contrato_Con[21], Tarifa[21], IBAN[35], Estado_Con[16];
    SQLDOUBLE Potencia_Con;
    SQLCHAR Fecha_Inicio[11], Fecha_Fin[11];
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_LONG, &ID_Contrato, 0, NULL);
        SQLGetData(handler, 2, SQL_C_CHAR, CUPS, sizeof(CUPS), NULL);
        SQLGetData(handler, 3, SQL_C_CHAR, Tipo_Contrato_Con, sizeof(Tipo_Contrato_Con), NULL);
        SQLGetData(handler, 4, SQL_C_DOUBLE, &Potencia_Con, 0, NULL);
        SQLGetData(handler, 5, SQL_C_CHAR, Tarifa, sizeof(Tarifa), NULL);
        SQLGetData(handler, 6, SQL_C_CHAR, IBAN, sizeof(IBAN), NULL);
        SQLGetData(handler, 7, SQL_C_CHAR, Fecha_Inicio, sizeof(Fecha_Inicio), NULL);
        SQLGetData(handler, 8, SQL_C_CHAR, Fecha_Fin, sizeof(Fecha_Fin), NULL);
        SQLGetData(handler, 9, SQL_C_CHAR, Estado_Con, sizeof(Estado_Con), NULL);
        std::cout << "ID_Contrato:" << ID_Contrato << "\n";
        std::cout << "CUPS:" << CUPS << "\n";
        std::cout << "Tipo_Contrato_Con:" << Tipo_Contrato_Con << "\n";
        std::cout << "Potencia_Con:" << Potencia_Con << "\n";
        std::cout << "Tarifa:" << Tarifa << "\n";
        std::cout << "IBAN:" << IBAN << "\n";
        std::cout << "Fecha_Inicio:" << Fecha_Inicio << "\n";
        std::cout << "Fecha_Fin:" << Fecha_Fin << "\n";
        std::cout << "Estado_Con:" << Estado_Con << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    // Tabla Asociado
    SQLExecDirectA(handler, (SQLCHAR*)"SELECT * FROM Asociado;", SQL_NTS);
    std::cout << "\n--Tabla Asociado:--\n";
    SQLVARCHAR DNI_Asociado[10];
    SQLINTEGER ID_Contrato_A;
    while (SQLFetch(handler) == SQL_SUCCESS) {
        SQLGetData(handler, 1, SQL_C_CHAR, DNI_Asociado, sizeof(DNI_Asociado), NULL);
        SQLGetData(handler, 2, SQL_C_LONG, &ID_Contrato_A, 0, NULL);
        std::cout << "DNI_Asociado:" << DNI_Asociado << "\n";
        std::cout << "ID_Contrato_A:" << ID_Contrato_A << "\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

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
        std::cout << "\nGestion de Transmision/Distribucion de energia seleccionada.\n";
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
                std::string direccion, dni_cliente, tipo_contrato, zona_geografica;
                int id_contrato;

                std::cin.ignore();
                std::cout << "Introduzca la direccion del hogar a dar de alta: ";
                std::getline(std::cin, direccion);
                std::cout << "Introduzca el DNI del cliente asociado al hogar: ";
                std::getline(std::cin, dni_cliente);
                std::cout << "Introduzca el ID del contrato asociado al hogar: ";
                std::cin >> id_contrato;
                std::cout << "Introduzca el tipo de contrato del contrato asociado al hogar: ";
                std::cin.ignore();
                std::getline(std::cin, tipo_contrato);
                std::cout << "Introduzca la zona geografica del hogar: ";
                std::getline(std::cin, zona_geografica);

                if (hogares.altaHogar(direccion, dni_cliente, id_contrato, tipo_contrato, zona_geografica)) {
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
                std::string direccion, dni_cliente, nuevo_dni_cliente;
                DatosContrato datos_contrato;

                std::cin.ignore();
                std::cout << "Introduzca el DNI del cliente: ";
                std::getline(std::cin, dni_cliente);
                std::cout << "Introduzca la direccion del hogar de dicho cliente que desea modificar: ";
                std::getline(std::cin, direccion);
                std::cout << "Introduzca el nuevo DNI del cliente asociado al hogar (deje vacio para no modificar): ";
                std::getline(std::cin, nuevo_dni_cliente);
                std::cout << "Introduzca el nuevo CUPS del contrato asociado al hogar (deje vacio para no modificar): ";
                std::getline(std::cin, datos_contrato.cups);
                std::cout << "Introduzca el nuevo tipo de contrato (deje vacio para no modificar): ";
                std::getline(std::cin, datos_contrato.tipo_contrato);
                std::cout << "Introduzca la nueva tarifa (deje vacio para no modificar): ";
                std::getline(std::cin, datos_contrato.tarifa);
                std::cout << "Introduzca la nueva potencia contratada (deje vacio para no modificar): ";
                std::string potencia_str;
                std::getline(std::cin, potencia_str);

                if (!potencia_str.empty()) {
                    datos_contrato.potencia_contratada = std::stod(potencia_str);
                } else {
                    datos_contrato.potencia_contratada = -1;
                }

                if (hogares.modificarHogar(direccion, dni_cliente, nuevo_dni_cliente, datos_contrato)) {
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
                std::cin.ignore();
                std::cout << "Introduzca la fecha de resolucion (YYYY-MM-DD): ";
                std::getline(std::cin, fecha_resolucion);
                std::cout << "Introduzca el estado de la incidencia: ";
                std::getline(std::cin, estado_incidencia);
                if (hogares.registrarIncidencia(id_incidencia, id_contrato_i, descripcion_incidencia,
                                               tipo_incidencia, fecha_incidencia, fecha_resolucion,
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

void crearTriggerActualizarDatosHogar(ConexionADB &conexion, SQLHSTMT handler) {
    // Creamos un trigger (disparador) en la base de datos y si el trigger ya existiera, se reemplaza
    // en caso de que se ejecutase varias veces para evitar errores.
    // Con AFTER UPDATE le indicamos al trigger que se debe ejecutar despues de actualizar la tabla,
    // para este caso la de Contrato y la de Cliente.
    // El trigger se ejecutará para cada fila que se actualice, no una vez por la sentencia completa.
    // Con OF (...) indicamos que se activa solo si se modifica la columna indica entre parentesis y 
    // con ON (...) indicamos la tabla a la que pertenece el trigger.
    // Todo el bloque que está entre BEGIN y END es lo que se ejecuta cuando el trigger se dispara, en este
    // caso que se actualice la tabla Hogar con el nuevo Tipo_Contrato o DNI_Cliente debido a que son claves
    // externas a esas tablas y daría error al intentar modificarlas manualmente.

    SQLRETURN retTriggerTipoContrato = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE OR REPLACE TRIGGER trg_update_tipo_contrato "
        "AFTER UPDATE OF Tipo_Contrato ON Contrato "
        "FOR EACH ROW "
        "BEGIN "
        "UPDATE Hogar "
        "SET Tipo_Contrato_H = :NEW.Tipo_Contrato "
        "WHERE ID_Contrato_H = :OLD.ID_Contrato; "
        "END;", SQL_NTS);

    if (retTriggerTipoContrato != SQL_SUCCESS && retTriggerTipoContrato != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando trigger trg_update_tipo_contrato\n";
    } else {
        std::cout << "Trigger trg_update_tipo_contrato creado correctamente.\n";
    }
    
    SQLRETURN retTriggerDNICliente = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE OR REPLACE TRIGGER trg_update_dni_cliente "
        "AFTER UPDATE OF DNI_CIF ON Cliente "
        "FOR EACH ROW "
        "BEGIN "
        "UPDATE Hogar "
        "SET DNI_Cliente = :NEW.DNI_CIF "
        "WHERE DNI_Cliente = :OLD.DNI_CIF; "
        "END;", SQL_NTS);
    
    if (retTriggerDNICliente != SQL_SUCCESS && retTriggerDNICliente != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando trigger trg_update_dni_cliente\n";
    } else {
        std::cout << "Trigger trg_update_dni_cliente creado correctamente.\n";
    }
}


void gestionEmpleados(GestionEmpleados &empleados) {
    int opcion_empleado;

    do {
        std::cout << "\nGestion de Empleados seleccionada.\n";
        std::cout << "--Bienvenido al subsistema de gestion de empleados---\n";
        std::cout << "Aqui podra contratar, despedir y modificar empleados, modificar contratos, mostrar datos de los empleados, calcular incentivos y solucionar incidencias.\n"; 
        std::cout << "\n===== MENU GESTION DE EMPLEADOS =====\n";
        std::cout << "1. Contratar empleado\n";
        std::cout << "2. Despedir empleado\n";
        std::cout << "3. Listar emeplados\n";
        std::cout << "4. Modificar contrato\n";
        std::cout << "5. Calcular incentivos\n";
        std::cout << "6. Solucionar incidencia\n";
        std::cout << "7. Volver al menu principal\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion_empleado;

        switch (opcion_empleado) {
            case 1: {
                std::string dni_empleado, nombre, apellidos, telefono, correo_electronico, puesto;

                std::cin.ignore();
                std::cout << "Introduzca el DNI del empleado: ";
                std::getline(std::cin, dni_empleado);
                std::cout << "Introduzca el nombre del empleado: ";
                std::getline(std::cin, nombre);
                std::cout << "Introduzca los apellidos del empleado: ";
                std::getline(std::cin, apellidos);
                std::cout << "Introduzca el número de teléfono del empleado: ";
                std::cin.ignore();
                std::getline(std::cin, telefono);
                std::cout << "Introduzca el correo electrónico del empleado: ";
                std::getline(std::cin, correo_electronico);
                std::cout << "Introduzca el puesto del empleado en la empresa: ";
                std::cin.ignore();
                std::getline(std::cin, puesto);
                if (empleados.contratarEmpleado(dni_empleado, nombre, apellidos, telefono, correo_electronico, puesto)) {
                     std::cout << "Empleado contratado correctamente.\n";
                } else {
                    std::cout << "Error al contratar el empleado.\n";
                }
                break;
            }
            case 2:
                {
                    std::string dni_empleado;
                    std::cin.ignore();
                    std::cout << "Introduzca el DNI del empleado a despeddir: ";
                    std::getline(std::cin, dni_empleado);

                    if (empleados.despedirEmpleado(dni_empleado)) {
                        std::cout << "Empleado despedido correctamente.\n";
                    } else {
                        std::cout << "Error al despedir al empleado.\n";
                    }
                }
                break;
            case 3:
                {

                    std::vector<EmpleadoInfo> empleados_listados = empleados.mostrarEmpleados();
                    std::cout << "Empleados encontrados:\n";
                    for (const auto& e : empleados_listados) {
                        std::cout << "DNI: " << e.dni_empleado << "\n";
                        std::cout << "Nombre: " << e.nombre << "\n";
                        std::cout << "Apellidos: " << e.apellidos << "\n";
                        std::cout << "Telefono: " << e.telefono << "\n";
                        std::cout << "Correo Electronico: " << e.correo_electronico << "\n";
                        std::cout << "Puesto: " << e.puesto << "\n";
                        std::cout << "Ventas:" << e.ventas << "\n\n";
                    }
                }
                break;
            case 4: {
                std::string dni_empleado;

                std::cin.ignore();
                std::cout << "Introduzca el DNI del empleado: ";
                std::getline(std::cin, dni_empleado);

                std::string datos_modificados = empleados.modificarEmpleado(dni_empleado);
                if (!datos_modificados.empty()) {
                    std::cout << "Empleado modificado correctamente. Nuevos datos:\n" << datos_modificados << "\n";
                } else {
                    std::cout << "Error al modificar el empleado.\n";
                }
                break;
            }
            case 5: {
                
                std::pair<std::vector<EmpleadoInfo>,int> incentivo_empleados = empleados.incentivoParaEmpleados();
                    std::cout << "Empleados encontrados:\n";
                    for (const auto& e : incentivo_empleados.first) {
                        std::cout << "DNI: " << e.dni_empleado << "\n";
                        std::cout << "Nombre: " << e.nombre << "\n";
                        std::cout << "Apellidos: " << e.apellidos << "\n";
                        std::cout << "Telefono: " << e.telefono << "\n";
                        std::cout << "Correo Electronico: " << e.correo_electronico << "\n";
                        std::cout << "Puesto: " << e.puesto << "\n";
                        std::cout << "Ventas:" << e.ventas << "\n";
                        std::cout << "Incentivo: " << incentivo_empleados.second << "\n\n";
                    }               
                     break;
            }
            case 6: {
                std::string dni_empleado, estado_incidencia;
                int id_incidencia;

                std::cin.ignore();
                std::cout << "Introduzca el DNI del empleado que soluciona la incidencia: ";
                std::getline(std::cin, dni_empleado);
                std::cout << "Introduzca el ID de la incidencia a solucionar: ";
                std::cin >> id_incidencia;
                std::cout << "Introduzca el estado de la incidencia.";
                std::getline(std::cin, estado_incidencia);
                std::pair<std::string,std::string> solucion_incidecias = empleados.solucionIncidencias(dni_empleado, id_incidencia, estado_incidencia);
                if (!solucion_incidecias.first.empty()) {
                    std::cout << "DNI del empleado encargado:\n" << solucion_incidecias.first << "\n";
                    std::cout << "Nuevo estado de la incidencia:\n" << solucion_incidecias.second << "\n";
                } else {
                    std::cout << "Error al solucionar la incidencia.\n";
                }                     
                break;
            }
            case 7:
                std::cout << "Volviendo al menu principal...\n";
                break;
            default:
                std::cout << "Opcion no valida. Intente de nuevo.\n";
                break;
        }
    } while (opcion_empleado != 7);
}

void crearTriggerVentas(ConexionADB &conexion, SQLHSTMT handler) {
    // Creamos un trigger (disparador) que se ejecuta automáticamante cuando se actualiza el número de ventas
    //de un empleado. El objetivo es recalcualr el incentivo de forma automáticay garantizar
    //que la regla de negocio se cumpla siempre, independientemiente de que la actualización
    //venga desde la aplicación o directamente desde la base de datos.
    //
    //AFTER UPDATE OF Ventas indica que el trigger se dispara después de modificar la columna Ventas
    //de la tabla Empleado.
    //
    //FOR EACH ROW indica que el trigger se ejecuta para cada fila que se actualice.
    //
    // El bloque entre BEGIN y END contiene la lógica del trigger:
    // - Si las ventas nuevas (:NEW.ventas) son mayores que 30, se calcula el incentivo como
    //   (ventas - 30) * 100 y se actualiza la columna incentivo del empleado correspondiente.
    // - Si las ventas nuevas son 30 o menos, se establece el incentivo a 0.

    SQLRETURN retTrigger = SQLExecDirectA(handler, (SQLCHAR*)
        "CREATE OR REPLACE TRIGGER trg_calcular_incentivo_empleado "
        "AFTER UPDATE OF Ventas ON Empleado "
        "FOR EACH ROW "
        "BEGIN "
        "   IF :NEW.Ventas > 30 THEN "
        "       UPDATE Empleado "
        "       SET Incentivo = (:NEW.ventas - 30)*100 "
        "       WHERE DNI = :NEW.DNI; "
        "   ELSE "
        "       UPDATE Empleado "
        "       SET Incentivo = 0 "
        "       WHERE DNI = :NEW.DNI; "
        "   END IF; "
        "END;", SQL_NTS);

    if (retTrigger != SQL_SUCCESS && retTrigger != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error creando trigger trg_calcular_incentivo_empleado\n";
    } else {
        std::cout << "Trigger trg_calcular_incentivo_empleado creado correctamente.\n";
    }

}

int main(int argc, char ** argv){
    std::string user, pwd;
    int opcion;

    // Declaramos el entorno y la conexión:
    SQLHSTMT handler;
    ConexionADB conexion;

    // Pedimos las credenciales al usuario para conectarse a la db:
    std::string dsn = "practbd"; // Nombre del DSN, puesto por defecto tal y como en la explicación del S1
    std::cout << "Esperando credenciales...\n";
    std::cout << "Usuario: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pwd;

    // Creamos la conexión
    if (!conexion.connect(dsn, user, pwd)) {
        std::cerr << "Error al conectar con la BD\n";
        return 1;
    }
    
    SQLHDBC con = conexion.getConnection();
    handler = SQL_NULL_HSTMT;

    SQLAllocHandle(SQL_HANDLE_STMT, con, &handler);

    GestionTransmisionDistribucion hogares(conexion);
    GestionEmpleados empleados(conexion);

    // Borramos todas las tablas de la sesion anterior
    borrarTablas(conexion, handler);

    // Creamos las tablas (prueba)
    crearTablas(conexion, handler);

    // Creamos el trigger para actualizar el tipo de contrato en Hogar al modificarlo en Contrato
    crearTriggerActualizarDatosHogar(conexion, handler);

    // Inserto DNI en cliente e ID contrato en Contrato para poder dar de alta hogares
    int id_contrato = 1;
    char cups[23] = "ES12345678901256890123";
    char tipo_contrato[21] = "Residencial";
    double potencia_con = 5.5;
    char tarifa[20] = "Tarifa1";
    char iban[35] = "ES7620770024003102575766";
    char contrato[2048];
    sprintf(contrato, "INSERT INTO Contrato (ID_Contrato, CUPS, Tipo_Contrato, Potencia_Con, Tarifa, IBAN) VALUES (%d, '%s', '%s', %f, '%s', '%s');", id_contrato, cups, tipo_contrato, potencia_con, tarifa, iban);
    SQLRETURN retContrato = SQLExecDirectA(handler, (SQLCHAR*) contrato, SQL_NTS);
    if (retContrato != SQL_SUCCESS && retContrato != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error ejecutando SQL\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    SQLEndTran(SQL_HANDLE_DBC, conexion.getConnection(), SQL_COMMIT);

    char dni[10] = "12345678A";
    char nombre[21] = "Juan";
    char apellidos[81] = "Perez Gomez";
    char direccion[101] = "Calle Falsa 123";
    char telefono[10] = "600123468";
    char email[101] = "juan@gmail.com";
    char cliente[2048];
    sprintf(cliente, "INSERT INTO Cliente (DNI_CIF, nombre, apellidos, direccion, telefono, email) VALUES ('%s', '%s', '%s', '%s', '%s', '%s');", dni, nombre, apellidos, direccion, telefono, email);
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*) cliente, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error ejecutando SQL\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);

    SQLEndTran(SQL_HANDLE_DBC, conexion.getConnection(), SQL_COMMIT);
    
    // Mostramos las tablas (prueba)
    mostrarContenidoTablas(conexion, handler);

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
            case 1: gestionTransmisionDistribucion(hogares); break;
            case 2: 

            case 3: gestionEmpleados(empleados); break;

            case 4: 

            case 5: mostrarContenidoTablas(conexion, handler); break;

            case 6: std::cout << "Saliendo...\n"; break;
            default: std::cout << "Opcion no valida.\n"; break;
        }
    } while (opcion != 6);

    SQLFreeHandle(SQL_HANDLE_STMT, handler);

    std::cout << "Finalizando...\n";

    conexion.disconnect();

    return 0;
}
