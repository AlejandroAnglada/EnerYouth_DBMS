#include "../include/funcionalidades_main.h"

int main(){
    std::string user, pwd, dsnTemp;
    int opcion;

    // Declaramos el entorno y la conexión:
    SQLHSTMT handler;
    ConexionADB conexion;
    // Pedimos las credenciales al usuario para conectarse a la db:
    std::string dsn = "practbd"; // Nombre del DSN, puesto por defecto tal y como en la explicación del S1
    std::cout << "Esperando credenciales...\n";
    std::cout << "DSN (por defecto 'practbd'; dejar en blanco si quiere usar esa): ";
    std::cin >> dsnTemp;
    if(!dsnTemp.empty()) dsn = dsnTemp;
    std::cout << "Usuario: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pwd;

    // Creamos la conexión
    if (!conexion.connect(dsn, user, pwd)) {
        std::cerr << "Error al conectar con la BD\n";
        return 1;
    }

    // Handler y conexión
    SQLHDBC con = conexion.getConnection();
    handler = SQL_NULL_HSTMT;
    SQLAllocHandle(SQL_HANDLE_STMT, con, &handler);

    // Creamos instancias de cada subsistema
    GestionTransmisionDistribucion hogares(conexion);
    GestionEmpleados empleados(conexion);
    GestionRecursosEnergeticos rec_ener(conexion);
    GestionClientes clientes(conexion);

    // Borramos todas las tablas de la sesion anterior
    borrarTablas(conexion, handler);

    // Creamos las tablas (prueba)
    crearTablas(conexion, handler);

    // Creamos el trigger para actualizar el tipo de contrato en Hogar al modificarlo en Contrato
    crearTriggerActualizarDatosHogar(conexion, handler);
    // Creamos el trigger para calcular incentivos al actualizar las ventas de un empleado
    crearTriggerVentas(conexion, handler);
    // Creamos el trigger para bloquear cesión de potencia si el cedente está en números rojos.
    crearTriggerBloquearCesion(conexion, handler);
    // Creamos trigger para finalizar contratos al dar de baja un cliente
    crearTriggerContratosBaja(conexion, handler);

    // Inserto un cliente y contrato para poder probar el subsitema de Transmisión/Distribución
    char dni[10] = "12345678A";
    char nombre[21] = "Juan";
    char apellidos[81] = "Perez Gomez";
    char direccion[101] = "Calle Falsa 123";
    char telefono[10] = "600123468";
    char email[101] = "juan@gmail.com";
    int id_cliente = 1;
    char fecha_registro[11] = "2023-10-01";
    char fecha_baja[11] = "";
    char motivo_baja[256] = "";
    char cliente[2048];
    sprintf(cliente, "INSERT INTO Cliente (DNI_CIF, Nombre, Apellidos, Direccion, Telefono, Email, ID_Cliente, Fecha_Registro, Fecha_Baja, Motivo_Baja) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d, TO_DATE('%s', 'YYYY-MM-DD'), TO_DATE('%s', 'YYYY-MM-DD'), '%s');", dni, nombre, apellidos, direccion, telefono, email, id_cliente, fecha_registro, fecha_baja, motivo_baja);
    
    SQLRETURN ret = SQLExecDirectA(handler, (SQLCHAR*) cliente, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error insertando Cliente\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);
    SQLEndTran(SQL_HANDLE_DBC, conexion.getConnection(), SQL_COMMIT);

    int id_contrato = 1;
    char cups[23] = "ES12345678901256890123";
    char tipo_contrato[21] = "Residencial";
    double potencia_con = 5.5;
    char tarifa[20] = "FIja";
    char iban[35] = "ES7620770024003102575766";
    char dni_cif[10] = "12345678A";
    char fecha_inicio[11] = "2023-10-01";

    char contrato[2048];
    sprintf(contrato, "INSERT INTO Contrato (ID_Contrato, CUPS, Tipo_Contrato, Potencia_Con, Tarifa, IBAN, Fecha_Inicio, Fecha_Fin, Estado, DNI_CIF) VALUES (%d, '%s', '%s', %f, '%s', '%s', TO_DATE('%s', 'YYYY-MM-DD'), NULL, 'Activo', '%s');", id_contrato, cups, tipo_contrato, potencia_con, tarifa, iban, fecha_inicio, dni_cif);
    SQLRETURN retContrato = SQLExecDirectA(handler, (SQLCHAR*) contrato, SQL_NTS);
    if (retContrato != SQL_SUCCESS && retContrato != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error insertando Contrato\n";
    }
    SQLFreeStmt(handler, SQL_CLOSE);
    SQLEndTran(SQL_HANDLE_DBC, conexion.getConnection(), SQL_COMMIT);

    // Insertamos instalaciones de prueba
    insertarInstalaciones(conexion, handler);

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
            
            case 2: gestionRecursosEnergeticos(rec_ener); break;

            case 3: gestionEmpleados(empleados); break;

            case 4: gestionClientes(clientes); break;

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