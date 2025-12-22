#ifndef GESTION_TRANSMISION_DISTRIBUCION_H
#define GESTION_TRANSMISION_DISTRIBUCION_H

#include <string>
#include <vector>

// Forward declaration para evitar dependencias innecesarias en la cabecera.
class ConexionADB;

// Estructura para almacenar información de un hogar.
struct HogarInfo {
    std::string dni_cliente;
    std::string tipo_contrato;
    std::string direccion;
};

// Estructura para almacenar datos de un contrato.
struct DatosContrato {
    std::string cups;
    std::string tipo_contrato;
    double potencia_contratada;
    std::string tarifa;
    std::string iban;
    std::string fecha_inicio;
    std::string fecha_fin;
};

/**
 * @class GestionTransmisionDistribucion
 * @brief Clase encargada de implementar el subsistema de gestión de transmisión y distribución.
 *
 * Esta clase proporciona la funcionalidad necesaria para dar de alta, baja y modificar un
 * hogar, mostrar el listado de hogares registrados pudiendo filtrarse por zona geográfica y 
 * registrar las incidencias relacionadas con el suministro eléctrico de un hogar.
 */
class GestionTransmisionDistribucion {
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
     * @param con Referencia a una conexión a base de datos ya inicializada.
     */
    explicit GestionTransmisionDistribucion(ConexionADB& con);

    /* ========================= RF-1.1 ========================= */
    /**
     * @brief Da de alta un nuevo hogar en el sistema.
     * @param direccion Dirección del hogar.
     * @param dni_cliente DNI del cliente asociado al hogar.
     * @param id_contrato ID del contrato asociado al hogar.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool altaHogar(const std::string& direccion, const std::string& dni_cliente, int id_contrato);

    /* ========================= RF-1.2 ========================= */
    /**
     * @brief Da de baja un hogar del sistema.
     * @param direccion Dirección del hogar a dar de baja.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool bajaHogar(const std::string& direccion);

    /* ========================= RF-1.3 ========================= */
    /**
     * @brief Muestra el listado de hogares registrados, pudiendo filtrarse por zona geográfica, DNI del cliente o tipo de contrato.
     * @param zona_geografica Zona geográfica para filtrar los hogares. Si está vacío, se muestran todos.
     * @param dni_cliente DNI del cliente para filtrar los hogares. Si está vacío, se muestran todos.
     * @param tipo_contrato Tipo de contrato para filtrar los hogares. Si está vacío, se muestran todos.
     * @return Vector de strings con las direcciones de los hogares encontrados.
     */
    std::vector<HogarInfo> listarHogares(const std::string& zona_geografica = "",
                                        const std::string& dni_cliente = "",
                                        const std::string& tipo_contrato = "");

    /* ========================= RF-1.4 ========================= */
    /**
     * @brief Modifica los datos de un hogar existente.
     * @param direccion Dirección del hogar a modificar.
     * @param dni_cliente DNI del cliente asociado al hogar.
     * @param cups Nuevo CUPS del contrato asociado al hogar.
     * @param tipo_contrato Nuevo tipo de contrato del hogar.
     * @param tarifa Nueva tarifa del contrato asociado al hogar.
     * @param iban Nuevo IBAN del contrato asociado al hogar.
     * @param fecha_inicio Nueva fecha de inicio del contrato asociado al hogar.
     * @param fecha_fin Nueva fecha de fin del contrato asociado al hogar.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool modificarHogar(const std::string& direccion, 
                        const std::string& dni_cliente,
                        const DatosContrato& datos_contrato);

    /* ========================= RF-1.5 ========================= */
    /**
     * @brief Registra una incidencia relacionada con el suministro eléctrico de un hogar.
     * @param id_incidencia ID de la incidencia.
     * @param tipo_incidencia Tipo de incidencia.
     * @param descripcion Descripción de la incidencia.
     * @param id_contrato ID del contrato asociado al hogar.
     * @param fecha_incidencia Fecha de la incidencia.
     * @param fecha_resolucion Fecha de resolución de la incidencia.
     * @param estado_incidencia Estado de la incidencia.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool registrarIncidencia(int id_incidencia, const std::string& tipo_incidencia,
                            const std::string& descripcion, int id_contrato,
                            const std::string& fecha_incidencia, const std::string& fecha_resolucion,
                            const std::string& estado_incidencia);
};

#endif // GESTION_TRANSMISION_DISTRIBUCION_H