#include <iostream>
#include <string>
#include <algorithm>   // max

using namespace std;

// ==========================================
// 1. CLASE PARA EL SEGUIMIENTO/SANCIÓN (Lista enlazada interna)
// ==========================================
struct DetalleSancion {
    string descripcion; 
    string hora;
    DetalleSancion* siguiente;

    DetalleSancion(string desc, string h) {
        descripcion = desc;
        hora = h;
        siguiente = NULL;
    }
};

// ==========================================
// 2. CLASE PARA EL INFRACTOR (Nodo del Árbol BST)
// ==========================================
struct InfractorCivico {
    string folio;         
    string nombreInfractor;
    string faltaCometida; 
    string coloniaRemision; 
    DetalleSancion* cabezaSanciones; 
    int alt;              // altura del nodo (solo la usa el AVL)
    
    InfractorCivico* izquierdo;
    InfractorCivico* derecho;

    InfractorCivico(string f, string nombre, string falta, string colonia) {
        folio = f;
        nombreInfractor = nombre;
        faltaCometida = falta;
        coloniaRemision = colonia;
        cabezaSanciones = NULL;
        alt = 1;
        izquierdo = NULL;
        derecho = NULL;
    }

    void agregarSancion(string desc, string hora) {
        DetalleSancion* nuevaSancion = new DetalleSancion(desc, hora);
        if (cabezaSanciones == NULL) {
            cabezaSanciones = nuevaSancion;
        } else {
            DetalleSancion* temp = cabezaSanciones;
            while (temp->siguiente != NULL) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevaSancion;
        }
        cout << "[+] Sancion/Estatus registrado correctamente para el folio " << folio << endl;
    }

    void mostrarSeguimiento() {
        cout << "\n--- SEGUIMIENTO DEL INFRACTOR (Folio: " << folio << ") ---" << endl;
        if (cabezaSanciones == NULL) {
            cout << "No hay sanciones registradas aun." << endl;
            return;
        }
        DetalleSancion* temp = cabezaSanciones;
        int i = 1;
        while (temp != NULL) {
            cout << i++ << ". Hora: " << temp->hora << " | Accion: " << temp->descripcion << endl;
            temp = temp->siguiente;
        }
    }
};

// ==========================================
// 2.5 COLA DE AUDIENCIAS (FIFO, lista enlazada con frente y final)
// ==========================================
struct NodoCola {
    string folio;
    NodoCola* siguiente;
    NodoCola(string f) { folio = f; siguiente = NULL; }
};

class ColaAudiencias {
private:
    NodoCola* frente;
    NodoCola* final;
public:
    ColaAudiencias() { frente = NULL; final = NULL; }

    bool estaVacia() { return frente == NULL; }

    // Entra por el final: O(1)
    void encolar(string folio) {
        NodoCola* nuevo = new NodoCola(folio);
        if (final == NULL) { frente = nuevo; final = nuevo; }
        else { final->siguiente = nuevo; final = nuevo; }
    }

    // Sale por el frente: O(1). Precondicion: la cola no esta vacia.
    string desencolar() {
        NodoCola* temp = frente;
        string folio = temp->folio;
        frente = frente->siguiente;
        if (frente == NULL) final = NULL;
        delete temp;
        return folio;
    }
};

// ==========================================
// 3. CLASE GESTORA (Estructura del Árbol BST)
// ==========================================
class JuzgadoCivicoBST {
private:
    InfractorCivico* raiz;
    bool modoAVL;              // false = BST simple, true = AVL balanceado
    ColaAudiencias cola;

    // ---- AVL: altura, rotaciones y balanceo ----
    int altura(InfractorCivico* n) { return n == NULL ? 0 : n->alt; }
    void actualizar(InfractorCivico* n) { n->alt = 1 + max(altura(n->izquierdo), altura(n->derecho)); }
    int balance(InfractorCivico* n) { return n == NULL ? 0 : altura(n->izquierdo) - altura(n->derecho); }

