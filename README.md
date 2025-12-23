# EnerYouth_DBMS
Implementación final del sistema de información "EnerYouth" de la asignatura DDSI, de la carrera de "Ingeniería Informática" cursada durante el curso 2025-2026 en Granada.

## Descripción
A lo largo del curso, hemos ido diseñando diferentes partes de un sistema de información a libre elección. En nuestro caso, decidimos hacerlo sobre una empresa energética llamada "EnerYouth". Hasta este instante, hemos definido en lenguaje natural los requisitos de cada subsistema de información (enumerados más adelante), sus requisitos funcionales formales, sus diagramas de flujo, sus esquemas relacionales y sus diagramas entidad/relación, además de lo respectivo implementado para la totalidad del sistema. 
> [!WARNING]
> Este proyecto ha sido desarrollado **exclusivamente con fines académicos** en el contexto de la asignatura DDSI.
>  
> **No debe utilizarse como base para un sistema de información real**, ya que no cumple requisitos de
> seguridad, auditoría, concurrencia, escalabilidad ni validación usualmente propios de entornos de producción.

Adelante están las descripciones de cada subsistema en lenguaje natural, disponibles para el lector:

### Subsistema de gestión y transmisión/distribución de energía
El subsistema de gestión y transmisión/distribución registrará los hogares que tienen un contrato con la empresa así como las incidencias que puedan ocurrir, por lo que será imprescindible que, de cada hogar, se almacene su dirección (en una cadena de 100 caracteres).

Una de las funcionalidades del sistema será la capacidad de dar de alta un hogar al que suministrarle energía. Para ello se deberá proporcionar la dirección del mismo (una cadena de 100 caracteres), la información del cliente que contrata el servicio al que se le asociará el contrato (DNI, una cadena de 9 caracteres, puesto que el cliente ya está registrado en el sistema gracias al subsistema de gestión de clientes) y el identificador del contrato de dicho cliente, pues un cliente puede tener contratados varios hogares (ID_Contrato, entero de 9 cifras). El sistema confirmará si la inserción se realizó con éxito o devolverá un error en caso de que la dirección ya exista en la base de datos.

Asimismo, para dar de baja un hogar que quiera prescindir de los servicios de la empresa se deberá proporcionar su dirección (cadena de 100 caracteres) que le identifica y se suspenderá el contrato asociado, informando del éxito de la operación o mostrando un error en caso de que la dirección no esté registrada.

El subsistema también permitirá mostrar un listado de hogares registrados en la empresa junto a todos sus datos, pudiendo filtrarse por zonas geográficas, aportando la ciudad o país (cadena de 100 caracteres), clientes (en caso de que un mismo cliente tenga más de un hogar con contrato en la empresa, aportando el DNI (cadena de 9 caracteres)) o tipo de contrato (empresarial, doméstico, etc, en una cadena de 20 caracteres).

Para modificar el contrato del hogar el sistema debe permitir modificar las condiciones del contrato asociado a ese hogar, por tanto, se deberá proporcionar la dirección del hogar (cadena de 100 caracteres) y el usuario responsable del contrato (DNI, cadena de 9 caracteres).

Por último, para gestionar las incidencias el sistema deberá permitir registrar aquellas incidencias (identificador de la incidencia, número entero de 9 dígitos, tipo de incidencia, cadena de 100 caracteres, descripción de la incidencia, cadena de 200 caracteres, estado de la incidencia, cadena de 50 caracteres, contrato asociado, número entero de 9 dígitos y fechas en las que se desarrolla, tipo date) relacionadas con el suministro energético de un hogar (cortes programados, averías, mantenimientos, etc), que quedarán vinculadas al contrato y podrán consultarse posteriormente por el administrador y el cliente, pudiendo ver, no solo los datos de la incidencia, sino también la fecha en la que se produjo y la fecha en la que se resolvió.

### Subsistema de gestión de recursos energéticos
El subsistema de gestión de recursos deberá ser capaz de clasificar, ordenar y acceder a toda la información relacionada con la gestión de recursos energéticos de EnerYouth. 

