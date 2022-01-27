set(LLU_ROOT "C:/Program Files/Wolfram Research/LibraryLinkUtilities")
find_package(LLU REQUIRED CONFIG PATH_SUFFIXES LLU)

add_library(NetworkModel NetworkModel.cpp)
