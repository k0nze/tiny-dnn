cmake_minimum_required( VERSION 3.2 )

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)
project( get_tiny_dnn_model_from_caffe )

find_package( Protobuf REQUIRED )
# generate CPP file for connexion between protobuf and tiny_dnn 
PROTOBUF_GENERATE_CPP( PROTO_SRC PROTO_HDR ${CMAKE_CURRENT_SOURCE_DIR}/../../tiny_dnn/io/caffe/caffe.proto )

# set the executable (main cpp + protobuf generated caffe.pb.h/caffe.pb.cc )
add_executable( get_tiny_dnn_model_from_caffe caffe_converter.cpp ${PROTO_SRC} ${PROTO_HDR} )

# Set the include dir (binary for caffe.pb.h, protobuf and location of tiny-dnn source)
target_include_directories( get_tiny_dnn_model_from_caffe PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                               ${PROTOBUF_INCLUDE_DIR} 
                                               ${CMAKE_CURRENT_SOURCE_DIR}/../../ )

# Link with protobuf 
target_link_libraries( get_tiny_dnn_model_from_caffe ${PROTOBUF_LIBRARY} pthread )

# Set C++11 mode 
set_target_properties( get_tiny_dnn_model_from_caffe PROPERTIES CXX_STANDARD 11 )

cotire(get_tiny_dnn_model_from_caffe)