    InfractorCivico* rotarDerecha(InfractorCivico* y) {
        InfractorCivico* x = y->izquierdo;
        y->izquierdo = x->derecho;
        x->derecho = y;
        actualizar(y); actualizar(x);
        return x;
    }

    InfractorCivico* rotarIzquierda(InfractorCivico* x) {
        InfractorCivico* y = x->derecho;
        x->derecho = y->izquierdo;
        y->izquierdo = x;
        actualizar(x); actualizar(y);
        return y;
    }

    // En BST simple solo actualiza la altura. En AVL ademas rota si hace falta.
    InfractorCivico* balancear(InfractorCivico* n) {
        actualizar(n);
        if (!modoAVL) return n;
        int fb = balance(n);
        if (fb > 1) {
            if (balance(n->izquierdo) < 0) n->izquierdo = rotarIzquierda(n->izquierdo);
            return rotarDerecha(n);
        }
        if (fb < -1) {
            if (balance(n->derecho) > 0) n->derecho = rotarDerecha(n->derecho);
            return rotarIzquierda(n);
        }
        return n;
    }

    // Inserta un nodo ya creado (se usa al cambiar de BST a AVL y viceversa)
    InfractorCivico* insertarNodo(InfractorCivico* actual, InfractorCivico* nodo) {
        if (actual == NULL) return nodo;
        if (nodo->folio < actual->folio) actual->izquierdo = insertarNodo(actual->izquierdo, nodo);
        else actual->derecho = insertarNodo(actual->derecho, nodo);
        return balancear(actual);
    }

    // Recorre en pre-orden y vuelve a insertar cada nodo en el arbol nuevo
    void reconstruir(InfractorCivico* n, InfractorCivico*& nueva) {
        if (n == NULL) return;
        InfractorCivico* izq = n->izquierdo;
        InfractorCivico* der = n->derecho;
        n->izquierdo = NULL; n->derecho = NULL; n->alt = 1;
        nueva = insertarNodo(nueva, n);
        reconstruir(izq, nueva);
        reconstruir(der, nueva);
    }

    InfractorCivico* insertarRec(InfractorCivico* nodo, string folio, string nombre, string falta, string colonia, bool& exito) {
        if (nodo == NULL) {
            exito = true;
            return new InfractorCivico(folio, nombre, falta, colonia);
        }

        if (folio < nodo->folio) {
            nodo->izquierdo = insertarRec(nodo->izquierdo, folio, nombre, falta, colonia, exito);
        } else if (folio > nodo->folio) {
            nodo->derecho = insertarRec(nodo->derecho, folio, nombre, falta, colonia, exito);
        } else {
            exito = false; 
        }
        return balancear(nodo);   // BST: actualiza altura | AVL: rota si hace falta
    }

    InfractorCivico* buscarRec(InfractorCivico* nodo, string folio) {
        if (nodo == NULL || nodo->folio == folio) {
            return nodo;
        }
        if (folio < nodo->folio) {
            return buscarRec(nodo->izquierdo, folio);
        }
        return buscarRec(nodo->derecho, folio);
    }

    void mostrarInOrderRec(InfractorCivico* nodo) {
        if (nodo != NULL) {
            mostrarInOrderRec(nodo->izquierdo);
            cout << "Folio: " << nodo->folio 
                 << " | Infractor: " << nodo->nombreInfractor 
                 << " | Falta: " << nodo->faltaCometida 
                 << " | Colonia: " << nodo->coloniaRemision << endl;
            mostrarInOrderRec(nodo->derecho);
        }
    }

    void liberarMemoriaNodo(InfractorCivico* nodo) {
        DetalleSancion* sancTemp = nodo->cabezaSanciones;
        while (sancTemp != NULL) {
            DetalleSancion* sigSanc = sancTemp->siguiente;
            delete sancTemp;
            sancTemp = sigSanc;
        }
        delete nodo;
    }

