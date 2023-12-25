#include <filesystem>
#include <iostream>
#include <windows.h>
#include "gcl/exporter/fbxexporter.h"
#include "gcl/exporter/fbxexportoptions.h"
#include "gcl/grannyconverterlibrary.h"
#include "gcl/importer/grannyimporter.h"
#include "gcl/importer/grannyimportoptions.h"

namespace fs = std::filesystem;

void setWorkingDirectoryToExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    std::string executablePath = buffer;
    size_t lastSlash = executablePath.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        executablePath = executablePath.substr(0, lastSlash);
    }

    SetCurrentDirectoryA(executablePath.c_str());
}

void processFile(const std::string& filePath) {
    GCL::GrannyConverterLibrary grannyConverterLibrary;

    GCL::Importer::GrannyImportOptions importOptions;
    GCL::Exporter::FbxExportOptions exporterOptions;

    exporterOptions.exportSkeleton = true;
    exporterOptions.exportMaterials = true;
    exporterOptions.exportAnimation = true;

    GCL::Importer::GrannyImporter importer(importOptions);
    importer.importFromFile(filePath.c_str());

    fs::path inputFilePath(filePath);
    std::string outputFileName = (inputFilePath.stem().string() + ".fbx").c_str();

    std::string outputDirectory = inputFilePath.parent_path().string();
    std::string outputPath = (fs::path(outputDirectory) / outputFileName).string();

    GCL::Exporter::FbxExporter exporter(exporterOptions, importer.getScene());

    try {
        exporter.exportToFile(outputPath.c_str());
        std::cout << "Exported: " << outputPath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Export failed with error: " << e.what() << std::endl;
    }
}

void processDirectory(const std::string& directoryPath) {
    GCL::GrannyConverterLibrary grannyConverterLibrary;

    GCL::Importer::GrannyImportOptions importOptions;
    GCL::Exporter::FbxExportOptions exporterOptions;

    exporterOptions.exportSkeleton = true;
    exporterOptions.exportMaterials = true;
    exporterOptions.exportAnimation = true;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() &&
            (entry.path().extension() == ".gr2" || entry.path().extension() == ".GR2")) {
            GCL::Importer::GrannyImporter importer(importOptions);
            importer.importFromFile(entry.path().string().c_str());

            fs::path inputFilePath(entry.path());
            std::string outputFileName = (inputFilePath.stem().string() + ".fbx").c_str();

            std::string outputDirectory = inputFilePath.parent_path().string();
            std::string outputPath = (fs::path(outputDirectory) / outputFileName).string();

            GCL::Exporter::FbxExporter exporter(exporterOptions, importer.getScene());

            try {
                exporter.exportToFile(outputPath.c_str());
                std::cout << "Exported: " << outputPath << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Export failed with error: " << e.what() << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    setWorkingDirectoryToExecutablePath();

    if (argc == 2) {
        // Check if the dragged item is a file or a directory
        if (fs::is_directory(argv[1])) {
            processDirectory(argv[1]);
        }
        else {
            processFile(argv[1]);
        }
        return 0;
    }
    else if (argc == 3) {
        std::string option = argv[1];
        std::string inputPath = argv[2];

        if (option == "-d") {
            processDirectory(inputPath);
        }
        else if (option == "-f") {
            processFile(inputPath);
        }
        else {
            std::cerr << "Usage: ConverterExample -d directory_path OR -f file_path" << std::endl;
            return 1;
        }
        return 0;
    }

    std::cerr << "Usage: ConverterExample -d directory_path OR -f file_path" << std::endl;
    return 1;
}
