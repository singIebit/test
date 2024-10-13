#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <thread>
#include <mutex>

std::mutex mtx;  // Mutex para proteger o acesso ao vetor de chaves

// Função que converte um número inteiro em uma string hexadecimal
std::string intToHex(unsigned long long int num, int key_range) {
    std::stringstream ss;
    ss << std::hex << std::setw((key_range + 3) / 4) << std::setfill('0') << num;  // Calcula o comprimento baseado em key_range
    return ss.str();
}

// Função que gera as chaves privadas em um intervalo e armazena no vetor compartilhado
void generatePrivateKeysThread(int key_range, unsigned long long start, unsigned long long end, std::vector<std::string>& privateKeys) {
    for (unsigned long long i = start; i < end; i++) {
        std::string key = intToHex(i, key_range);
        std::lock_guard<std::mutex> lock(mtx);  // Protege o acesso ao vetor de chaves
        privateKeys.push_back(key);
    }
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
    if (key_range < 1 || key_range > 160) {
        std::cerr << "Erro: key_range deve estar entre 1 e 64" << std::endl;
        return 1;
    }

    std::cout << "Gerando chaves privadas para key_range = " << key_range << " usando threads...\n";

    unsigned long long totalKeys = pow(2, key_range);
    int num_threads = std::thread::hardware_concurrency();  // Número de threads baseado no hardware
    if (num_threads == 0) num_threads = 1;  // Valor padrão caso não seja possível obter a contagem de threads

    std::vector<std::string> privateKeys;  // Vetor compartilhado para armazenar todas as chaves
    std::vector<std::thread> threads;  // Vetor para armazenar as threads

    unsigned long long keys_per_thread = totalKeys / num_threads;
    unsigned long long start = 0;

    // Cria as threads, dividindo a carga de trabalho
    for (int i = 0; i < num_threads; ++i) {
        unsigned long long end = (i == num_threads - 1) ? totalKeys : start + keys_per_thread;
        threads.push_back(std::thread(generatePrivateKeysThread, key_range, start, end, std::ref(privateKeys)));
        start = end;
    }

    // Aguarda todas as threads terminarem
    for (auto& t : threads) {
        t.join();
    }

    // Exibe as chaves geradas
    displayPrivateKeys(privateKeys);

    return 0;
}