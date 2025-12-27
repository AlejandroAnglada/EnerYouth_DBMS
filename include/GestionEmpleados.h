#ifndef GestionEmpleados_H
#define GestionEmpleados_H

#include <string>
#include <vector>

// Forward declaration para evitar dependencias innecesarias en la cabecera.
class ConexionADB;

// Estructura para almacenar información de un empleado.
struct EmpleadoInfo {
    std::string dni_empleado;
    std::string nombre;
    std::string apellidos;
    std::string telefono;
    std::string correo_electronico;
    std::string puesto;
    int ventas;
    int incentivo;
};

/**
 * @class GestionEmpleados
 * @brief Clase encargada de implementar el subsistema de gestión de empleados.
 *
 * Esta clase proporciona la funcionalidad necesaria para dar de alta, baja y consulta
 * de empleados.
 */
class GestionEmpleados {
private:
    /**
     * @brief Referencia a la conexión con la base de datos.
     *
     * La conexión se recibe ya establecida desde el exterior (ConexionADB),
     * evitando que esta clase gestione directamente la apertura o cierre de la misma.
     */
    ConexionADB& conexion;
public:
    /**
     * @brief Constructor.
     * @param con Referencia a una conexión a base de datos ya inicializada. Para ver su funcionalidad,
     * véase el fichero "ConexionADB.h". NOTA: Declarado "explicit" para NO poder usar el constructor por
     * defecto. JAMÁS deberá poder existir una instancia de subsistema *sin* una conexión asociada (¿cómo se
     * pretende dar funcionalidad a tu subsistema sin conectarte a la base de datos? Jeje).
     */
    explicit GestionEmpleados(ConexionADB& con);
   
    /* ========================= RF-4.1 ========================= */

    /**
     * @brief Da de alta un nuevo empleado en el sistema.
     * @param dni_empleado DNI del empleado.
     * @param nombre Nombre del empleado.
     * @param apellidos Apellidos del empleado.
     * @param telefono Teléfono del empleado.
     * @param correo_electronico Correo electrónico del empleado.
     * @param puesto Puesto de trabajo del empleado.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool contratarEmpleado(const std::string& dni_empleado, const std::string& nombre, 
                           const std::string& apellidos, const std::string& telefono,
                           const std::string& correo_electronico, const std::string& puesto = "Empleado General");


    /* ========================= RF-4.2 ========================= */
    
    /**
     * @brief Da de baja un empleado existente en el sistema.
     * @param dni_empleado DNI del empleado.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool despedirEmpleado(const std::string& dni_empleado);    

    /* ========================= RF-4.3 ========================= */
    /**
     * @brief Muestra la lista de todos los empleados en el sistema.
     * @return Cadena de texto con la información de todos los empleados.
     */
    std::vector<EmpleadoInfo> mostrarEmpleados();   
    
    /* ========================= RF-4.4 ========================= */
    /**
     * @brief Modifica los datos de un empleado existente en el sistema.
     * @param dni_empleado DNI del empleado.
     * @return Vector de string con la información del empleado modificado.
     */
    std::string modificarEmpleado(const std::string& dni_empleado);

    /* ========================= RF-4.5 ========================= */
    /**
     * @brief Calcula el incentivo para un empleado basado en su puesto.
     * @return vector de string con la infotmación de cada empleado que debe recibir incentivo y el incentivo correspondiente.
     */
    std::vector<EmpleadoInfo> incentivoParaEmpleados();

    /*========================= RF-4.6 =========================*/
    /**
     * @brief Soluciona las incidencias asignadas a un empleado.
     * @param dni_empleado DNI del empleado.
     * @param id_incidencia ID de la incidencia.
     * @param estado_incidencia Nuevo estado de la incidencia.
     * @return Par con el DNI del empleado que ha tratado la incidencia y su nuevo estado.
     */
    std::pair<std::string, std::string> solucionIncidencias(std::string& dni_empleado,int id_incidencia, std::string estado_incidencia);
};
#endif // GestionEmpleados_H