Para cada instancia de fuente de energía se deberán gestionar (esto es, dar de alta y de baja cuando sea conveniente) sus datos que consisten en una cadena de caracteres con longitud máxima de 60, que corresponde al nombre de la fuente; una cadena de caracteres de longitud máxima de 180, que corresponde a la descripción breve de la fuente de energía; y una cadena de caracteres con longitud máxima de 180, que corresponde a la dirección de las instalaciones en las que se genera el tipo de energía instanciada.

Además, también deberá poder consultar qué ingresos netos anuales (la diferencia entre el beneficio y los gastos) genera unas instalaciones de generación de energía. Para ello, se deberá proporcionar como dato de entrada una dirección correspondiente a la ubicación física de las instalaciones, representada por una cadena de caracteres de máximo 180, y se retornará un valor entero.

El subsistema también podrá proporcionar información sobre las instalaciones que generen determinado tipo de energía, dato de entrada que viene representado por una cadena de caracteres de máximo 60 caracteres de longitud, y le retornará al usuario una serie de instalaciones, representadas por sus direcciones.

Si se desease conocer los ingresos netos anuales de un sólo tipo de energía, se deberá poder hacer con un formato similar a los ingresos netos anuales de unas instalaciones concretas; es decir, proporcionando como dato de entrada el nombre del tipo de energia (máximo 60 caracteres), y retornando un entero que representa dichos ingresos netos.

También deberá poder ser capaz de redirigir la potencia energética que genere una instalación hacia otra, cumpliendo una serie de restricciones, que son las que siguen:

La productividad de la primera instalación debe superar el 65% (medida en megavatios hora) para no causar problemas de falta de alimentación a la primera instalación.
La distancia entre las dos instalaciones no debe ser mayor de 125 kilómetros para no representar un elevado coste en transporte.
La productividad de la segunda instalación debe ser inferior al 35%, punto a partir del cual se considerará que tiene problemas para cumplir con el suministro energético mínimo.

En caso de cumplirse todas estas restricciones, se deberá proporcionar el nombre de la primera instalación, el nombre de la segunda instalación y el porcentaje de potencia que se cederá (ateniéndose de nuevo a las restricciones propuestas).

En caso de que se registrase una nueva instalación, el sistema deberá proporcionar la funcionalidad pertinente para que esto se haga de forma sencilla.


### Subsistema de gestión de clientes
El subsistema de gestión de clientes se encarga de tratar los datos más sensibles de nuestros clientes recogiendo la información necesaria para introducirlos en el sistema de manera que cada uno de ellos quede registrado.

Para el registro de un nuevo cliente se crea una ficha con estado inicial ‘Activo’ mediante una cadena de 8 caracteres, un número entero único que se va incrementando para cada cliente y la fecha de registro con una variable tipo Date con formato DD/MM/AA. Todo esto tras comprobar que los datos y sus formatos están introducidos correctamente. Se solicitan los datos personales, es decir, el DNI/NIF mediante una cadena de 9 caracteres, el nombre y los apellidos en unas cadenas de 20 y 80 caracteres respectivamente y la dirección del domicilio en otra cadena de 100 caracteres, y los datos de contacto como el número de teléfono y el email en cadenas de 9 y 100 caracteres respectivamente. Si todo es correcto, se añade al cliente exitosamente en el sistema mostrando una confirmación de registro, de lo contrario se devuelve un mensaje de error informativo.

El subsistema de gestión de clientes permite la visualización de la ficha con los datos de un cliente identificándolo previamente mediante su DNI. Si el cliente ha sido identificado correctamente se devuelven los datos privados del cliente.

Se permite también la actualización de los datos del cliente con el fin de que si cambia cualquiera de estos, puedan ser modificados para que un cliente no permanezca siempre con su ficha original y esta pueda tener versiones posteriores. De nuevo, se accede identificando al cliente mediante el DNI/NIF e indicando el campo o los campos a modificar y sus nuevos valores, controlando que los formatos de los campos sean válidos y, en resumen, todo lo que se comprobaba para dar de alta a un nuevo cliente.

