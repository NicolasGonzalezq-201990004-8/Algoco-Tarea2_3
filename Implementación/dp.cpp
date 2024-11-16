#include <iostream>
#include <string>
#include <algorithm>
#include <climits>  
#include <vector>
#include <fstream>
#include <chrono>

// Vectores y matrices de costos
std::vector<int> cost_insert(26);  
std::vector<int> cost_delete(26);  
std::vector<std::vector<int>> cost_replace(26, std::vector<int>(26)); 
std::vector<std::vector<int>> cost_transpose(26, std::vector<int>(26)); 

// Funciones de costos
int costo_sub(char a, char b) {
    return cost_replace[a - 'a'][b - 'a'];
}

int costo_ins(char b) {
    return cost_insert[b - 'a'];
}

int costo_del(char a) {
    return cost_delete[a - 'a'];
}

int costo_trans(char a, char b) {
    return cost_transpose[a - 'a'][b - 'a'];
}

// Función para cargar el vector de costos
bool loadCostVector(const std::string& filename, std::vector<int>& costVector) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error abriendo el archivo " << filename << std::endl;
        return false;
    }
    for (int& cost : costVector) {
        file >> cost; 
    }
    file.close();
    return true;
}

// Función para cargar la matriz de costos
bool loadCostMatrix(const std::string& filename, std::vector<std::vector<int>>& costMatrix) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error abriendo el archivo " << filename << std::endl;
        return false;
    }
    for (int i = 0; i < 26; ++i) {
        for (int j = 0; j < 26; ++j) {
            file >> costMatrix[i][j];  
        }
    }
    file.close();
    return true;
}

// Algoritmo DP para calcular el costo de edición
int calcularCostoDP(const std::string& S1, const std::string& S2) {
    int m = S1.size();
    int n = S2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, INT_MAX));

    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == 0) {
                dp[i][j] = j > 0 ? dp[i][j - 1] + costo_ins(S2[j - 1]) : 0;
            } else if (j == 0) {
                dp[i][j] = i > 0 ? dp[i - 1][j] + costo_del(S1[i - 1]) : 0;
            } else {
                int insertCost = dp[i][j - 1] + costo_ins(S2[j - 1]);
                int deleteCost = dp[i - 1][j] + costo_del(S1[i - 1]);
                int replaceCost = dp[i - 1][j - 1] + costo_sub(S1[i - 1], S2[j - 1]);

                dp[i][j] = std::min({insertCost, deleteCost, replaceCost});

                if (i > 1 && j > 1 && S1[i - 1] == S2[j - 2] && S1[i - 2] == S2[j - 1]) {
                    int transposeCost = dp[i - 2][j - 2] + costo_trans(S1[i - 1], S1[i - 2]);
                    dp[i][j] = std::min(dp[i][j], transposeCost);
                }
            }
        }
    }
    return dp[m][n];
}

// Función para procesar casos desde un archivo de texto
void procesarCasos(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::string S1, S2;

    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << filename << std::endl;
        return;
    }

    // Leer el archivo caso por caso
    while (getline(file, line)) {
        if (line.find("S1:") != std::string::npos) {
            S1 = line.substr(4, line.size() - 5);  // Extraer la cadena entre comillas
        } 
        if (line.find("S2:") != std::string::npos) {
            S2 = line.substr(4, line.size() - 5);  // Extraer la cadena entre comillas
        }

        // Si ambos S1 y S2 están leídos, calculamos el costo
        if (!S1.empty() && !S2.empty()) {
            auto start = std::chrono::high_resolution_clock::now();
            int distance = calcularCostoDP(S1, S2);  // Usar DP en lugar de fuerza bruta
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            // Escribir el resultado en la consola
            std::cout << "S1: '" << S1 << "'\n";
            std::cout << "S2: '" << S2 << "'\n";
            std::cout << "Distancia mínima de edición: " << distance << std::endl;
            std::cout << "Tiempo de ejecución: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " [µs]" << std::endl;

            // Limpiar para el siguiente caso
            S1.clear();
            S2.clear();
        }
    }
    file.close();
}

int main() {
    // Cargar archivos de costos
    if (!loadCostVector("cost_insert.txt", cost_insert)) return 1;
    if (!loadCostVector("cost_delete.txt", cost_delete)) return 1;
    if (!loadCostMatrix("cost_replace.txt", cost_replace)) return 1;
    if (!loadCostMatrix("cost_transpose.txt", cost_transpose)) return 1;

    // Procesar los casos
    procesarCasos("casos.txt");

    return 0;
}