    InfractorCivico* encontrarMinimo(InfractorCivico* nodo) {
        InfractorCivico* actual = nodo;
        while (actual && actual->izquierdo != NULL) {
            actual = actual->izquierdo;
        }
        return actual;
    }

    InfractorCivico* eliminarRec(InfractorCivico* nodo, string folio, bool& encontrado) {
        if (nodo == NULL) {
            encontrado = false;
            return NULL;
        }

        if (folio < nodo->folio) {
            nodo->izquierdo = eliminarRec(nodo->izquierdo, folio, encontrado);
        } else if (folio > nodo->folio) {
            nodo->derecho = eliminarRec(nodo->derecho, folio, encontrado);
        } else {
            encontrado = true;
            if (nodo->izquierdo == NULL) {
                InfractorCivico* temp = nodo->derecho;
                liberarMemoriaNodo(nodo);
                return temp;
            } else if (nodo->derecho == NULL) {
                InfractorCivico* temp = nodo->izquierdo;
                liberarMemoriaNodo(nodo);
                return temp;
            }

            InfractorCivico* temp = encontrarMinimo(nodo->derecho);
            nodo->folio = temp->folio;
            nodo->nombreInfractor = temp->nombreInfractor;
            nodo->faltaCometida = temp->faltaCometida;
            nodo->coloniaRemision = temp->coloniaRemision;
            nodo->cabezaSanciones = temp->cabezaSanciones; 
            
            temp->cabezaSanciones = NULL; 
            nodo->derecho = eliminarRec(nodo->derecho, temp->folio, encontrado);
        }
        return balancear(nodo);
    }

public:
    JuzgadoCivicoBST() {
        raiz = NULL;
        modoAVL = false;
    }

    int alturaArbol() { return altura(raiz); }
    bool esAVL() { return modoAVL; }

    void cambiarModo() {
        modoAVL = !modoAVL;
        InfractorCivico* vieja = raiz;
        raiz = NULL;
        reconstruir(vieja, raiz);
        cout << "[OK] Ahora el arbol es " << (modoAVL ? "AVL balanceado" : "BST simple")
             << ". Altura: " << altura(raiz) << endl;
    }

    // Atiende al primero de la cola. Si ya fue liberado, se salta.
    void atenderSiguiente() {
        while (!cola.estaVacia()) {
            string folio = cola.desencolar();
            InfractorCivico* inf = buscarRec(raiz, folio);
            if (inf != NULL) {
                cout << "[AUDIENCIA] Turno de " << inf->folio << " - " << inf->nombreInfractor
                     << " (" << inf->faltaCometida << ")" << endl;
                return;
            }
        }
        cout << "[!] No hay personas esperando audiencia." << endl;
    }

    void registrarInfractor(string folio, string nombre, string falta, string colonia) {
        bool exito = false;
        raiz = insertarRec(raiz, folio, nombre, falta, colonia, exito);
        if (exito) {
            cola.encolar(folio);   // entra por el final de la cola de audiencias
            cout << "[OK] Infractor registrado con folio " << folio << " con exito en el arbol." << endl;
        } else {
            cout << "[!] Error: Ya existe un registro con el folio " << folio << endl;
        }
    }

    InfractorCivico* buscarInfractor(string folio) {
        return buscarRec(raiz, folio);
    }

    void mostrarTodosOrdenados() {
        if (raiz == NULL) {
            cout << "\n[!] No hay infractores en el Juzgado Civico actualmente." << endl;
            return;
        }
        cout << "\n==================================================" << endl;
        cout << "   LISTADO DE INFRACTORES (ORDENADOS POR FOLIO)   " << endl;
        cout << "==================================================" << endl;
        mostrarInOrderRec(raiz);
        cout << "Altura del arbol: " << altura(raiz) << " (" << (modoAVL ? "AVL" : "BST simple") << ")" << endl;
    }

