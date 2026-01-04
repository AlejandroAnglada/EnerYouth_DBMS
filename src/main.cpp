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