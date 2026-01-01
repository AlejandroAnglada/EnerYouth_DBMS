#ifndef GESTION_CLIENTES_H
#define GESTION_CLIENTES_H

#include <string>
#include <vector>

// Forward declaration para evitar dependencias innecesarias en la cabecera.
class ConexionADB;

// Estructura para almacenar información de un cliente.
struct ClienteInfo {
    std::string dni_cif;
    std::string nombre;
    std::string apellidos;
    std::string direccion;
    std::string telefono;
    std::string email;
    std::string estado;
    int id_cliente;
    std::string fecha_registro;
    std::string fecha_baja;
    std::string motivo_baja;
};

// Estructura para almacenar información de un contrato.
struct ContratoInfo {
    int id_contrato;
    std::string cups;
    std::string tipo_contrato;
    double potencia_con;
    std::string tarifa;
    std::string iban;
    std::string fecha_inicio;
    std::string fecha_fin;
    std::string estado;
    std::string dni_cif;
};

/**
 * @class GestionClientes
 * @brief Clase encargada de implementar el subsistema de gestión de clientes.
 *
 * Esta clase proporciona la funcionalidad necesaria para dar de alta, baja, consultar y modificar
 * clientes, así como para crear y finalizar los contratos asociados a dichos clientes.
 */
class GestionClientes {
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
    explicit GestionClientes(ConexionADB& con);

    /* ========================= RF-3.1 ========================= */
    /**
     * @brief Da de alta un nuevo cliente en el sistema.
     * @param dni_cif DNI o CIF del cliente.
     * @param nombre Nombre del cliente.
     * @param apellidos Apellidos del cliente.
     * @param direccion Dirección del cliente.
     * @param telefono Teléfono del cliente.
     * @param email Correo electrónico del cliente.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool altaCliente(const std::string& dni_cif,
                     const std::string& nombre,
                     const std::string& apellidos,
                     const std::string& direccion,
                     const std::string& telefono,
                     const std::string& email);

    /* ========================= RF-3.2 ========================= */
    /**
     * @brief Consulta la ficha de un cliente existente en el sistema dado su DNI/CIF.
     * @param dni_cif DNI o CIF del cliente.
     * @param info Estructura ClienteInfo donde se almacenarán los datos del cliente.
     * @return true si la consulta fue exitosa; false en caso contrario.
     */
    bool consultarCliente(const std::string& dni_cif,
                         ClienteInfo& info);
    
    /* ========================= RF-3.3 ========================= */
    /**
     * @brief Actualiza los datos de un cliente existente en el sistema.
     * @param dni_cif DNI o CIF del cliente.
     * @param campo Campo a modificar ("nombre", "apellidos", "direccion", "telefono", "email").
     * @param valor_nuevo Nuevo valor para el campo especificado.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     */
    bool actualizarCliente(const std::string& dni_cif,
                         const std::string& campo,
                         const std::string& valor_nuevo);

    /* ========================= RF-3.4 ========================= */
    /**
     * @brief Crea un nuevo contrato asociado a un cliente existente en el sistema.
     * @param dni_cif DNI o CIF del cliente.
     * @param cups CUPS del contrato.
     * @param tipo_contrato Tipo de contrato ("Doméstico", "Empresarial", "Industrial").
     * @param potencia_con Potencia contratada (debe ser > 3).
     * @param tarifa Tarifa del contrato.
     * @param iban IBAN para domiciliación de los pagos del contrato.
     * @param fecha_inicio Fecha de inicio del contrato.
     * @param fecha_fin Fecha de fin del contrato (opcional).
     * @return true si la operación se realizó con éxito; false en caso contrario.
     * NOTA: El contrato se crea con estado "Activo" y fecha de inicio actual.
     */
    bool crearContrato(const std::string& dni_cif,
                       const std::string& cups,
                       const std::string& tipo_contrato,
                       double potencia_con,
                       const std::string& tarifa,
                       const std::string& iban,
                       const std::string& fecha_inicio,
                       const std::string& fecha_fin = "");

    /**
     * @brief Finaliza un contrato existente en el sistema.
     * @param id_contrato ID del contrato.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     * NOTA: El contrato se finaliza cambiando su estado a "Finalizado" y estableciendo la fecha de fin a la fecha actual.
     */
    bool finalizarContrato(int id_contrato);

    /* ========================= RF-3.5 ========================= */
    /**
     * @brief Da de baja un cliente existente en el sistema.
     * @param dni_cif DNI o CIF del cliente.
     * @param fecha_baja Fecha de baja del cliente.
     * @param motivo_baja Motivo de la baja del cliente.
     * @return true si la operación se realizó con éxito; false en caso contrario.
     * NOTA: Antes de dar de baja al cliente, se debe verificar que no tiene contratos activos.
     */
    bool bajaCliente(const std::string& dni_cif,
                     const std::string& fecha_baja,
                     const std::string& motivo_baja);

private:
    /* ================ MÉTODOS PRIVADOS PARA VALIDACIONES INTERNAS ================ */

    /**
     * @brief Valida el formato del DNI o CIF.
     * @param dni_cif DNI o CIF a validar.
     * @return true si el formato es válido; false en caso contrario.
     */
    bool validarFormatoDNI(const std::string& dni_cif) const;

    /**
     * @brief Valida el formato del correo electrónico.
     * @param email Correo electrónico a validar.
     * @return true si el formato es válido; false en caso contrario.
     */
    bool validarFormatoEmail(const std::string& email) const;

    /**
     * @brief Valida el formato del teléfono.
     * @param telefono Teléfono a validar.
     * @return true si el formato es válido; false en caso contrario.
     */
    bool validarFormatoTelefono(const std::string& telefono) const;

    /**
     * @brief Valida el formato del IBAN.
     * @param iban IBAN a validar.
     * @return true si el formato es válido; false en caso contrario.
     */
    bool validarFormatoIBAN(const std::string& iban) const;

    /**
     * @brief Valida que la potencia contratada sea mayor que 3.
     * @param potencia Potencia contratada a validar.
     * @return true si la potencia es válida; false en caso contrario.
     */
    bool validarPotencia(double potencia) const;

    /**
     * @brief Verifica si existe una tarifa en la base de datos.
     * @param tarifa Tarifa a verificar.
     * @return true si la tarifa existe; false en caso contrario.
     */
    bool existeTarifa(const std::string& tarifa) const;

    /**
     * @brief Verifica si existe un cliente en la base de datos.
     * @param dni_cif DNI o CIF del cliente a verificar.
     * @return true si el cliente existe; false en caso contrario.
     */
    bool existeCliente(const std::string& dni_cif) const;

    /**
     * @brief Verifica si existe un contrato activo con un determinado CUPS.
     * @param cups CUPS del contrato a verificar.
     * @return true si el contrato existe; false en caso contrario.
     */
    bool existeContratoActivoPorCUPS(const std::string& cups) const;

    /**
     * @brief Genera el siguiente ID_Cliente disponible.
     * @return Siguiente ID_Cliente disponible.
     */
    int generarSiguienteIDCliente() const;

    /**
     * @brief Genera el siguiente ID_Contrato disponible.
     * @return Siguiente ID_Contrato disponible.
     */
    int generarSiguienteIDContrato() const;
};

#endif // GESTION_CLIENTES_H