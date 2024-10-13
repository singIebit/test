#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>

// Função que converte um número inteiro em uma string hexadecimal
std::string intToHex(unsigned long long int num, int key_range) {
    std::stringstream ss;
    ss << std::hex << std::setw((key_range + 3) / 4) << std::setfill('0') << num; // Calcula o comprimento baseado em key_range
    return ss.str();
}

// Função que gera as chaves privadas e retorna em um vetor
std::vector<std::string> generatePrivateKeys(int key_range) {
    std::vector<std::string> privateKeys;
    unsigned long long totalKeys = pow(2, key_range); // Calcula o número total de combinações 2^key_range

    for (unsigned long long i = 0; i < totalKeys; i++) {
        privateKeys.push_back(intToHex(i, key_range)); // Gera uma chave em hexadecimal e armazena
    }

    return privateKeys;
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

    std::cout << "Gerando chaves privadas para key_range = " << key_range << "...\n";

    std::vector<std::string> privateKeys = generatePrivateKeys(key_range);
    displayPrivateKeys(privateKeys);

    return 0;
}
