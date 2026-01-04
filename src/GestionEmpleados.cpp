#include "../include/GestionEmpleados.h"
#include "../include/ConexionADB.h"
#include <sqltypes.h>
#include <iostream>
#include <algorithm>
#include <regex>        // Para validaciones básicas
#include <sstream>      // Para conversiones simples
#include <ctime>        // Para obtener la fecha actual
#include <iomanip>      // Para formatear fechas


// Función para escapar comillas simples en cadenas SQL
std::string escapeSQLE(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\'') result += "''"; // duplicamos la comilla simple
        else result += c;
    }
    return result;
}

// Función para mostrar errores SQL
void mostrarErrorE(SQLHSTMT stmt) {
    SQLCHAR sqlState[6], msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT msgLen;

    SQLGetDiagRecA(
        SQL_HANDLE_STMT,
        stmt,
        1,
        sqlState,
        &nativeError,
        msg,
        sizeof(msg),
        &msgLen
    );

    std::cerr << "SQLSTATE: " << sqlState << "\n";
    std::cerr << "Mensaje : " << msg << "\n";
}

bool GestionEmpleados::validarFormatoDNI_E(const std::string& dni_cif) const {
    if (dni_cif.empty() || dni_cif.length() > 9)
        return false;

    // Expresiones regulares para validar DNI/CIF
    // Para DNI: 8 dígitos seguidos de una letra
    std::regex patron_dni("^[0-9]{8}[A-Za-z]$");
    // Para CIF: 1 letra, 8 dígitos
    std::regex patron_cif("^[A-Za-z][0-9]{8}$");

    return std::regex_match(dni_cif, patron_dni) || std::regex_match(dni_cif, patron_cif);
}

