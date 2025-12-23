#ifndef GESTION_RECURSOS_ENERGETICOS_H
#define GESTION_RECURSOS_ENERGETICOS_H

#include <string>
#include <vector>

// Forward declaration para evitar dependencias innecesarias en la cabecera.
class ConexionADB;

/**
 * @class GestionRecursosEnergeticos
 * @brief Clase encargada de implementar el subsistema de gestión de recursos energéticos.
 *
 * Esta clase proporciona la funcionalidad necesaria para dar de alta, baja y consulta
 * de fuentes e instalaciones energéticas, así como operaciones más complejas como la
 * cesión de potencia entre instalaciones, siguiendo los requisitos especificados en
 * el diseño del sistema.
 */
class GestionRecursosEnergeticos {
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
     * véase el fichero "ConexionADB.h".
     */
    explicit GestionRecursosEnergeticos(ConexionADB& con);

    /* ========================= RF-2.1 ========================= */

    /**
     * @brief Da de alta una nueva fuente de energía en el sistema.
     * @param nombre Nombre de la fuente de energía.
     * @param descripcion Descripción de la fuente.
     * @param direccion Dirección de las instalaciones asociadas.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool altaFuenteEnergetica(const std::string& nombre,
                              const std::string& descripcion,
                              const std::string& direccion,
                              const std::string& fechaYYYYMMDD);

    /**
     * @brief Da de baja una fuente de energía existente en el sistema.
     * @param nombre Nombre de la fuente de energía.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool bajaFuenteEnergetica(const std::string& nombre);

    /* ========================= RF-2.2 ========================= */

    /**
     * @brief Consulta los ingresos netos anuales de una instalación concreta.
     * @param direccion Dirección de la instalación.
     * @param ingresos Valor de salida con los ingresos netos.
     * @return true si la consulta fue exitosa; false en caso contrario.
     */
    bool consultarIngresosPorInstalacion(const std::string& direccion,
                                         int& ingresos) const;

    /* ========================= RF-2.3 ========================= */

    /**
     * @brief Consulta las instalaciones que generan un determinado tipo de energía.
     * @param tipoEnergia Tipo de energía.
     * @param instalaciones Vector de direcciones resultantes.
     * @return true si la consulta fue exitosa; false en caso contrario.
     */
    bool consultarInstalacionesPorEnergia(const std::string& tipoEnergia,
                                          std::vector<std::string>& instalaciones) const;

    /* ========================= RF-2.4 ========================= */

    /**
     * @brief Consulta los ingresos netos anuales asociados a un tipo de energía.
     * @param tipoEnergia Tipo de energía.
     * @param ingresos Valor de salida con los ingresos netos.
     * @return true si la consulta fue exitosa; false en caso contrario.
     */
    bool consultarIngresosPorTipoEnergia(const std::string& tipoEnergia,
                                         int& ingresos) const;

    /* ========================= RF-2.5 ========================= */

    /**
     * @brief Cede potencia de una instalación a otra, cumpliendo las restricciones semánticas.
     * @param instalacionCedente Dirección de la instalación que cede potencia.
     * @param instalacionReceptora Dirección de la instalación que recibe potencia.
     * @param potencia Potencia a ceder (en MWh).
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool cederPotencia(const std::string& instalacionCedente,
                       const std::string& instalacionReceptora,
                       int potencia);

    /* ========================= RF-2.6 ========================= */

    /**
     * @brief Da de alta una nueva instalación energética en el sistema.
     * @param direccion Dirección de la instalación.
     * @param tipoEnergia Tipo de energía generada.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool altaInstalacionEnergetica(const std::string& direccion,
                                   const std::string& tipoEnergia);
};

#endif // GESTION_RECURSOS_ENERGETICOS_H
