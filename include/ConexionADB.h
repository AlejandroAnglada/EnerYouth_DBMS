// Para poder compilar tanto en Windows como en Linux
#ifdef _WIN32
  #include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>
#include <string>

//Compilación usando el param -Iinclude porfa

/**
 * @author Alejandro Anglada Álvarez
 * @date 2025-12-16
 * @class ConexionADB
 * @file ConexionADB.h
 * @brief Clase de conexión a bases de datos. Se usa pasándola como referencia a los métodos de las clases de cada subsistema (véase su documentación), así
 * solo hay que iniciar sesión una sola vez por ejecución, y no iniciarla y cerrarla en cada método.
 */
class ConexionADB {
private:
    /**
     * @brief Entorno de la base de datos; dato de tipo SQLHENV.
     */
    SQLHENV env;

    /**
     * @brief La conexión a la base de datos; dato de tipo SQLHDBC
     */
    SQLHDBC dbc;

    /**
     * @brief Booleano que indica si se ha establecido correctamente o no la conexión.
     */
    bool conexion_establecida;

public:
    /**
     * @brief Constructor por defecto. Inicializa todos los atributos a null, para iniciar la conexión.
     */
    ConexionADB();

    /**
     * @brief Método no-const. Inicia la conexión con la base de datos en función de los valores de los parámetros (véanse adelante).
     * @pre Los parámetros deben representar un servidor y usuario válidos. De lo contrario no se establece conexión.
     * @param dsn Nombre del servidor al que se tratará de conectar.
     * @param user Nombre del usuario. Debe estar autorizado en el servidor.
     * @param pwd Contraseña asociada al parámetro user.
     * @post La conexión habrá sido establecida; el valor de conexion_establecida se actualizará de forma acorde.
     * @return true si se ha logrado conectar correctamente; false en otro caso.
     */
    bool connect(const std::string& dsn,
                 const std::string& user,
                 const std::string& pwd);

    /**
     * @brief Método const. Devuelve la conexión a la base de datos para hacer las ejecuciones, dentro de los métodos de cada subsistema.
     * @return Tipo de dato SQLHDBC. Handler de la conexión a la base de datos.
     */
    SQLHDBC getConnection() const;

    /**
     * @brief Método const. Devuelve si la conexión se ha establecido o no de forma exitosa.
     * @return true si la conexión está bien establecida; false en otro caso.
     */
    bool isConnected() const;
    
    /**
     * @brief Método no-const. Se desconecta de la base de datos de forma segura. Tiene el mismo efecto que el destructor.
     */
    void disconnect();
    
    /**
     * @brief Destructor. Interrumpe de forma segura la conexión a la base de datos. Tiene el mismo efecto que disconnect().
     */
    ~ConexionADB();
};

