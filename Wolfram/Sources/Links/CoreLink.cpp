#include "Links/CoreLink.h"

int WolframLibrary_initialize(WolframLibraryData libData) {
    try {
        LLU::LibraryData::setLibraryData(libData);
        LLU::ErrorManager::registerPacletErrors(LLU::PacletErrors);
    } catch (const LLU::LibraryLinkError &e) {
        return e.which();
    }
    return LLU::ErrorCode::NoError;
}