    void liberarInfractor(string folio) {
        bool encontrado = false;
        raiz = eliminarRec(raiz, folio, encontrado);
        if (encontrado) {
            cout << "[OK] Infractor con folio " << folio << " liberado y eliminado del arbol." << endl;
        } else {
            cout << "[!] No se encontro el registro con folio: " << folio << endl;
        }
    }
};

// ==========================================
// 4. FUNCIÓN PRINCIPAL (Menú Interactivo)
// ==========================================
int main() {
    JuzgadoCivicoBST juzgado;
    int opcion;
    string folio, nombre, falta, colonia, desc, hora;

    do {
        cout << "\n==================================================" << endl;
        cout << "  JUZGADO CIVICO - FALTAS ADMINISTRATIVAS         " << endl;
        cout << "==================================================" << endl;
        cout << "1. Registrar nuevo infractor remitido" << endl;
        cout << "2. Buscar infractor por Folio (Busqueda BST)" << endl;
        cout << "3. Agregar estatus/sancion a un infractor" << endl;
        cout << "4. Ver todos los infractores (Ordenados In-Order)" << endl;
        cout << "5. Liberar / Concluir caso de infractor" << endl;
        cout << "6. Atender siguiente audiencia (Cola)" << endl;
        cout << "7. Cambiar tipo de arbol (BST simple / AVL)" << endl;
        cout << "0. Salir" << endl;
        cout << "Elige una opcion: ";
        cin >> opcion;
        cin.ignore(); 

        switch (opcion) {
            case 1:
                cout << "Ingrese Folio (ej. JC-101): ";
                getline(cin, folio);
                cout << "Nombre del Infractor: ";
                getline(cin, nombre);
                cout << "Falta cometida (ej. Ingerir alcohol en via publica): ";
                getline(cin, falta);
                cout << "Colonia de remision (ej. Centro Historico): ";
                getline(cin, colonia);
                juzgado.registrarInfractor(folio, nombre, falta, colonia);
                break;

            case 2:
                cout << "Ingrese el Folio a buscar: ";
                getline(cin, folio);
                {
                    InfractorCivico* encontrado = juzgado.buscarInfractor(folio);
                    if (encontrado != NULL) {
                        cout << "\n[ENCONTRADO] Folio: " << encontrado->folio 
                             << " | Nombre: " << encontrado->nombreInfractor 
                             << " | Falta: " << encontrado->faltaCometida 
                             << " | Colonia: " << encontrado->coloniaRemision << endl;
                        encontrado->mostrarSeguimiento();
                    } else {
                        cout << "[!] Infractor no encontrado en el arbol." << endl;
                    }
                }
                break;

            case 3:
                cout << "Ingrese el Folio del infractor: ";
                getline(cin, folio);
                {
                    InfractorCivico* encontrado = juzgado.buscarInfractor(folio);
                    if (encontrado != NULL) {
                        cout << "Descripcion de la sancion/avance (ej. Cumple 12 horas de arresto): ";
                        getline(cin, desc);
                        cout << "Hora (ej. 16:00 hrs): ";
                        getline(cin, hora);
                        encontrado->agregarSancion(desc, hora);
                    } else {
                        cout << "[!] Infractor no encontrado." << endl;
                    }
                }
                break;

            case 4:
                juzgado.mostrarTodosOrdenados();
                break;

            case 5:
                cout << "Ingrese el Folio del infractor a liberar: ";
                getline(cin, folio);
                juzgado.liberarInfractor(folio);
                break;

            case 6:
                juzgado.atenderSiguiente();
                break;

            case 7:
                juzgado.cambiarModo();
                break;

            case 0:
                cout << "Saliendo del sistema..." << endl;
                break;

            default:
                cout << "[!] Opcion invalida. Intente de nuevo." << endl;
        }
    } while (opcion != 0);

    return 0;
}