#include "../include/GestionEmpleados.h"
#include "../include/ConexionADB.h"
#include <sqltypes.h>
#include <iostream>
#include <algorithm>

// Función para escapar comillas simples en cadenas SQL
std::string escapeSQL(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\'') result += "''"; // duplicamos la comilla simple
        else result += c;
    }
    return result;
}

// Función para mostrar errores SQL
void mostrarError(SQLHSTMT stmt) {
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

        SQLHDBC con = conexion.getConnection();
        SQLHSTMT handler = SQL_NULL_HSTMT;

        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return false;
        }

        // Damos de alta el empleado
        char empleado[2048];
        std::string dni_empleado_esc = escapeSQL(dni_empleado);
        std::string nombre_esc = escapeSQL(nombre);
        std::string apellidos_esc = escapeSQL(apellidos);
        std::string telefono_esc = escapeSQL(telefono);
        std::string correo_electronico_esc = escapeSQL(correo_electronico);
        std::string puesto_esc = escapeSQL(puesto);
        sprintf(empleado, "INSERT INTO Empleado (DNI_Empleado, Nombre, Apellidos, Telefono, Correo_Electronico, Puesto) "
                          "VALUES ('%s', '%s', '%s', '%s', '%s', '%s');",
                                    dni_empleado_esc.c_str(), nombre_esc.c_str(), apellidos_esc.c_str(),      
                                    telefono_esc.c_str(), correo_electronico_esc.c_str(), puesto_esc.c_str());
       
                                    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)empleado, SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para contratar al empleado.\n";
            mostrarError(handler);
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
        std::string dni_empleado_esc = escapeSQL(dni_empleado);
        sprintf(empleado, "DELETE FROM Empleado WHERE DNI_Empleado = '%s';", dni_empleado_esc.c_str());
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)empleado, SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para despedir al empleado.\n";
            mostrarError(handler);
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
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)"SELECT DNI_Empleado, Nombre, Apellidos, Telefono, Correo_Electronico, Puesto FROM Empleado;", SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para mostrar los empleados.\n";
            mostrarError(handler);
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
        // Procesamos los resultados
        while (SQLFetch(handler) == SQL_SUCCESS) {
            SQLGetData(handler, 1, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
            SQLGetData(handler, 2, SQL_C_CHAR, nombre_buf, sizeof(nombre_buf), NULL);
            SQLGetData(handler, 3, SQL_C_CHAR, apellidos_buf, sizeof(apellidos_buf), NULL);
            SQLGetData(handler, 4, SQL_C_CHAR, telefono_buf, sizeof(telefono_buf), NULL);
            SQLGetData(handler, 5, SQL_C_CHAR, correo_buf, sizeof(correo_buf), NULL);
            SQLGetData(handler, 6, SQL_C_CHAR, puesto_buf, sizeof(puesto_buf), NULL);
            SQLGetData(handler, 7, SQL_C_CHAR, ventas_buf, sizeof(ventas_buf), NULL);
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
        std::string dni_empleado_esc = escapeSQL(dni_empleado);
        std::string consulta = "SELECT DNI_Empleado, Nombre, Apellidos, Telefono, Correo_Electronico, Puesto FROM Empleado WHERE DNI_Empleado = '" + dni_empleado_esc + "';";
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)consulta.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para modificar el empleado.\n";
            mostrarError(handler);
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
        

        while(n != 7){
            std::cout << "Ingrese el número correspondiente al campo que desea modificar:\n";
            std::cout << "1. Nombre\n";
            std::cout << "2. Apellidos\n";
            std::cout << "3. Teléfono\n";
            std::cout << "4. Correo Electrónico\n";
            std::cout << "5. Puesto\n";
            std::cout << "6.Ventas\n";
            std::cout << "7. Salir\n";
            std::cin >> n;
            std::cin.ignore(); // Limpiar el buffer de entrada
            std::string campo;
            switch(n){
                case 1:campo = "Nombre"; break;
                case 2:campo = "Apellidos"; break;
                case 3:campo = "Telefono"; break;
                case 4:campo = "Correo_Electronico"; break;
                case 5:campo = "Puesto"; break;
                case 6:campo = "Ventas"; break;
                case 7:{
                    std::cout << "Saliendo de la modificación de empleado.\n";
                    break;
                }
                default:
                   std::cout << "Opción no válida. Intente de nuevo.\n";
                }
            std::string nuevo;
            std::cout << "Ingrese el nuevo " << campo << ":";
            std::getline(std::cin, nuevo);
            std::string actualizar = "UPDATE Empleado SET " + campo + " = '" + escapeSQL(nuevo) + "' WHERE DNI_Empleado = '" + dni_empleado_esc + "';";
               
            SQLRETURN retActualizar = SQLExecDirectA(handler, (SQLCHAR*)actualizar.c_str(), SQL_NTS);
            if (retActualizar != SQL_SUCCESS && retActualizar != SQL_SUCCESS_WITH_INFO) {
                std::cout << "Error al ejecutar la sentencia SQL para actualizar el empleado.\n";
                mostrarError(handler);
                SQLFreeHandle(SQL_HANDLE_STMT, handler);
                return datos_modificados;
            } else {
                datos_modificados += "Campo " + campo + " actualizado a " + nuevo + ".\n";
            }
        }
        

        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return datos_modificados;
    }

    /* ========================= RF-4.5 ========================= */

    std::pair<std::vector<EmpleadoInfo>, int> GestionEmpleados::incentivoParaEmpleados (){
        std::pair<std::vector<EmpleadoInfo>, int> incentivos;
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
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)"SELECT DNI_Empleado, Nombre, Apellidos, Telefono, Correo_Electronico, Puesto, Ventas FROM Empleado WHERE Ventas > 30;", SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para mostrar los empleados con incentivos.\n";
            mostrarError(handler);
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
        char ventas_buf[10];
        // Procesamos los resultados
        while (SQLFetch(handler) == SQL_SUCCESS) {
            SQLGetData(handler, 1, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
            SQLGetData(handler, 2, SQL_C_CHAR, nombre_buf, sizeof(nombre_buf), NULL);
            SQLGetData(handler, 3, SQL_C_CHAR, apellidos_buf, sizeof(apellidos_buf), NULL);
            SQLGetData(handler, 4, SQL_C_CHAR, telefono_buf, sizeof(telefono_buf), NULL);
            SQLGetData(handler, 5, SQL_C_CHAR, correo_buf, sizeof(correo_buf), NULL);
            SQLGetData(handler, 6, SQL_C_CHAR, puesto_buf, sizeof(puesto_buf), NULL);
            SQLGetData(handler, 7, SQL_C_CHAR, ventas_buf, sizeof(ventas_buf), NULL);
            if(std::stoi(ventas_buf) > 30){
                EmpleadoInfo empleado;
                empleado.dni_empleado = std::string(dni_buf);
                empleado.nombre = std::string(nombre_buf);
                empleado.apellidos = std::string(apellidos_buf);
                empleado.telefono = std::string(telefono_buf);
                empleado.correo_electronico = std::string(correo_buf);
                empleado.puesto = std::string(puesto_buf);
                empleado.ventas = std::stoi(ventas_buf);
                incentivos.first.push_back(empleado);
                int incentivo = (std::stoi(ventas_buf) - 30) * 100; // Ejemplo: 100 unidades monetarias por cada venta extra sobre 30
                incentivos.second += incentivo;
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
        SQLHSTMT handler = SQL_NULL_HSTMT;
        
        // Inicializamos el handler para ejecutar la sentencia SQL
        if (SQLAllocHandle(SQL_HANDLE_STMT, con, &handler) != SQL_SUCCESS) {
            return resultado;
        }
        // Ejecutamos la consulta para obtener todos los empleados
        SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*)"SELECT DNI_Empleado, Nombre, Apellidos, Telefono, Correo_Electronico, Puesto FROM Empleado;", SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para mostrar los empleados.\n";
            mostrarError(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return resultado;
        }
        // Buffers para obtener los datos
        char dni_buf[10];
        // Procesamos los resultados
        while (SQLFetch(handler) == SQL_SUCCESS) {
            SQLGetData(handler, 1, SQL_C_CHAR, dni_buf, sizeof(dni_buf), NULL);
            if(dni_empleado == std::string(dni_buf)){
                resultado.first = dni_empleado;
                break;
            }
        }
        // Actualizamos el estado de la incidencia
        std::string estado_esc = escapeSQL(estado_incidencia);
        char actualizar[2048];
        sprintf(actualizar, "UPDATE Incidencia SET Estado_Incidencia = '%s' WHERE ID_Incidencia = %d;", estado_esc.c_str(), id_incidencia);
        SQLRETURN retActualizar = SQLExecDirectA(handler, (SQLCHAR*)actualizar, SQL_NTS);
        if (retActualizar != SQL_SUCCESS && retActualizar != SQL_SUCCESS_WITH_INFO) {
            std::cout << "Error al ejecutar la sentencia SQL para actualizar la incidencia.\n";
            mostrarError(handler);
            SQLFreeHandle(SQL_HANDLE_STMT, handler);
            return resultado;
        }
        resultado.second = estado_incidencia;
        // Liberamos recursos
        SQLFreeHandle(SQL_HANDLE_STMT, handler);
        return resultado;

    }