bool GestionEmpleados::validarFormatoEmail_E(const std::string& email) const {
    // Expresión regular para validar correo electrónico (blabla@blabla.blabla)
    std::regex patron_email("^[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return std::regex_match(email, patron_email);
}

bool GestionEmpleados::validarFormatoTelefono_E(const std::string& telefono) const {
    if (telefono.length() != 9 && telefono.length() != 12 && telefono.length() !=13) // 9 dígitos o prefijo seguido de 9 dígitos
        return false;

    // Expresión regular para validar que todo son números
    std::regex patron_telefono(R"(^(\+\d{1,3})?\d{9}$)");
    return std::regex_match(telefono, patron_telefono);
}


GestionEmpleados::GestionEmpleados(ConexionADB& con)
    : conexion(con) {
}

   
    /* ========================= RF-4.1 ========================= */
    bool GestionEmpleados::contratarEmpleado(const std::string& dni_empleado, const std::string& nombre, 
                                             const std::string& apellidos, const std::string& telefono,
                                             const std::string& correo_electronico, const std::string& puesto){
        //Comprobar que la conexión esté establecida
        if (!conexion.isConnected()) {
            return false;
        }

            // Validaciones de formato
        if (!validarFormatoDNI_E(dni_empleado)) {
            std::cout << "Error: DNI/CIF con formato incorrecto.\n";
            return false;
        }

        if (!validarFormatoEmail_E(correo_electronico)) {
            std::cout << "Error: Email con formato incorrecto.\n";
            return false;
        }

        if (!validarFormatoTelefono_E(telefono)) {
            std::cout << "Error: Teléfono con formato incorrecto (debe ser 9 dígitos).\n";
            return false;
        }


        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return false;
        }

        // Damos de alta el empleado
        char empleado[2048];
        std::string dni_empleado_esc = escapeSQLE(dni_empleado);
        std::string nombre_esc = escapeSQLE(nombre);
        std::string apellidos_esc = escapeSQLE(apellidos);
        std::string telefono_esc = escapeSQLE(telefono);
        std::string correo_electronico_esc = escapeSQLE(correo_electronico);
        std::string puesto_esc = escapeSQLE(puesto);
        sprintf(empleado, "INSERT INTO Empleado (DNI, Nombre, Apellidos, Telefono, Correo_Electronico, Posicion_Empresa) "
                          "VALUES ('%s', '%s', '%s', '%s', '%s', '%s');",
                                    dni_empleado_esc.c_str(), nombre_esc.c_str(), apellidos_esc.c_str(),      
                                    telefono_esc.c_str(), correo_electronico_esc.c_str(), puesto_esc.c_str());
       
                                    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)empleado, SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para contratar al empleado.\n";
            mostrarErrorE(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return false;
        }
        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return true;
    
    }


    /* ========================= RF-4.2 ========================= */

    bool GestionEmpleados::despedirEmpleado(const std::string& dni_empleado){
        //Comprobar que la conexión esté establecida
        if (!conexion.isConnected()) {
            return false;
        }

        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return false;
        }
        // Damos de baja el empleado
        char empleado[2048];
        std::string dni_empleado_esc = escapeSQLE(dni_empleado);
        sprintf(empleado, "DELETE FROM Empleado WHERE DNI = '%s';", dni_empleado_esc.c_str());
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)empleado, SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para despedir al empleado.\n";
            mostrarErrorE(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return false;
        }
        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return true;

    }  

    /* ========================= RF-4.3 ========================= */
  
    std::vector<EmpleadoInfo> GestionEmpleados::mostrarEmpleados(){
        std::vector<EmpleadoInfo> empleados;
        //Comprobar que la conexión esté establecida
        if (!conexion.isConnected()) {
            return empleados;
        }
        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler = SQL_NULL_HSTMT;
        
        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return empleados;
        }
        // Ejecutamos la consulta para obtener todos los empleados
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)"SELECT DNI, Nombre, Apellidos, Telefono, Correo_Electronico, Posicion_Empresa, Ventas FROM Empleado;", SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para mostrar los empleados.\n";
            mostrarErrorE(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return empleados;
        }
        // Buffers para obtener los datos
        char dni_buf[10];
        char nombre_buf[21];
        char apellidos_buf[41];
        char telefono_buf[21];
        char correo_buf[31];
        char puesto_buf[21];
        char ventas_buf[10];
        char incentivo_buf[10];
        // Procesamos los resultados
        while (SQLFetch(handler) == SQL_SUCCESS) {
            SQLGetData(handler, 1, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
            SQLGetData(handler, 2, SQL_C_CHAR, nombre_buf, sizeof(nombre_buf), NULL);
            SQLGetData(handler, 3, SQL_C_CHAR, apellidos_buf, sizeof(apellidos_buf), NULL);
            SQLGetData(handler, 4, SQL_C_CHAR, telefono_buf, sizeof(telefono_buf), NULL);
            SQLGetData(handler, 5, SQL_C_CHAR, correo_buf, sizeof(correo_buf), NULL);
            SQLGetData(handler, 6, SQL_C_CHAR, puesto_buf, sizeof(puesto_buf), NULL);
            SQLLEN len_v = 0;
            SQLGetData(handler, 7, SQL_C_CHAR, ventas_buf, sizeof(ventas_buf), &len_v);
            if(len_v == SQL_NULL_DATA || len_v == 0) {
                ventas_buf[0] = '0';
                ventas_buf[1] = '\0';
            }
            EmpleadoInfo empleado;
            empleado.dni_empleado = std::string(dni_buf);
            empleado.nombre = std::string(nombre_buf);
            empleado.apellidos = std::string(apellidos_buf);
            empleado.telefono = std::string(telefono_buf);
            empleado.correo_electronico = std::string(correo_buf);
            empleado.puesto = std::string(puesto_buf);
            empleado.ventas = std::stoi(ventas_buf);
            empleados.push_back(empleado);

        }
        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return empleados;

    }   
    
    /* ========================= RF-4.4 ========================= */

    std::string GestionEmpleados::modificarEmpleado(const std::string& dni_empleado){
        //Comprobar que la conexión esté establecida
        std::string datos_modificados;
        if (!conexion.isConnected()) {
            return datos_modificados;
        }
        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler = SQL_NULL_HSTMT;
        
        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return datos_modificados;
        }
        // Ejecutamos la consulta para obtener el empleado a modificar
        std::string dni_empleado_esc = escapeSQLE(dni_empleado);
        std::string consulta = "SELECT DNI, Nombre, Apellidos, Telefono, Correo_Electronico, Posicion_Empresa FROM Empleado WHERE DNI = '" + dni_empleado_esc + "';";
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para modificar el empleado.\n";
            mostrarErrorE(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return datos_modificados;
        }
        // Comprobamos si el empleado existe
        if (SQLFetch(handler) != SQL_SUCCESS) {
            std::cout << "El empleado con DNI " << dni_empleado << " no existe.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return datos_modificados;;
        }
        
        SQLFreeStmt(handler, SQL_CLOSE);

        int n = 0;
        std::cin.ignore(); // Limpiar el buffer de entrada
        

        do {
            std::cout << "Ingrese el número correspondiente al campo que desea modificar:\n";
            std::cout << "1. Nombre\n";
            std::cout << "2. Apellidos\n";
            std::cout << "3. Teléfono\n";
            std::cout << "4. Correo Electrónico\n";
            std::cout << "5. Puesto\n";
            std::cout << "6. Ventas\n";
            std::cout << "7. Salario\n";
            std::cout << "8. Salir\n";
            std::cin >> n;
            std::cin.ignore(); // Limpiar el buffer de entrada
            std::string campo;
            switch(n){
                case 1:campo = "Nombre"; break;
                case 2:campo = "Apellidos"; break;
                case 3:campo = "Telefono"; break;
                case 4:campo = "Correo_Electronico"; break;
                case 5:campo = "Posicion_Empresa"; break;
                case 6:campo = "Ventas"; break;
                case 7: campo = "Salario"; break;
                case 8:{
                    std::cout << "Saliendo de la modificación de empleado.\n";
                    return datos_modificados;
                    break;
                }
                default:
                   std::cout << "Opción no válida. Intente de nuevo.\n";
                }
            std::string nuevo;
            std::cout << "Ingrese el nuevo " << campo << ":";
            std::getline(std::cin, nuevo);
            std::string actualizar;
            if(campo == "Telefono"){
                if (!validarFormatoTelefono_E(nuevo)) {
                    std::cout << "Error: Teléfono con formato incorrecto (debe ser 9 dígitos).\n";
                    continue;
                }
            }
            if(campo == "Correo_Electronico"){
                if (!validarFormatoEmail_E(nuevo)) {
                    std::cout << "Error: Email con formato incorrecto.\n";
                    continue;
                }
            }
            if(campo == "Ventas" || campo == "Salario"){
                actualizar = "UPDATE Empleado SET " + campo + " = '" + nuevo + "' WHERE DNI = '" + dni_empleado_esc + "';";

            } else {
                actualizar = "UPDATE Empleado SET " + campo + " = '" + escapeSQLE(nuevo) + "' WHERE DNI = '" + dni_empleado_esc + "';";
            }  
            SQLRETURN retActualizar = SQLExecDirectA(handler, (SQLCHAR*)actualizar.c_str(), SQL_NTS);
            if (retActualizar != SQL_SUCCESS && retActualizar != SQL_SUCCESS_WITH_INFO) {
                std::cout << "Error al ejecutar la sentencia SQL para actualizar el empleado.\n";
                mostrarErrorE(handler);
                SQLFreeHandle(SQL_HANDLE_STMT, handler);
                return datos_modificados;
            } else {
                datos_modificados += "Campo " + campo + " actualizado a " + nuevo + ".\n";
            }
        } while(n != 7);
        

        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return datos_modificados;
    }

    /* ========================= RF-4.5 ========================= */

    std::vector<EmpleadoInfo> GestionEmpleados::incentivoParaEmpleados (){
        std::vector<EmpleadoInfo> incentivos ={};
        //Comprobar que la conexión esté establecida
        if (!conexion.isConnected()) {
            return incentivos;
        }
        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler = SQL_NULL_HSTMT;
        
        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return incentivos;
        }
        // Comprobamos quién obtiene incentivo, número de ventas > 30
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)"SELECT DNI, Nombre, Apellidos, Telefono, Correo_Electronico, Posicion_Empresa, Ventas, Incentivo FROM Empleado WHERE Ventas > 30;", SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para mostrar los empleados con incentivos.\n";
            mostrarErrorE(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return incentivos;
        }
        // Buffers para obtener los datos
        char dni_buf[10];
        char nombre_buf[21];
        char apellidos_buf[41];
        char telefono_buf[21];
        char correo_buf[31];
        char puesto_buf[21];
        int ventas_buf, incentivo_buf;
        // Procesamos los resultados
        while (SQLFetch(handler) == SQL_SUCCESS) {
            SQLGetData(handler, 1, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
            SQLGetData(handler, 2, SQL_C_CHAR, nombre_buf, sizeof(nombre_buf), NULL);
            SQLGetData(handler, 3, SQL_C_CHAR, apellidos_buf, sizeof(apellidos_buf), NULL);
            SQLGetData(handler, 4, SQL_C_CHAR, telefono_buf, sizeof(telefono_buf), NULL);
            SQLGetData(handler, 5, SQL_C_CHAR, correo_buf, sizeof(correo_buf), NULL);
            SQLGetData(handler, 6, SQL_C_CHAR, puesto_buf, sizeof(puesto_buf), NULL);
            SQLGetData(handler, 7, SQL_C_LONG, &ventas_buf, 0, NULL);
            SQLGetData(handler, 8, SQL_C_LONG, &incentivo_buf, 0, NULL);
            if(ventas_buf > 30){
                EmpleadoInfo empleado;
                empleado.dni_empleado = std::string(dni_buf);
                empleado.nombre = std::string(nombre_buf);
                empleado.apellidos = std::string(apellidos_buf);
                empleado.telefono = std::string(telefono_buf);
                empleado.correo_electronico = std::string(correo_buf);
                empleado.puesto = std::string(puesto_buf);
                empleado.ventas = ventas_buf;
                empleado.incentivo = incentivo_buf; 
                incentivos.push_back(empleado);
            }
        }
        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return incentivos;
    }

    /*========================= RF-4.6 =========================*/
  
    std::pair<std::string, std::string> GestionEmpleados::solucionIncidencias(std::string& dni_empleado,int id_incidencia, std::string estado_incidencia){
        std::pair<std::string, std::string> resultado;
        //Comprobar que la conexión esté establecida
        if (!conexion.isConnected()) {
            return resultado;
        }
        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler_empleado = SQL_NULL_HSTMT;
        
        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler_empleado) != SQL_SUCCESS) {
            return resultado;
        }
        // Ejecutamos la consulta para obtener todos los empleados
        std::string dni_empleado_esc = escapeSQLE(dni_empleado);
        std::string consulta = "SELECT DNI, Nombre, Apellidos, Telefono, Correo_Electronico, Posicion_Empresa FROM Empleado WHERE DNI = '" + dni_empleado_esc + "';";
        SQLRETURN ret = SQLExecDirectA(handler_empleado, (SQLCHAR*)consulta.c_str(), SQL_NTS);
        
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para modificar el empleado.\n";
            mostrarErrorE(handler_empleado);
            SQLFreeHandle(SQL_HANDLE_STMT, handler_empleado);
            return resultado;
        }
       
        // Comprobamos si el empleado existe
        if (SQLFetch(handler_empleado) != SQL_SUCCESS) {
            std::cout << "El empleado con DNI " << dni_empleado << " no existe.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, handler_empleado);
            return resultado;;
        }
        
        SQLFreeHandle(SQL_HANDLE_STMT, handler_empleado);

        SQLHSTMT handler_estado = SQL_NULL_HSTMT;
        if(SQLAllocHandle(SQL_HANDLE_STMT, con, &handler_estado) != SQL_SUCCESS) {
            return resultado;
        }
        // Actualizamos el estado de la incidencia
        std::string estado_esc = escapeSQLE(estado_incidencia);
        std::string actualizar = "UPDATE Incidencia SET Estado_Incidencia = '" + estado_esc + "' WHERE ID_Incidencia = " +  std::to_string(id_incidencia);
        SQLRETURN retActualizar = SQLExecDirectA(handler_estado, (SQLCHAR*)actualizar.c_str(), SQL_NTS);
        if (retActualizar != SQL_SUCCESS && retActualizar != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para actualizar la incidencia.\n";
            mostrarErrorE(handler_estado);
            SQLFreeHandle(SQL_HANDLE_STMT, handler_estado);
            return resultado;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, handler_estado);
        
        //Obtener ID_Contrato_I de la incidencia
        int id_contrato_i;
        SQLHSTMT handler_contrato = SQL_NULL_HSTMT;

        if(SQLAllocHandle(SQL_HANDLE_STMT, con, &handler_contrato) != SQL_SUCCESS) {
            return resultado;
        }

        std::string consulta_contrato = "SELECT ID_Contrato_I FROM Incidencia WHERE ID_Incidencia = '" + std::to_string(id_incidencia) + "';";
        SQLRETURN ret_contrato = SQLExecDirectA(handler_contrato, (SQLCHAR*)consulta_contrato.c_str(), SQL_NTS);
        if (ret_contrato != SQL_SUCCESS && ret_contrato != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para obtener el ID_Contrato_I.\n";
            mostrarErrorE(handler_contrato);
            SQLFreeHandle(SQL_HANDLE_STMT, handler_contrato);
            return resultado;
        }

        if(SQLFetch(handler_contrato) != SQL_SUCCESS) {
            std::cout << "No se encontró el contrato asociado a la incidencia " << id_incidencia << ".\n";
            SQLFreeHandle(SQL_HANDLE_STMT, handler_contrato);
            return resultado;
        }

        SQLGetData(handler_contrato, 1, SQL_C_LONG, &id_contrato_i, 0, NULL);
        SQLFreeHandle(SQL_HANDLE_STMT, handler_contrato);

        // Insertar en la tabla Soluciona
        SQLHSTMT handler_soluciona = SQL_NULL_HSTMT;
        if(SQLAllocHandle(SQL_HANDLE_STMT, con, &handler_soluciona) != SQL_SUCCESS) {
            return resultado;
        }
        char soluciona[2048];
        sprintf(soluciona, "INSERT INTO Soluciona (DNI, ID_Incidencia, ID_Contrato_I) "
                           "VALUES ('%s', %d, %d);",
                           dni_empleado_esc.c_str(), id_incidencia, id_contrato_i);
        SQLRETURN ret_soluciona = SQLExecDirectA(handler_soluciona, (SQLCHAR*)soluciona, SQL_NTS);
        if (ret_soluciona != SQL_SUCCESS && ret_soluciona != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para insertar en Soluciona.\n";
            mostrarErrorE(handler_soluciona);
            SQLFreeHandle(SQL_HANDLE_STMT, handler_soluciona);
            return resultado;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, handler_soluciona);
        resultado.first = dni_empleado;
        resultado.second = estado_incidencia;;
        return resultado;
    }

