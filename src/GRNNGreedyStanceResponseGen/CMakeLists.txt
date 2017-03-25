
include_directories(
  model
)

add_executable(GRNNGreedyStanceResponseGen GRNNGreedyStanceResponseGen.cpp)
target_link_libraries(GRNNGreedyStanceResponseGen ${LIBS})

