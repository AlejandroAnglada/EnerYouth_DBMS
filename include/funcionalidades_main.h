#ifndef FUNCIONALIDADES_MAIN_H
#define FUNCIONALIDADES_MAIN_H

// Porfa, portabilidad; recordad que no todos los usuarios usan Windows.
#ifdef _WIN32
    #include <windows.h>
#endif
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string>
#include <vector>
// Para limpiar el buffer cuando use getline (porque yo uso strings y doubles/ints)
#include <limits>
#include "../include/ConexionADB.h"
#include "../include/GestionTransmisionDistribucion.h"
#include "../include/GestionEmpleados.h"
#include "../include/GestionRecursosEnergeticos.h"
#include "../include/GestionClientes.h"

// =============================================
// === TABLAS GENERALES PARA CADA SUBSISTEMA ===
// =============================================

// Crear todas las tablas
void crearTablas(ConexionADB &conexion, SQLHSTMT handler);

// Mostrar contenido de todas las tablas
void mostrarContenidoTablas(ConexionADB &conexion, SQLHSTMT handler);

// Borrar todas las tablas
void borrarTablas(ConexionADB &conexion, SQLHSTMT handler);

// ==================================
// === TRANSMISION Y DISTRIBUCION ===
// ==================================

// Submenú transmisión y distribución de energía
void gestionTransmisionDistribucion(GestionTransmisionDistribucion &hogares);

// Crear trigger actualización de datos hogar
void crearTriggerActualizarDatosHogar(ConexionADB &conexion, SQLHSTMT handler);

// =========================
// === GESTION EMPLEADOS ===
// =========================

// Submenú empleados
void gestionEmpleados(GestionEmpleados &empleados);

// Crear trigger incentivos
void crearTriggerVentas(ConexionADB &conexion, SQLHSTMT handler);

// ====================================
// === GESTION RECURSOS ENERGETICOS ===
// ====================================

// Crear trigger bloqueo cesión potencia
void crearTriggerBloquearCesion(ConexionADB &conexion, SQLHSTMT handler);

// Pequeña función de lógica para confirmar/cancelar las opciones.
bool confirmar(const std::string &seleccion);

// Submenú recursos energéticos
void gestionRecursosEnergeticos(GestionRecursosEnergeticos& gre);

// Inserción de 6 instalaciones de ejemplo
void insertarInstalaciones(ConexionADB& db, SQLHSTMT handler);

// ========================
// === GESTION CLIENTES ===
// ========================

// Submenú clientes
void gestionClientes(GestionClientes &clientes);

// Crear trigger finalización contratos al dar de baja un cliente
void crearTriggerContratosBaja(ConexionADB& conexion, SQLHSTMT handler);

#endif // FUNCIONALIDADES_MAIN_H