La gestión de contratos de suministros también está activa en el subsistema de gestión de clientes de modo que se puedan crear los contratos asociados a un cliente, identificado nuevamente por su DNI/NIF en una cadena de 9 caracteres. Para la creación de un nuevo contrato se solicita al cliente cierta información relevante como el Código Universal de Punto de Suministro (CUPS) en una cadena de 22 caracteres, el tipo de contrato que va a elegir con una cadena de 20 caracteres, la potencia contratada representada en kilovatios con un double de 4 dígitos la parte entera y 2 la parte decimal, la tarifa del catálogo vigente elegida en una cadena de 10 caracteres, el IBAN de una cuenta bancaria a la que domiciliar la factura mensual y por último, mediante variables de tipo Date con formato DD/MM/AA, la fecha de inicio del contrato y, opcionalmente, la fecha de fin. Antes de escribir todo en el sistema se comprueba que el cliente está en estado ‘Activo’ y que no existe ningún contrato en estado ‘Activo’ o ‘Suspendido’ para ese CUPS, además de validar el IBAN, la existencia en el catálogo actual de la tarifa indicada y que la potencia indicada está dentro del rango permitido por la tarifa seleccionada.

Al igual que se puede dar de alta y se pueden actualizar los datos, también se puede dar de baja a un cliente registrado en el sistema indentificándolo mediante su DNI/NIF y cambiando el estado del clientes a ‘Inactivo’, registrando la fecha de la baja en una variable de tipo Date con formato DD/MM/AA y por último exponiendo el motivo de la baja en una cadena de 200 caracteres. Antes de confirmar la baja se comprueba que no existen contratos en estado ‘Activo’ o ‘Suspendido’ asociados a dicho cliente ya que en ese caso no se procesará la baja salvo que esta sea debidamente justificada y aprobada por un administrador, y por tanto se suspenderá el contrato, además de que la fecha de baja debe ser obligatoriamente posterior a la fecha de alta del cliente.

El propósito del subsistema de gestión de clientes no es otro que disponer de una base fiable y actualizada de clientes que permita a la empresa operar sobre ellos con datos únicos y actualizados, reduciendo así la probabilidad de que se cometa cualquier tipo de error.


### Subsistema de gestión de empleados
El subsistema de gestión de empleados deberá almacenar la información de cada empleado, su nombre (en una serie de hasta 20 caracteres), apellidos (en una serie de hasta 40 caracteres), DNI (en una serie de 9 caracteres), número de teléfono(en una serie de hasta 20 caracteres que pueden ser todos numéricos menos el primero que puede ser numérico o un signo +), correo electrónico (en una serie de hasta 30 caracteres), dirección (en una serie de hasta 40 caracteres), posición en la empresa (en una serie de hasta 20 caracteres) y el salario (en una serie de hasta 6 caracteres).

Para contratar a un empleado es necesario que proporcione su nombre, apellidos, número de teléfono, DNI, correo electrónico y posición que ocupará en la empresa, datos que el sistema almacenará, confirmando la creación del contrato o dando un error. Un número de teléfono solo puede pertenecer a un empleado, igual que el correo electrónico. Puede haber varios empleados con la misma posición en la empresa.

Para despedir a un empleado es necesario que proporcione su DNI y se calculará el finiquito 
correspondiente. El sistema eliminará al empleado o dará un error.

El sistema permitirá mostrar una lista de empleados donde se podrá ver su nombre, apellidos, número de teléfono, DNI, correo electrónico y posición que ocupa en la empresa.

En caso de que haya una modificación del contrato, un cambio en las condiciones laborales, como salario, horario o responsabilidades, que debe ser acordado por ambas partes, se deberá proporcionar el DNI del empleado para poder acceder a los datos y modificar los datos correspondientes.

El sistema gestionará el plan de incentivos para empleados para motivar, recompensar y reconocer el buen desempeño de los empleados, donde se llevará un registro de los objetivos mensuales y que empleados los logran, las recompensas podrán ser económicas como bonos o comisiones sobre ventas o no económicos como días adicionales de vacaciones, las recompensas pueden ser individuales o para equipos, se devolverá el DNI del empleado o grupos de empleados a los que se debe proporcionar el incentivo al final de cada mes.

