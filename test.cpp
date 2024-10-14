#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <future>
#include <thread>

// Função que converte um número inteiro em uma string hexadecimal
std::string intToHex(unsigned long long int num, int key_range) {
    std::stringstream ss;
    ss << std::hex << std::setw((key_range + 3) / 4) << std::setfill('0') << num;  // Calcula o comprimento baseado em key_range
    return ss.str();
}

// Função que gera as chaves privadas em um intervalo e retorna um vetor local
std::vector<std::string> generatePrivateKeysThread(int key_range, unsigned long long start, unsigned long long end) {
    std::vector<std::string> localKeys;
    for (unsigned long long i = start; i < end; i++) {
        localKeys.push_back(intToHex(i, key_range));
    }
    return localKeys;
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

    std::cout << "Gerando chaves privadas para key_range = " << key_range << " usando std::async...\n";

    unsigned long long totalKeys = pow(2, key_range);
    int num_threads = std::thread::hardware_concurrency();  // Número de threads baseado no hardware
    if (num_threads == 0) num_threads = 1;  // Valor padrão caso não seja possível obter a contagem de threads

    std::vector<std::future<std::vector<std::string>>> futures;  // Vetor de futuros para armazenar os resultados de cada thread

    unsigned long long keys_per_thread = totalKeys / num_threads;
    unsigned long long start = 0;

    // Cria as tarefas assíncronas, dividindo a carga de trabalho
    for (int i = 0; i < num_threads; ++i) {
        unsigned long long end = (i == num_threads - 1) ? totalKeys : start + keys_per_thread;
        // Usa std::async para iniciar a execução paralela
        futures.push_back(std::async(std::launch::async, generatePrivateKeysThread, key_range, start, end));
        start = end;
    }

    // Armazena as chaves geradas de todas as threads
    std::vector<std::string> privateKeys;
    for (auto& fut : futures) {
        std::vector<std::string> result = fut.get();  // Espera cada tarefa assíncrona terminar e pega o resultado
        privateKeys.insert(privateKeys.end(), result.begin(), result.end());  // Adiciona ao vetor principal
    }

    // Exibe as chaves geradas
    displayPrivateKeys(privateKeys);

    return 0;
}