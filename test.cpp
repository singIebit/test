#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <omp.h>

// Função que converte um número inteiro em uma string hexadecimal
std::string intToHex(unsigned long long int num, int key_range) {
    std::stringstream ss;
    ss << std::hex << std::setw((key_range + 3) / 4) << std::setfill('0') << num;  // Calcula o comprimento baseado em key_range
    return ss.str();
}

// Função que exibe as chaves privadas geradas
void displayPrivateKeys(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        std::cout << key << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <key_range>" << std::endl;
        return 1;
    }

    int key_range = std::stoi(argv[1]);

    // Verifica se o key_range é válido
    if (key_range < 1 || key_range > 64) {
        std::cerr << "Erro: key_range deve estar entre 1 e 64" << std::endl;
        return 1;
    }

    std::cout << "Gerando chaves privadas para key_range = " << key_range << " usando OpenMP...\n";

    unsigned long long totalKeys = pow(2, key_range);

    // Pré-aloca o vetor de chaves
    std::vector<std::string> privateKeys(totalKeys);

    // Paraleliza o loop usando OpenMP
    #pragma omp parallel for
    for (unsigned long long i = 0; i < totalKeys; i++) {
        privateKeys[i] = intToHex(i, key_range);  // Preenche cada chave em sua posição correta
    }

    // Exibe as chaves geradas (opcional)
    displayPrivateKeys(privateKeys);

    return 0;
}