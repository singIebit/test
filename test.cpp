#define CL_TARGET_OPENCL_VERSION 300
#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>
#include <CL/opencl.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>

std::string intToHex(unsigned long long num, int key_range) {
    static const char hex_chars[] = "0123456789abcdef";
    int hex_digits = (key_range + 3) / 4;
    std::string hex_str(hex_digits, '0');
    for (int i = hex_digits - 1; i >= 0; --i) {
        hex_str[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    return hex_str;
}

const char* kernelSource = R"(
__kernel void gen_keys(__global uint64_t* indices, __global char* results, int key_range, int hex_digits) {
    int gid = get_global_id(0);
    uint64_t num = indices[gid];
    const char hex_chars[] = "0123456789abcdef";
    
    for (int i = hex_digits - 1; i >= 0; --i) {
        results[gid * hex_digits + i] = hex_chars[num & 0xF];
        num >>= 4;
    }
}
)";

void genKeys(unsigned long long totalKeys, int key_range, int buffer_size) {
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        cl::Platform platform = platforms.front();
        
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        cl::Device device = devices.front();

        std::cout << "Utilizando dispositivo: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

        cl::Context context(device);
        cl::CommandQueue queue(context, device);

        cl::Program::Sources sources;
        sources.push_back({kernelSource, strlen(kernelSource)});
        cl::Program program(context, sources);
        program.build({device});

        std::vector<uint64_t> indices(buffer_size);
        std::vector<char> results(buffer_size * ((key_range + 3) / 4));

        cl::Buffer bufferIndices(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uint64_t) * buffer_size, indices.data());
        cl::Buffer bufferResults(context, CL_MEM_WRITE_ONLY, sizeof(char) * results.size());

        cl::Kernel kernel(program, "gen_keys");
        kernel.setArg(0, bufferIndices);
        kernel.setArg(1, bufferResults);
        kernel.setArg(2, key_range);
        kernel.setArg(3, (key_range + 3) / 4);

        unsigned long long remainingKeys = totalKeys;
        unsigned long long processedKeys = 0;

        auto startTime = std::chrono::high_resolution_clock::now();

        while (remainingKeys > 0) {
            unsigned long long chunkSize = std::min(remainingKeys, static_cast<unsigned long long>(buffer_size));

            for (unsigned long long i = 0; i < chunkSize; ++i) {
                indices[i] = processedKeys + i;
            }

            queue.enqueueWriteBuffer(bufferIndices, CL_TRUE, 0, sizeof(uint64_t) * chunkSize, indices.data());
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(chunkSize), cl::NullRange);
            queue.enqueueReadBuffer(bufferResults, CL_TRUE, 0, sizeof(char) * chunkSize * ((key_range + 3) / 4), results.data());

            for (unsigned long long i = 0; i < chunkSize; ++i) {
                std::string key(results.begin() + i * ((key_range + 3) / 4), results.begin() + (i + 1) * ((key_range + 3) / 4));
                std::cout << key << std::endl;
            }

            processedKeys += chunkSize;
            remainingKeys -= chunkSize;

            auto currentTime = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() >= 10) {
                std::cout << "\rProcess key: " << intToHex(processedKeys, key_range) << " (" << processedKeys << "/" << totalKeys << ")" << std::flush;
                startTime = currentTime;
            }
        }
    } catch (cl::Error& e) {
        std::cerr << "Error OpenCL: " << e.what() << " (" << e.err() << ")\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Use: " << argv[0] << " <key_range>" << std::endl;
        return 1;
    }

    int key_range = std::stoi(argv[1]);

    if (key_range < 1 || key_range > 160) {
        std::cerr << "Error: Range must be 160 or less" << std::endl;
        return 1;
    }

    std::cout << "Range " << key_range << ", starting...\n";

    unsigned long long totalKeys = 1ULL << key_range;
    int buffer_size = 1024;

    genKeys(totalKeys, key_range, buffer_size);

    return 0;
}
