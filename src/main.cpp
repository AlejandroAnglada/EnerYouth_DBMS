// Porfa, portabilidad; recordad que no todos los usuarios usan Windows.
#ifdef _WIN32
    #include <windows.h>
#endif
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string>

void crearTablas(SQLHENV &entorno, SQLHDBC &conexion, SQLHSTMT &handler) {
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
