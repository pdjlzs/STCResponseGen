
include_directories(
  model
)

add_executable(CNNGreedyResponseGen CNNGreedyResponseGen.cpp)
target_link_libraries(CNNGreedyResponseGen ${LIBS})

